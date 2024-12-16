package main

import (
	"debug/dwarf"
	"debug/elf"
	"fmt"
	"log"
	"os"
)

func main() {
	if len(os.Args) < 2 {
		log.Fatal("Usage: read_dwarf <executable>")
	}

	f, err := elf.Open(os.Args[1])
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()

	data, err := f.DWARF()
	if err != nil {
		log.Fatal(err)
	}

	reader := data.Reader()

	for {
		entry, err := reader.Next()
		if err != nil {
			log.Fatal(err)
		}
		if entry == nil {
			break
		}
		fmt.Println("Entry:", entry)

		if entry.Tag == dwarf.TagSubprogram {
			args := 0
			name := entry.Val(dwarf.AttrName).(string)

			for {
				child, err := reader.Next()
				if err != nil {
					log.Fatal(err)
				}
				if child == nil || child.Tag == 0 {
					break
				}

				if child.Tag == dwarf.TagFormalParameter {
					args++
				}
			}
			fmt.Println(name, "has", args, "arguments")
		}
	}
}
