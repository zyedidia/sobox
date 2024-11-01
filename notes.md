# How the generated library works

Here's an example trace for how a sandboxed library works in practice:

* The tool produces `libfoo.box.so` from `libfoo.so` (a shared library compiled with the LFI toolchain). In the future the tool will also need a listing of function signatures to handle stack arguments, but for now only arguments passed in registers are supported.
* An executable links against `libfoo.box.so` as if it is `libfoo.so`.
* When the executable starts, a C constructor in `libfoo.box.so` starts the LFI engine as part of its library initialization.
* As part of the initialization, the constructor also starts a "stub" process: an ELF executable that is embedded by the tool in `libfoo.box.so` that was linked with `libfoo.so` when it was compiled.
* When the stub starts, the dynamic linker inside the sandbox loads `libfoo.so` into the sandbox address space (because the stub is linked with `libfoo.so`).
* The stub's main function then creates a table of all exported functions in `libfoo.so`, and then exits.
* Once the stub exits, the `libfoo.box.so` constructor resumes and copies the exported function table out of the sandbox and into some host memory (this becomes the "trampoline table").
* Information about the stub process (its registers, etc..) is stored in the "active LFI process" variable, which is a thread-local.
* The stub also stores a "return" function in its exported function table. The return function in the stub makes a "return" runtime call, which is a special runtime call in liblfi that receives sandbox exits.
* The constructor completes

That is the initialization phase. Now the invocation phase (performance-critical):

* The executable code attempts to call a function called `bar`.
* This goes to the `bar` function in `libfoo.box.so`, which is a trampoline that first reads the address of the internal `bar` from the trampoline table, and then sets up the sandbox registers/stack using the currently active LFI process, a variable stored in thread-local memory. The return address pushed on the stack is the address of the sandbox's return function.
* The trampoline then jumps to the internal `bar` function.
* The function inside the sandbox executes, and then returns (implemented as a bundle-aligned indirect jump), which takes it to its return function (also inside the sandbox), which invokes the "return" runtime call.
* The return runtime call restores the host stack (saved in the active LFI process struct), and restores callee-saved registers.
* When the return runtime call attempts to return, the return address points back to the original caller of `bar`, so at this point the function has completed execution and control transfers back to the caller.

# How the tool works

When you invoke the `sobox` tool, you pass it a shared library that you want to wrap (`libfoo.so`). The tool will generate four files from some templates stored in `embed/` and put them into a `gen` directory.

* `stub.s`: an LFI program that is meant to run inside the sandbox and is linked with `libfoo.so`. This program records all exported functions in `libfoo.so` into a table, and then makes a runtime call to exit. It also includes the code for the "retfn", a function that invokes the return runtime call.
* `includes.c`, `trampolines.s`, `libinit.c`: these comprise the code for the generated library.
    * `includes.c`: used to embed files into the generated library, such as the dynamic linker, `libfoo.so` itself, and the stub.
    * `libinit.c`: contains the library constructor that initializes the LFI engine, runs the stub, and copies the sandbox's trampoline table into the host trampoline table (`__lfi_trampotable`, defined in `trampolines.s`).
    * `trampolines.s`: includes the host trampoline table (`__lfi_trampotable`), as well as trampolines for each exported function. Each function's trampoline loads the sandbox address for the wrapped function from the trampoline table into `%r12` and then jumps to the generic `__lfi_trampoline` routine that switches to the sandbox environment and jumps to `%r12`.

After generating `stub.s`, Sobox invokes the LFI GCC compiler to build it to an ELF program. The compiler is invoked with `libfoo.so` so that the stub gets dynamically linked with the wrapped library.

Next, `includes.c`, `libinit.c` and `trampolines.s` are compiled into `libfoo.box.so`, along with `liblfix` which is also linked in statically. `liblfix` is the library for creating LFI sandboxes and includes handlers for a Linux-compatible system call subset.
