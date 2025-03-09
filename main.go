package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"path/filepath"
	"runtime"
	"strings"

	"embed"
)

//go:embed embed
var files embed.FS

var out = flag.String("o", "", "output file")
var exportFlag = flag.String("export", "", "comma-separated list of exported symbols")
var exposeFlag = flag.String("expose", "", "comma-separated list of exposed symbols")
var verbose = flag.Bool("V", false, "verbose output")
var libname = flag.String("lib", "", "library name")
var dir = flag.String("dir", "", "directory for temporary files; uses system tempdir if empty")
var cc = flag.String("cc", "cc", "system C compiler")
var lficc = flag.String("lficc", "", "LFI C compiler (autodetected if empty)")
var arch = flag.String("arch", "", "target architecture (x64, arm64)")
var showExports = flag.Bool("show-exports", false, "show exported symbols and exit")
var genStub = flag.Bool("gen-stub", false, "only generate files needed for the stub (but do not compile)")
var genLib = flag.String("gen-lib", "", "only generate files needed for lib (but do not compile); must provide the stub.elf as input")
var dyn = flag.Bool("dyn", false, "generate dynamic library")

func fatal(v ...interface{}) {
	fmt.Fprintln(os.Stderr, v...)
	os.Exit(1)
}

func Arch(a string) string {
	switch a {
	case "x64", "amd64", "x86-64", "x86_64":
		return "x64"
	case "arm64", "aarch64":
		return "arm64"
	}
	fatal("invalid arch:", a)
	panic("unreachable")
}

func GoArch(a string) string {
	switch Arch(a) {
	case "x64":
		return "amd64"
	case "arm64":
		return "arm64"
	}
	panic("unreachable")
}

func GNUArch(a string) string {
	switch Arch(a) {
	case "x64":
		return "x86_64"
	case "arm64":
		return "aarch64"
	}
	panic("unreachable")
}

func main() {
	objmap := make(map[string]string)
	flag.Func("map", "map object to file", func(s string) error {
		parts := strings.SplitN(s, "=", 2)
		if len(parts) != 2 {
			return fmt.Errorf("invalid format: %s", s)
		}
		objmap[parts[0]] = parts[1]
		return nil
	})

	flag.Parse()

	log.SetFlags(0)
	if !*verbose {
		log.SetOutput(io.Discard)
	}

	args := flag.Args()
	if len(args) <= 0 {
		fatal("no input")
	}

	if *arch == "" {
		*arch = runtime.GOARCH
	}

	lib := args[0]
	f, err := os.Open(lib)
	if err != nil {
		fatal(err)
	}

	exportSet := make(map[string]bool)
	if *exportFlag != "" {
		syms := strings.Split(*exportFlag, ",")
		for _, s := range syms {
			exportSet[s] = true
		}
	}

	var exports []string
	if strings.HasSuffix(lib, ".a") {
		exports = StaticGetExports(f, exportSet)
	} else if strings.HasSuffix(lib, ".so") {
		exports = DynamicGetExports(f, exportSet)
	} else {
		fatal("expected *.a or *.so file, got", lib)
	}
	log.Println("exports:", exports)

	if *showExports {
		for _, s := range exports {
			fmt.Println(s)
		}
		os.Exit(0)
	}

	if *libname == "" {
		base := filepath.Base(lib)
		*libname = strings.TrimSuffix(base, filepath.Ext(base)) + "_box"
	}

	if *lficc == "" {
		*lficc = GNUArch(*arch) + "-lfi-linux-musl-clang"
	}

	if *exposeFlag != "" {
		exposed = append(exposed, strings.Split(*exposeFlag, ",")...)
	}

	dir := WriteFiles(*dir, *libname, "embed/stub", exports, exposed, objmap)

	if *genStub {
		os.Exit(0)
	}

	static := true
	if filepath.Ext(lib) == ".so" {
		static = false
	}

	if *genLib != "" {
		objmap["stub"] = *genLib
	} else {
		inlibname := strings.TrimSuffix(filepath.Base(lib), filepath.Ext(filepath.Base(lib)))
		objmap["stub"] = CompileStub(dir, inlibname, lib, static)
	}

	if *out == "" {
		*out = *libname + filepath.Ext(lib)
	}

	WriteFiles(dir, *libname, "embed/lib", exports, exposed, objmap)

	if static && !*dyn {
		CompileStaticLib(dir, *out)
	} else {
		CompileDynamicLib(dir, *out)
	}
}
