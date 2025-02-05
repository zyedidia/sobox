package main

import (
	"fmt"
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
		filepath.Join(dir, fmt.Sprintf("embed/stub/arch/%s/main.s", Arch(*arch))),
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
		Run("patchelf", "--set-interpreter", fmt.Sprintf("/lib/ld-musl-%s.so.1", GNUArch(*arch)), stub)
	}
	return stub
}

func CompileStaticLib(dir, out string) {
	files := []string{
		filepath.Join(dir, "embed/lib/includes.c"),
		filepath.Join(dir, "embed/lib/lib.c"),
		filepath.Join(dir, fmt.Sprintf("embed/lib/arch/%s/callback.s", Arch(*arch))),
		filepath.Join(dir, fmt.Sprintf("embed/lib/arch/%s/trampolines.S", Arch(*arch))),
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
		filepath.Join(dir, fmt.Sprintf("embed/lib/arch/%s/callback.s", Arch(*arch))),
		filepath.Join(dir, fmt.Sprintf("embed/lib/arch/%s/trampolines.S", Arch(*arch))),
		"-llfi", "-O2", "-fPIC", "-shared",
		"-o", out,
	)
}
