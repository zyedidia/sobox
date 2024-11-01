package main

import (
	"debug/elf"
	_ "embed"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"sort"
	"strings"

	"golang.org/x/exp/maps"
)

//go:embed incbin.h
var incbin string

//go:embed trampoline.S
var trampoline string

func fatal(err ...interface{}) {
	fmt.Fprintln(os.Stderr, err...)
	os.Exit(1)
}

func run(command string, args ...string) {
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

func ident(s string) string {
	return strings.ReplaceAll(s, ".", "__")
}

func genIncludes(filemap map[string]string, w io.Writer) error {
	files := maps.Keys(filemap)
	sort.Strings(files)

	fmt.Fprintf(w, "#include <stddef.h>\n")
	fmt.Fprintf(w, "#include <stdint.h>\n")
	fmt.Fprintf(w, "%s\n", incbin)
	fmt.Fprintf(w, "char* sbx_filenames[] = {\n")
	for _, file := range files {
		fmt.Fprintf(w, "\t\"%s\",\n", file)
	}
	fmt.Fprintf(w, "};\n")

	for _, file := range files {
		fname, ok := filemap[file]
		if !ok {
			return fmt.Errorf("no location given for file %s\n", file)
		}
		fmt.Fprintf(w, "INCBIN(%s, \"%s\");\n", ident(file), fname)
	}

	fmt.Fprintf(w, "struct File {\n")
	fmt.Fprintf(w, "\tuint8_t* start;\n")
	fmt.Fprintf(w, "\tuint8_t* end;\n")
	fmt.Fprintf(w, "};\n")
	fmt.Fprintf(w, "struct File sbx_filedata[] = {\n")
	for _, file := range files {
		fmt.Fprintf(w, "(struct File){&__lfidata_%s_start[0], &__lfidata_%s_end[0]},\n", ident(file), ident(file))
	}
	fmt.Fprintf(w, "};\n")
	fmt.Fprintf(w, "size_t sbx_nfiles = %d;\n", len(files))
	return nil
}

func genTrampolines(syms []elf.Symbol, w io.Writer) {
	fmt.Fprintf(w, ".section \".text.trampolines\"\n")
	fmt.Fprintf(w, "%s\n", trampoline)
	for _, sym := range syms {
		fmt.Fprintf(w, ".global %s\n", sym.Name)
		fmt.Fprintf(w, "%s:\n", sym.Name)
		fmt.Fprintf(w, "\tmov __lfi_%s@GOTPCREL(%%rip), %%r12\n", sym.Name)
		fmt.Fprintf(w, "\tmov (%%r12), %%r12\n")
		fmt.Fprintf(w, "\tjmp __lfi_trampoline\n")
	}
	fmt.Fprintf(w, ".section \".data.trampolines\"\n")
	fmt.Fprintf(w, ".global __lfi_tplntbl\n")
	fmt.Fprintf(w, "__lfi_tplntbl:\n")
	fmt.Fprintf(w, ".global __lfi_tplntbl_retfn\n")
	fmt.Fprintf(w, "__lfi_tplntbl_retfn:\n")
	fmt.Fprintf(w, ".quad 0\n")
	for _, sym := range syms {
		fmt.Fprintf(w, ".global __lfi_%s\n", sym.Name)
		fmt.Fprintf(w, "__lfi_%s:\n", sym.Name)
		fmt.Fprintf(w, "\t.quad 0\n")
	}
	fmt.Fprintf(w, ".global __lfi_tplntbl_size\n")
	fmt.Fprintf(w, "__lfi_tplntbl_size:\n")
	fmt.Fprintf(w, ".quad %d\n", len(syms)+1) // +1 for retfn
	fmt.Fprintf(w, ".section .note.GNU-stack,\"\",@progbits\n")
}

func genStub(syms []elf.Symbol, w io.Writer) {
	fmt.Fprintf(w, ".section \".text.sbxmain\"\n")
	fmt.Fprintf(w, ".p2align 5\n")
	fmt.Fprintf(w, ".global main\n")
	fmt.Fprintf(w, "main:\n")
	fmt.Fprintf(w, "\tleaq tplntbl(%%rip), %%rdi\n")
	fmt.Fprintf(w, "\tmov $60, %%rax\n")
	fmt.Fprintf(w, "\tsyscall\n")
	fmt.Fprintf(w, "\tint3\n")
	fmt.Fprintf(w, ".p2align 5\n")
	fmt.Fprintf(w, ".global retfn\n")
	fmt.Fprintf(w, "retfn:\n")
	fmt.Fprintf(w, "\tlibret\n")
	fmt.Fprintf(w, "\tint3\n")

	fmt.Fprintf(w, ".section \".data.sbxtbl\"\n")
	fmt.Fprintf(w, ".global tplntbl\n")
	fmt.Fprintf(w, "tplntbl:\n")
	fmt.Fprintf(w, "\t.quad retfn\n")
	for _, sym := range syms {
		fmt.Fprintf(w, "\t.quad %s\n", sym.Name)
	}
	fmt.Fprintf(w, ".section .note.GNU-stack,\"\",@progbits\n")
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
	args := flag.Args()
	if len(args) <= 0 {
		log.Fatal("no input")
	}
	solib := args[0]
	f, err := os.Open(solib)
	if err != nil {
		log.Fatal(err)
	}
	ef, err := elf.NewFile(f)
	if err != nil {
		log.Fatal(err)
	}

	syms, err := ef.Symbols()
	if err != nil {
		log.Fatal(err)
	}

	var exports []elf.Symbol
	for _, sym := range syms {
		if elf.ST_BIND(sym.Info) == elf.STB_GLOBAL && elf.ST_TYPE(sym.Info) == elf.STT_FUNC && sym.Section != elf.SHN_UNDEF {
			if sym.Name == "_init" || sym.Name == "_fini" {
				// Musl inserts these symbols for some reason
				continue
			}
			exports = append(exports, sym)
		}
	}

	gen := "gen"
	os.MkdirAll(gen, os.ModePerm)

	stub, err := os.Create(filepath.Join(gen, "stub.s"))
	if err != nil {
		log.Fatal(err)
	}
	trampolines, err := os.Create(filepath.Join(gen, "trampolines.s"))
	if err != nil {
		log.Fatal(err)
	}
	includes, err := os.Create(filepath.Join(gen, "includes.c"))
	if err != nil {
		log.Fatal(err)
	}
	stubgen := filepath.Join(gen, "stub.elf")
	lficc := "x86_64-lfi-linux-musl-gcc"

	genStub(exports, stub)

	genTrampolines(exports, trampolines)

	stub.Close()
	trampolines.Close()

	run(lficc, stub.Name(), "-o", stubgen, solib)
	objmap["stub"] = stubgen

	objs, err := ef.DynString(elf.DT_NEEDED)
	if err != nil {
		log.Fatal(err)
	}
	for _, obj := range objs {
		if _, ok := objmap[obj]; !ok {
			log.Fatalf("error: library requires %s, but no object provided", obj)
		}
	}

	err = genIncludes(objmap, includes)
	if err != nil {
		log.Fatal(err)
	}

	includes.Close()

	// compile stub with LFI and link to library
	// compile new .so with liblfi, libsobox, trampolines.c, includes.c
}
