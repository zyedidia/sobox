// for memfd_create
#define _GNU_SOURCE
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "lfi.h"
#include "lfix.h"

enum {
    PATH_MAX = 64,
};

struct File {
    uint8_t* start;
    uint8_t* end;
};

static bool cbinit(LFIProc* proc);

extern void sbx_cbtrampoline();

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
        fprintf(stderr, "sobox: error loading LFI: %s\n", lfi_strerror());
        exit(1);
    }
    engine.pause = true;
    engine.readfile = readfile;

    struct File* stub = findfile("stub");
    if (!stub) {
        fprintf(stderr, "sobox: error loading LFI: could not find stub file\n");
        exit(1);
    }

    char* args[] = {"stub", NULL};
    LFIXProc* p = lfix_proc_newfile(&engine, stub->start, stub->end - stub->start, 1, &args[0]);
    if (!p) {
        fprintf(stderr, "sobox: error creating LFI process: %s\n", lfi_strerror());
        exit(1);
    }

    bool ok = cbinit(p->l_proc);
    if (!ok) {
        fprintf(stderr, "sobox: failed to initialize callback entries\n");
        exit(1);
    }

    uint64_t r = lfi_proc_start(p->l_proc);
    if (r < 256) {
        fprintf(stderr, "sobox: failed to start LFI process\n");
        exit(1);
    }

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

enum {
    MAXCALLBACKS = 4096,
};

static void* callbacks[MAXCALLBACKS];

static ssize_t
cbfreeslot()
{
    for (ssize_t i = 0; i < MAXCALLBACKS; i++) {
        if (!callbacks[i])
            return i;
    }
    return -1;
}

static ssize_t
cbfind(void* fn)
{
    for (size_t i = 0; i < MAXCALLBACKS; i++) {
        if (callbacks[i] == fn)
            return i;
    }
    return -1;
}

struct CallbackEntry {
    uint8_t code[16];
    uint64_t target;
    uint64_t trampoline;
};

// Code for a callback trampoline.
static uint8_t cbtrampoline[16] = {
   0x4c, 0x8b, 0x15, 0x09, 0x00, 0x00, 0x00, // mov    0x9(%rip),%r10
   0xff, 0x25, 0x0b, 0x00, 0x00, 0x00,       // jmp    *0xb(%rip)
   0x0f, 0x01f, 0x00,                        // nop
};

static struct CallbackEntry* cbentries_alias;
static struct CallbackEntry* cbentries_box;

static bool
cbinit(LFIProc* proc)
{
    int fd = memfd_create("", 0);
    if (fd < 0)
        return false;
    size_t size = MAXCALLBACKS * sizeof(struct CallbackEntry);
    int r = ftruncate(fd, size);
    if (r < 0)
        goto err;
    // Map callback entries outside the sandbox as read/write.
    void* aliasmap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (aliasmap == (void*) -1)
        goto err;
    cbentries_alias = (struct CallbackEntry*) aliasmap;
    // Fill in the code for each entry.
    for (size_t i = 0; i < MAXCALLBACKS; i++) {
        memcpy(&cbentries_alias[i].code, &cbtrampoline[0], sizeof(cbentries_alias[i].code));
    }
    // Share the mapping inside the sandbox as read/exec.
    void* boxmap = lfi_proc_mapany(proc, size, PROT_READ | PROT_EXEC, MAP_SHARED, fd, 0);
    if (boxmap == (void*) -1)
        goto err1;
    cbentries_box = (struct CallbackEntry*) boxmap;
    return true;
err1:
    munmap(aliasmap, size);
err:
    close(fd);
    return false;
}

void*
sbx_register_cb(void* fn, size_t stackframe)
{
    assert(fn);
    assert(cbfind(fn) == -1 && "fn is already registered as a callback");

    ssize_t slot = cbfreeslot();
    if (slot == -1)
        return NULL;

    // TODO: support non-zero stackframes: create a trampoline for 'stackframe'
    // if it does not exist
    if (stackframe != 0)
        return NULL;

    // write 'fn' into the 'target' field for the chosen slot.
    __atomic_store_n(&cbentries_alias[slot].target, (uint64_t) fn, __ATOMIC_SEQ_CST);
    // write the trampoline into the 'trampoline' field for the chosen slot
    __atomic_store_n(&cbentries_alias[slot].trampoline, (uint64_t) sbx_cbtrampoline, __ATOMIC_SEQ_CST);

    // Mark the slot as allocated.
    callbacks[slot] = fn;

    return &cbentries_box[slot].code[0];
}

void
sbx_unregister_cb(void* fn)
{
    ssize_t slot = cbfind(fn);
    if (slot == -1)
        return;
    callbacks[slot] = NULL;
    __atomic_store_n(&cbentries_alias[slot].target, 0, __ATOMIC_SEQ_CST);
    __atomic_store_n(&cbentries_alias[slot].trampoline, 0, __ATOMIC_SEQ_CST);
}
