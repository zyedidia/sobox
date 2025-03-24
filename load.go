package main

import (
	"bytes"
	"debug/elf"
	"encoding/binary"
	"io"
	"log"
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
	"_lfi_jni_CallObjectMethod",
	"_lfi_jni_CallBooleanMethod",
	"_lfi_jni_CallByteMethod",
	"_lfi_jni_CallCharMethod",
	"_lfi_jni_CallShortMethod",
	"_lfi_jni_CallIntMethod",
	"_lfi_jni_CallLongMethod",
	"_lfi_jni_CallFloatMethod",
	"_lfi_jni_CallDoubleMethod",
	"_lfi_jni_CallVoidMethod",
	"_lfi_jni_CallNonvirtualObjectMethod",
	"_lfi_jni_CallNonvirtualBooleanMethod",
	"_lfi_jni_CallNonvirtualByteMethod",
	"_lfi_jni_CallNonvirtualCharMethod",
	"_lfi_jni_CallNonvirtualShortMethod",
	"_lfi_jni_CallNonvirtualIntMethod",
	"_lfi_jni_CallNonvirtualLongMethod",
	"_lfi_jni_CallNonvirtualFloatMethod",
	"_lfi_jni_CallNonvirtualDoubleMethod",
	"_lfi_jni_CallNonvirtualVoidMethod",
	"_lfi_jni_CallStaticObjectMethod",
	"_lfi_jni_CallStaticBooleanMethod",
	"_lfi_jni_CallStaticByteMethod",
	"_lfi_jni_CallStaticCharMethod",
	"_lfi_jni_CallStaticShortMethod",
	"_lfi_jni_CallStaticIntMethod",
	"_lfi_jni_CallStaticLongMethod",
	"_lfi_jni_CallStaticFloatMethod",
	"_lfi_jni_CallStaticDoubleMethod",
	"_lfi_jni_CallStaticVoidMethod",
	"_lfi_jni_NewObject",
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
	ObjGetStackArgs(file, es)
	return exports
}

func DynamicGetExports(dynlib *os.File, es map[string]bool) ([]string, map[string][]StackArg) {
	f, err := elf.NewFile(dynlib)
	if err != nil {
		fatal(err)
	}
	return ObjGetExports(f, es), ObjGetStackArgs(f, es)
}

func StaticGetExports(staticlib *os.File, es map[string]bool) ([]string, map[string][]StackArg) {
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
	}
	return exports, nil
}

type StackArg struct {
	Offset uint32
	Size   uint32
}

func ObjGetStackArgs(file *elf.File, es map[string]bool) map[string][]StackArg {
	sec := file.Section(".stack_args")
	if sec == nil {
		return nil
	}

	syms, err := file.Symbols()
	if err != nil {
		log.Fatal(err)
	}
	symtab := make(map[uint64]string)
	for _, sym := range syms {
		symtab[sym.Value] = sym.Name
	}

	info := make(map[string][]StackArg)

	b := make([]byte, 8)
	idx := uint64(0)
	for idx < sec.Size {
		sec.ReadAt(b, int64(idx))
		idx += 8
		fn := binary.LittleEndian.Uint64(b)

		sec.ReadAt(b, int64(idx))
		idx += 4
		entries := binary.LittleEndian.Uint32(b)

		var args []StackArg
		for i := uint32(0); i < entries; i++ {
			// stack offset
			sec.ReadAt(b, int64(idx))
			idx += 4
			offset := binary.LittleEndian.Uint32(b)
			// size
			sec.ReadAt(b, int64(idx))
			idx += 4
			size := binary.LittleEndian.Uint32(b)

			args = append(args, StackArg{
				Offset: offset,
				Size:   size,
			})
		}

		sym := symtab[fn]
		info[sym] = args
	}

	return info
}
