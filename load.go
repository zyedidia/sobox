package main

import (
	"bytes"
	"debug/elf"
	"encoding/binary"
	"fmt"
	"io"
	"os"

	"github.com/erikgeiser/ar"
)

var exposed = []string{
	"_lfi_retfn",
	"_lfi_pause",
	"_lfi_thread_create",
	"_lfi_thread_destroy",
	"malloc",
	"calloc",
	"free",
}

func IsExport(sym string, exports map[string]bool) bool {
	return len(exports) > 0 && exports[sym] || len(exports) == 0
}

func ObjGetExports(file *elf.File, es map[string]bool) []string {
	syms, err := file.Symbols()
	if err != nil {
		fatal(err)
	}
	var exports []string
	for _, sym := range syms {
		if IsExport(sym.Name, es) && (elf.ST_BIND(sym.Info) == elf.STB_GLOBAL && elf.ST_TYPE(sym.Info) == elf.STT_FUNC && sym.Section != elf.SHN_UNDEF) {
			if sym.Name == "_init" || sym.Name == "_fini" {
				// Musl inserts these symbols on shared libraries, but after we
				// compile the stub they will be linked internally, and should
				// not be exported.
				continue
			}
			exports = append(exports, sym.Name)
		}
	}
	return exports
}

func DynamicGetExports(dynlib *os.File, es map[string]bool) []string {
	f, err := elf.NewFile(dynlib)
	if err != nil {
		fatal(err)
	}
	return ObjGetExports(f, es)
}

func StaticGetExports(staticlib *os.File, es map[string]bool) []string {
	r, err := ar.NewReader(staticlib)
	if err != nil {
		fatal(err)
	}
	var exports []string
	for {
		_, err := r.Next()
		if err != nil {
			break
		}
		data, err := io.ReadAll(r)
		if err != nil {
			continue
		}
		b := bytes.NewReader(data)
		ef, err := elf.NewFile(b)
		if err != nil {
			continue
		}
		exports = append(exports, ObjGetExports(ef, es)...)
		// ObjGetStackArgs(ef, es)
	}
	return exports
}

type StackArg struct {
	Offset uint64
	Size   uint64
}

func ObjGetStackArgs(file *elf.File, es map[string]bool) map[string][]StackArg {
	sec := file.Section(".stack_args")
	if sec == nil {
		return nil
	}
	entries := sec.Size / 8
	b := make([]byte, 8)
	for i := 0; i < int(entries); i++ {
		sec.ReadAt(b, int64(i*8))
		val := binary.LittleEndian.Uint64(b)
		fmt.Println(file, val)
	}
	return nil
}
