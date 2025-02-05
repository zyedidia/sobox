package main

import (
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

func Run(command string, args ...string) {
	cmd := exec.Command(command, args...)
	log.Println(cmd)
	cmd.Stderr = os.Stderr
	cmd.Stdout = os.Stdout
	cmd.Stdin = os.Stdin
	err := cmd.Run()
	if err != nil {
		fatal(err)
	}
}

func LibName(lib string) string {
	_, after, _ := strings.Cut(lib, "lib")
	return after
}

func CompileStub(dir, lib, libpath string, static bool) string {
	stub := filepath.Join(dir, "stub.elf")

	args := []string{
		filepath.Join(dir, "embed/stub/thread.c"),
		filepath.Join(dir, "embed/stub/arch/x64/main.s"),
		"-o", stub,
		"-L" + filepath.Dir(libpath),
		"-l" + LibName(lib),
		"-O2",
	}

	if static {
		args = append(args, "-static-pie")
	}

	Run(*lficc, args...)
	if !static {
		Run("patchelf", "--set-interpreter", "/lib/ld-musl-x86_64.so.1", stub)
	}
	return stub
}

func CompileStaticLib(dir, out string) {
	files := []string{
		filepath.Join(dir, "embed/lib/includes.c"),
		filepath.Join(dir, "embed/lib/lib.c"),
		filepath.Join(dir, "embed/lib/arch/x64/callback.s"),
		filepath.Join(dir, "embed/lib/arch/x64/trampolines.s"),
	}
	var objs []string
	for _, f := range files {
		Run(*cc, f, "-c", "-O2", "-o", f+".o")
		objs = append(objs, f+".o")
	}
	args := []string{"rcs", out}
	Run("ar", append(args, objs...)...)
}

func CompileDynamicLib(dir, out string) {
	Run(*cc,
		filepath.Join(dir, "embed/lib/includes.c"),
		filepath.Join(dir, "embed/lib/lib.c"),
		filepath.Join(dir, "embed/lib/arch/x64/callback.s"),
		filepath.Join(dir, "embed/lib/arch/x64/trampolines.s"),
		"-llfi", "-O2", "-fPIC", "-shared",
		"-o", out,
	)
}
