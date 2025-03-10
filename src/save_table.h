#ifndef SAVE_TABLE_H
#define SAVE_TABLE_H

#include "table_gen.h"

void save_table(TableInfo *ti, const char *name);

void gen_and_save_table(size_t table_size, size_t hash_size, const char *filename);

#endif
