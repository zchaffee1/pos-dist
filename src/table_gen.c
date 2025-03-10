#include "blake3.h"
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>

typedef struct {
    int nonce;
    uint8_t hash[];
} NonceHashPair;

typedef struct {
    size_t table_size;
    size_t hash_size;
    NonceHashPair *data;
} TableInfo;

void gen_hash(TableInfo *ti, NonceHashPair *data) {
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, &data->nonce, sizeof(data->nonce));
    blake3_hasher_finalize(&hasher, data->hash, ti->hash_size);
}

void gen_data(TableInfo *ti) {
    int max_size = 1 << ti->table_size;

    for (int nonce = 0; nonce < max_size; nonce++) {
        NonceHashPair *data = (NonceHashPair*)((uint8_t*)ti->data + nonce * (offsetof(NonceHashPair, hash) + ti->hash_size));
        data->nonce = nonce;
        gen_hash(ti, data);
    }
}

void gen_info(TableInfo *ti, size_t table_size, size_t hash_size)
{
  ti->table_size = table_size;
  ti->hash_size = hash_size;
  size_t num_elements = 1 << ti->table_size;
  ti->data = malloc(num_elements * (offsetof(NonceHashPair, hash) + ti->hash_size));
}

void gen_table(TableInfo *ti, size_t table_size, size_t hash_size)
{
  gen_info(ti, table_size, hash_size);
  gen_data(ti);
}

/*
int main() {
  TableInfo table;
  TableInfo *ti = &table;
  gen_table(ti, 3, 32);
  //
    // Print all data in hash
    int max_table_size = 1 << ti->table_size;
    for (int i = 0; i < max_table_size; i++) {
        NonceHashPair *data = (NonceHashPair*)((uint8_t*)ti->data + i * (offsetof(NonceHashPair, hash) + ti->hash_size));
        printf("nonce: %d\thash: ", data->nonce);
        for (size_t j = 0; j < ti->hash_size; j++) {
            printf("%02x", data->hash[j]);
        }
        printf("\n");
    }

    free(ti->data);
    return 0;
}
*/
