#ifndef TIMSORT_H
#define TIMSORT_H

#include <stdlib.h>
#include <stdint.h>
#include "table_gen.h"

/**
 * Sort an array of NonceHashPair structures by their hash values
 *
 * This function implements a Timsort algorithm optimized for hash sorting,
 * which provides excellent performance characteristics for both random and
 * partially ordered data.
 *
 * @param ti Pointer to a TableInfo structure containing the data to sort
 */

void sort(TableInfo *ti);

#endif /* TIMSORT_H */
