#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "lfi.h"
#include "lfix.h"

enum {
    PATH_MAX = 64,
};

struct File {
    uint8_t* start;
    uint8_t* end;
};

extern char* sbx_filenames[];
extern struct File sbx_filedata[];
extern size_t sbx_nfiles;

extern void* __lfi_trampotable[];
extern size_t __lfi_trampotable_size;

static struct File*
findfile(const char* filename)
{
    for (size_t i = 0; i < sbx_nfiles; i++) {
        if (strncmp(filename, sbx_filenames[i], PATH_MAX) == 0)
            return &sbx_filedata[i];
    }
    return NULL;
}

static void
install_trampotable(void** table)
{
    for (size_t i = 0; i < __lfi_trampotable_size; i++) {
        __lfi_trampotable[i] = table[i];
    }
}

static bool
readfile(const char* filename, uint8_t** data, size_t* size)
{
    struct File* f = findfile(filename);
    if (!f)
        return false;

    *data = f->start;
    *size = f->end - f->start;

    return true;
}

static LFIXEngine engine;

__attribute__((constructor)) void
sbx_init(void)
{
    bool b = lfix_init(&engine);
    if (!b) {
        fprintf(stderr, "error loading LFI: %s\n", lfi_strerror());
        exit(1);
    }
    engine.pause = true;
    engine.readfile = readfile;

    struct File* stub = findfile("stub");
    if (!stub) {
        fprintf(stderr, "error loading LFI: could not find stub file\n");
        exit(1);
    }

    char* args[] = {"stub", NULL};
    LFIXProc* p = lfix_proc_newfile(&engine, stub->start, stub->end - stub->start, 1, &args[0]);
    if (!p) {
        fprintf(stderr, "error creating LFI process: %s\n", lfi_strerror());
        exit(1);
    }

    uint64_t r = lfi_proc_start(p->l_proc);

    // TODO: validate the trampoline table (its location) before installing it,
    // since intalling it involves reading from sandbox memory. Possibly also
    // validate its contents (not entirely necessary if the trampoline uses
    // uses a safe bundle-aligned jump for entry, but probably still a good
    // idea).
    install_trampotable((void**) r);
}

void*
sbx_stackpush(size_t n)
{
    (void) n;
    assert(!"unimplemented");
}

size_t
sbx_stackpop(size_t n)
{
    (void) n;
    assert(!"unimplemented");
}
