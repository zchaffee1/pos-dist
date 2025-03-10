// REMOVE THE ../include
#include "../include/blake3.h"
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

void gen_table(TableInfo *ti) {
    int max_size = 1 << ti->table_size;

    for (int nonce = 0; nonce < max_size; nonce++) {
        NonceHashPair *data = (NonceHashPair*)((uint8_t*)ti->data + nonce * (offsetof(NonceHashPair, hash) + ti->hash_size));
        data->nonce = nonce;
        gen_hash(ti, data);
    }
}

int main() {
    TableInfo ti;
    ti.table_size = 3;
    ti.hash_size = 32;

    // Allocate memory for the table
    size_t num_elements = 1 << ti.table_size;
    ti.data = malloc(num_elements * (offsetof(NonceHashPair, hash) + ti.hash_size));

    if (ti.data == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    gen_table(&ti);

    // Print all data in hash
    int max_table_size = 1 << ti.table_size;
    for (int i = 0; i < max_table_size; i++) {
        NonceHashPair *data = (NonceHashPair*)((uint8_t*)ti.data + i * (offsetof(NonceHashPair, hash) + ti.hash_size));
        printf("nonce: %d\thash: ", data->nonce);
        for (size_t j = 0; j < ti.hash_size; j++) {
            printf("%02x", data->hash[j]);
        }
        printf("\n");
    }

    free(ti.data);
    return 0;
}
