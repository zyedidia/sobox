package main

import (
	"io"
	"io/fs"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"sort"
	"strings"
	"text/template"

	"golang.org/x/exp/maps"
)

func ReadEmbed(s string) string {
	data, err := files.ReadFile(s)
	if err != nil {
		fatal(err)
	}
	return string(data)
}

func ExecTemplate(w io.Writer, name string, data string, vars map[string]any, funcs template.FuncMap) {
	tmpl := template.New(name)
	tmpl.Funcs(funcs)
	tmpl, err := tmpl.Parse(data)
	if err != nil {
		fatal(err)
	}
	err = tmpl.Execute(w, vars)
	if err != nil {
		fatal(err)
	}
}

var rgx = regexp.MustCompile("[-./+]")

func ident(s string) string {
	return rgx.ReplaceAllString(s, "__")
}

func GenFile(path string, lib string, exported, exposed []string, filemap map[string]string, w io.Writer) {
	files := maps.Keys(filemap)
	sort.Strings(files)

	ExecTemplate(w, path, ReadEmbed(path), map[string]any{
		"lib":       LibName(lib),
		"exported":  exported,
		"exposed":   exposed,
		"files":     files,
		"nexported": len(exported),
		"nexposed":  len(exposed),
		"nfiles":    len(files),
	}, map[string]any{
		"ident": ident,
		"filename": func(s string) string {
			if name, ok := filemap[s]; ok {
				return name
			}
			fatal("no location given for file", s)
			return ""
		},
	})
}

// uses a temp directory if dir is ""
func WriteFiles(dir, lib, subdir string, exported, exposed []string, filemap map[string]string) string {
	if dir == "" {
		var err error
		dir, err = os.MkdirTemp("", "lfibind-*")
		if err != nil {
			fatal(err)
		}
	}

	os.MkdirAll(filepath.Join(dir, subdir), 0700)

	fs.WalkDir(files, subdir, func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			return err
		}
		tpath := filepath.Join(dir, path)
		if d.IsDir() {
			os.Mkdir(tpath, 0700)
		} else {
			if strings.HasSuffix(path, ".in") {
				// chop off .in
				tpath = tpath[:len(tpath)-3]
			}
			log.Println("creating", tpath)
			f, err := os.Create(tpath)
			if err != nil {
				fatal(err)
			}
			if strings.HasSuffix(path, ".in") {
				GenFile(path, lib, exported, exposed, filemap, f)
			} else {
				b, err := files.ReadFile(path)
				if err != nil {
					fatal(err)
				}
				f.Write(b)
			}
			f.Close()
		}
		return nil
	})
	return dir
}
