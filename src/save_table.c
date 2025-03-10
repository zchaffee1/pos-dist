#include "table_gen.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

void save_table(TableInfo *ti, const char *name)
{
  char filename[256];
  const char *directory = "tables/";

  // Check if the combined length of directory + name is too long
  if (strlen(directory) + strlen(name) >= sizeof(filename) - 5) {
    fprintf(stderr, "Error: Filename is too long.\n");
    return;
  }

  // Construct the path: directory + name
  snprintf(filename, sizeof(filename), "%s%s", directory, name);

  // Add .bin extension if no extension exists
  if (strchr(name, '.') == NULL) {
    strncat(filename, ".bin", sizeof(filename) - strlen(filename) - 1);
  }

  FILE *file = fopen(filename, "wb");
  if (!file) {
    perror("Failed to open file");
    return;
  }

  if (fwrite(&ti->table_size, sizeof(size_t), 1, file) != 1) {
    perror("Failed to write table_size");
    fclose(file);
    return;
  }

  if (fwrite(&ti->hash_size, sizeof(size_t), 1, file) != 1) {
    perror("Failed to write hash_size");
    fclose(file);
    return;
  }

  size_t num_elements = 1 << ti->table_size;
  size_t element_size = offsetof(NonceHashPair, hash) + ti->hash_size;
  size_t total_data_size = num_elements * element_size;

  if (fwrite(ti->data, total_data_size, 1, file) != 1) {
    perror("Failed to write data array");
    fclose(file);
    return;
  }

  fclose(file);
  printf("Table saved to %s successfully.\n", filename);
}

void gen_and_save_table(size_t table_size, size_t hash_size, const char *filename)
{
  TableInfo ti;
  gen_table(&ti, table_size, hash_size);
  save_table(&ti, filename);
}
