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
* The function inside the sandbox executes, and then executes `ret`, which takes it to its return function, which invokes the "return" runtime call.
* The return runtime call restores the host stack (saved in the active LFI process struct), and restores callee-saved registers.
* When the return runtime call attempts to return, the return address points back to the original caller of `bar`, so at this point the function has completed execution and control transfers back to the caller.
