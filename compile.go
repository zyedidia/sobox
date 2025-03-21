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
		"-lc++", "-lc++abi",
	}

	if static {
		args = append(args, "-static-pie")
	}
	if *mimalloc {
		args = append(args, "-lmimalloc")
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
		// filepath.Join(dir, fmt.Sprintf("embed/lib/arch/%s/callback.c", Arch(*arch))),
		filepath.Join(dir, fmt.Sprintf("embed/lib/arch/%s/callback.S", Arch(*arch))),
		filepath.Join(dir, fmt.Sprintf("embed/lib/arch/%s/trampolines.S", Arch(*arch))),
	}
	var objs []string
	for _, f := range files {
		Run(*cc, f, "-c", "-O2", "-o", f+".o")
		objs = append(objs, f+".o")
	}
	args := []string{"rcs", out}
	if *nodl {
		args = append(args, "-DNODLOPEN")
	}
	if *single {
		args = append(args, "-DSINGLEBOX")
	}
	if *large {
		args = append(args, "-DLARGEBOX")
	}
	Run("ar", append(args, objs...)...)
}

func CompileDynamicLib(dir, out string) {
	args := []string{
		filepath.Join(dir, "embed/lib/includes.c"),
		filepath.Join(dir, "embed/lib/lib.c"),
		filepath.Join(dir, "embed/lib/jni.c"),
		filepath.Join(dir, fmt.Sprintf("embed/lib/arch/%s/callback.S", Arch(*arch))),
		filepath.Join(dir, fmt.Sprintf("embed/lib/arch/%s/trampolines.S", Arch(*arch))),
		"-I", filepath.Join(dir, "embed/lib"),
		"-O2", "-fPIC", "-shared",
		"-o", out,
	}
	if *nodl {
		args = append(args, "-DNODLOPEN")
	}
	if *single {
		args = append(args, "-DSINGLEBOX")
	}
	if *large {
		args = append(args, "-DLARGEBOX")
	}
	Run(*cc, args...)
}
