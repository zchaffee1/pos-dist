#include "table_gen.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

char *correct_filename(const char *name) {
    const char *directory = "tables/";
    const char *extension = ".bin";
    char filename[256];

    // Check if the combined length of directory, name, and extension exceeds the buffer size
    if (strlen(directory) + strlen(name) + strlen(extension) >= sizeof(filename)) {
        fprintf(stderr, "Error: Filename is too long.\n");
        abort();
    }

    // Construct the base filename: directory + name
    snprintf(filename, sizeof(filename), "%s%s", directory, name);

    // Add .bin extension if no extension exists
    if (strchr(name, '.') == NULL) {
        strncat(filename, extension, sizeof(filename) - strlen(filename) - 1);
    }

    // Return a dynamically allocated copy of the filename
    return strdup(filename);
}

void save_table(TableInfo *ti, const char *name)
{
  char *filename = correct_filename(name);
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

void load_table(TableInfo *ti, const char *name)
{
    char *filename = correct_filename(name);
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    // Read table_size and hash_size from the file
    if (fread(&ti->table_size, sizeof(size_t), 1, file) != 1) {
        perror("Failed to read table_size");
        fclose(file);
        return;
    }

    if (fread(&ti->hash_size, sizeof(size_t), 1, file) != 1) {
        perror("Failed to read hash_size");
        fclose(file);
        return;
    }

    // Calculate the size of the data array
    size_t num_elements = 1 << ti->table_size; // 2^table_size
    size_t element_size = offsetof(NonceHashPair, hash) + ti->hash_size;
    size_t total_data_size = num_elements * element_size;

    // Allocate memory for the data array
    ti->data = malloc(total_data_size);
    if (!ti->data) {
        perror("Failed to allocate memory for data array");
        fclose(file);
        return;
    }

    // Read the data array from the file
    if (fread(ti->data, total_data_size, 1, file) != 1) {
        perror("Failed to read data array");
        fclose(file);
        free(ti->data);
        ti->data = NULL; // Ensure the pointer is set to NULL after freeing
        return;
    }

    fclose(file);
    printf("Table loaded from %s successfully.\n", filename);
    free(filename); // Free the filename allocated by correct_filename
}

void gen_and_save_table(size_t table_size, size_t hash_size, const char *filename)
{
  TableInfo ti;
  gen_table(&ti, table_size, hash_size);
  save_table(&ti, filename);
}

