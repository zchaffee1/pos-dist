#ifndef TABLE_GEN_H
#define TABLE_GEN_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int nonce;
    uint8_t hash[];
} NonceHashPair;

typedef struct {
    size_t table_size;
    size_t hash_size;
    NonceHashPair *data;
} TableInfo;

void gen_table(TableInfo *ti, size_t table_size, size_t hash_size);

#endif 
