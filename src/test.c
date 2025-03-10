#include "table_gen.h"
#include "sort_table.h"
#include "save_table.h"
#include <stdio.h>

void print_table(TableInfo *ti, int print_items) 
{
  for (int i = 0; i < print_items; i++){
    NonceHashPair *data = (NonceHashPair*)((uint8_t*)ti->data + i * (offsetof(NonceHashPair, hash) + ti->hash_size));
    printf("nonce: %d\thash: ", data->nonce);
    for (size_t j = 0; j < ti->hash_size; j++) {
      printf("%02x", data->hash[j]);
    }
    printf("\n");
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

int main() {
  TableInfo ti;
  gen_table(&ti, 5, 32);


  printf("Unsorted Table:\n");
  print_table(&ti, 5);

  sort(&ti);
  printf("Sorted Table:\n");
  print_table(&ti, 5);


  check_sort(&ti);

  save_table(&ti, "sorted_table");
}
