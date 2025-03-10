#include "table_gen.h"
#include "sort_table.h"
#include "table_io.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void print_pair(NonceHashPair *data, size_t hash_size)
{
  printf("nonce: %d\thash: ", data->nonce);
  for (size_t j = 0; j < hash_size; j++) {
    printf("%02x", data->hash[j]);
  }
  printf("\n");
}

void print_table(TableInfo *ti, int print_items)
{
  for (int i = 0; i < print_items; i++){
    NonceHashPair *data = (NonceHashPair*)((uint8_t*)ti->data + i * (offsetof(NonceHashPair, hash) + ti->hash_size));
    print_pair(data, ti->hash_size);
  }
}

void check_sort(TableInfo *ti)
{
  int max_table_size = 1 << ti->table_size;

  for (int i = 0; i < max_table_size-1; i++){
    NonceHashPair *data = (NonceHashPair*)((uint8_t*)ti->data + i * (offsetof(NonceHashPair, hash) + ti->hash_size));
    NonceHashPair *data2 = (NonceHashPair*)((uint8_t*)ti->data + (i + 1) * (offsetof(NonceHashPair, hash) + ti->hash_size));
    for (size_t j = 0; j < ti->hash_size; j++) {
      if (data->hash[j] > data2->hash[j]) {
        printf("Data not sorted\n");
        return;
      } else if (data->hash[j] < data2->hash[j]) {
        break;
      }
    }
  }
  printf("Data sorted\n");

}

void large_num(TableInfo *ti)
{
  int max = 1 << ti->table_size;

  for (int i = 0; i < max; i++) {
    NonceHashPair *data = (NonceHashPair*)((uint8_t*)ti->data + i * (offsetof(NonceHashPair, hash) + ti->hash_size));
    if (data->nonce == max-1){
      printf("Largest pair:\n");
      print_pair(data, ti->hash_size);
      return;
    }
  }
  printf("largest not found\n");
}

void gen_range(int start, int end, int step, size_t hash_size)
{
  for (int k = start; k <= end; k += step) {
    TableInfo ti;
    gen_table(&ti, k, hash_size);
    sort(&ti);
    char filename[256];
    snprintf(filename, sizeof(filename), "pospace_k%d_h%d_sorted.bin", k, (int)hash_size);
    save_table(&ti, filename);
    free(ti.data);
  }
}

int main() {
  /*
  TableInfo ti;
  gen_table(&ti, 29, 32);

  printf("Unsorted Table:\n");
  print_table(&ti, 5);

  sort(&ti);

  printf("Sorted Table:\n");
  print_table(&ti, 5);

  large_num(&ti);
  // check_sort(&ti);
  save_table(&ti, "sorted_table");
  free(ti.data);
  */

//  gen_range(14, 29, 5, 32);

  TableInfo ti;
  load_table(&ti, "pospace_k29_h32_sorted.bin");
  print_table(&ti, 10);
  return 0;
}
