#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef void* ht_key_t;
typedef void* ht_val_t;

static inline uint64_t ht_hash(void* keyp) {
    uint64_t key = (uint64_t) keyp;
    key ^= key >> 33;
    key *= 0xff51afd7ed558ccdULL;
    key ^= key >> 33;
    key *= 0xc4ceb9fe1a85ec53ULL;
    key ^= key >> 33;
    return key;
}

static inline bool ht_eq(ht_key_t a, ht_key_t b) {
    return a == b;
}

// ---

typedef struct {
    ht_key_t key;
    ht_val_t val;
    bool filled;
} ht_entry_t;

typedef struct {
    ht_entry_t* entries;
    size_t cap;
    size_t len;
} ht_t;

int ht_alloc(ht_t* tbl, size_t cap);
void ht_free(ht_t* tbl);

ht_val_t ht_get(ht_t* tbl, ht_key_t key, bool* found);
int ht_put(ht_t* tbl, ht_key_t key, ht_val_t val);
int ht_remove(ht_t* tbl, ht_key_t key);
