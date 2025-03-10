#include "table_gen.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Calculate the size of each NonceHashPair structure
static size_t get_pair_size(size_t hash_size) {
    return offsetof(NonceHashPair, hash) + hash_size;
}

// Comparison function for hashes
static int compare_hashes(const uint8_t *hash1, const uint8_t *hash2, size_t hash_size) {
    return memcmp(hash1, hash2, hash_size);
}

// Function to get a pointer to a NonceHashPair at a specific index
static NonceHashPair* get_pair(NonceHashPair *data, size_t index, size_t pair_size) {
    return (NonceHashPair*)((char*)data + index * pair_size);
}

// Binary insertion sort for small runs
static void insertion_sort(NonceHashPair *data, size_t start, size_t end, size_t pair_size, size_t hash_size) {
    for (size_t i = start + 1; i <= end; i++) {
        // Save the element to insert
        void *temp = malloc(pair_size);
        if (!temp) return; // Handle memory allocation failure
        memcpy(temp, get_pair(data, i, pair_size), pair_size);
        
        // Binary search to find insertion point
        int left = start;
        int right = i - 1;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (compare_hashes(((NonceHashPair*)temp)->hash, 
                               get_pair(data, mid, pair_size)->hash, 
                               hash_size) < 0) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        
        // Shift elements to make room for the insertion
        for (int j = i - 1; j >= left; j--) {
            memcpy(get_pair(data, j + 1, pair_size), 
                   get_pair(data, j, pair_size), 
                   pair_size);
        }
        
        // Insert the element
        memcpy(get_pair(data, left, pair_size), temp, pair_size);
        free(temp);
    }
}

// Find runs (sequences that are already sorted)
static size_t find_run(NonceHashPair *data, size_t start, size_t n, size_t pair_size, size_t hash_size) {
    if (start >= n - 1) return start;
    
    if (compare_hashes(get_pair(data, start, pair_size)->hash,
                       get_pair(data, start + 1, pair_size)->hash, 
                       hash_size) > 0) {
        // Descending run, reverse it
        size_t end = start + 1;
        while (end + 1 < n && 
               compare_hashes(get_pair(data, end, pair_size)->hash,
                             get_pair(data, end + 1, pair_size)->hash, 
                             hash_size) > 0) {
            end++;
        }
        
        // Reverse the descending run
        size_t left = start, right = end;
        while (left < right) {
            void *temp = malloc(pair_size);
            if (!temp) return end; // Handle memory allocation failure
            
            memcpy(temp, get_pair(data, left, pair_size), pair_size);
            memcpy(get_pair(data, left, pair_size), get_pair(data, right, pair_size), pair_size);
            memcpy(get_pair(data, right, pair_size), temp, pair_size);
            
            free(temp);
            left++;
            right--;
        }
        
        return end;
    } else {
        // Ascending run
        size_t end = start + 1;
        while (end + 1 < n && 
               compare_hashes(get_pair(data, end, pair_size)->hash,
                             get_pair(data, end + 1, pair_size)->hash, 
                             hash_size) <= 0) {
            end++;
        }
        
        return end;
    }
}

// Merge two adjacent runs
static void merge(NonceHashPair *data, size_t start, size_t mid, size_t end, 
                 size_t pair_size, size_t hash_size) {
    size_t len1 = mid - start + 1;
    size_t len2 = end - mid;
    
    // Allocate temporary storage
    void *left = malloc(len1 * pair_size);
    void *right = malloc(len2 * pair_size);
    
    if (!left || !right) {
        if (left) free(left);
        if (right) free(right);
        return; // Handle memory allocation failure
    }
    
    // Copy data to temporary arrays
    for (size_t i = 0; i < len1; i++) {
        memcpy((char*)left + i * pair_size, 
               get_pair(data, start + i, pair_size), 
               pair_size);
    }
    
    for (size_t i = 0; i < len2; i++) {
        memcpy((char*)right + i * pair_size, 
               get_pair(data, mid + 1 + i, pair_size), 
               pair_size);
    }
    
    // Merge back
    size_t i = 0, j = 0, k = start;
    
    while (i < len1 && j < len2) {
        NonceHashPair *left_pair = (NonceHashPair*)((char*)left + i * pair_size);
        NonceHashPair *right_pair = (NonceHashPair*)((char*)right + j * pair_size);
        
        if (compare_hashes(left_pair->hash, right_pair->hash, hash_size) <= 0) {
            memcpy(get_pair(data, k, pair_size), left_pair, pair_size);
            i++;
        } else {
            memcpy(get_pair(data, k, pair_size), right_pair, pair_size);
            j++;
        }
        k++;
    }
    
    // Copy remaining elements
    while (i < len1) {
        memcpy(get_pair(data, k, pair_size), 
               (char*)left + i * pair_size, 
               pair_size);
        i++;
        k++;
    }
    
    while (j < len2) {
        memcpy(get_pair(data, k, pair_size), 
               (char*)right + j * pair_size, 
               pair_size);
        j++;
        k++;
    }
    
    free(left);
    free(right);
}

// Determine min run length
static size_t get_min_run(size_t n) {
    size_t r = 0;
    while (n >= 64) {
        r |= n & 1;
        n >>= 1;
    }
    return n + r;
}

// Main Timsort function
void timsort(NonceHashPair *data, size_t n, size_t pair_size, size_t hash_size) {
    if (n <= 1) return;
    
    // Determine minimum run length
    size_t min_run = get_min_run(n);
    
    // Array to store the start indices of pending runs
    size_t *stack = (size_t*)malloc(sizeof(size_t) * (n / min_run + 1));
    size_t stack_size = 0;
    
    if (!stack) return; // Handle memory allocation failure
    
    // Process the array
    size_t curr = 0;
    while (curr < n) {
        // Find the next natural run
        size_t run_end = find_run(data, curr, n, pair_size, hash_size);
        
        // If run is shorter than min_run, extend using insertion sort
        if (run_end - curr + 1 < min_run && run_end + 1 < n) {
            size_t end = curr + min_run - 1;
            if (end >= n) end = n - 1;
            insertion_sort(data, curr, end, pair_size, hash_size);
            run_end = end;
        }
        
        // Push run onto stack
        stack[stack_size++] = curr;
        curr = run_end + 1;
        
        // Merge runs to maintain invariants
        while (stack_size >= 3) {
            size_t z = stack[stack_size - 1];
            size_t y = stack[stack_size - 2];
            size_t x = stack[stack_size - 3];
            
            // Check if X > Y + Z and Y > Z
            size_t x_len = y - x;
            size_t y_len = z - y;
            size_t z_len = curr - z;
            
            // Merge smaller runs (maintain invariant X > Y + Z and Y > Z)
            if (x_len <= y_len + z_len && y_len <= z_len) {
                // Merge Y and Z
                if (y_len < x_len) {
                    merge(data, y, z - 1, curr - 1, pair_size, hash_size);
                    stack[stack_size - 2] = stack[stack_size - 1];
                    stack_size--;
                } else {
                    // Merge X and Y
                    merge(data, x, y - 1, z - 1, pair_size, hash_size);
                    stack[stack_size - 3] = stack[stack_size - 2];
                    stack[stack_size - 2] = stack[stack_size - 1];
                    stack_size--;
                }
            } else {
                break;
            }
        }
    }
    
    // Final merges
    while (stack_size > 1) {
        size_t y = stack[stack_size - 1];
        size_t x = stack[stack_size - 2];
        stack_size -= 2;
        
        // Merge the two runs
        merge(data, x, y - 1, n - 1, pair_size, hash_size);
        
        // Push the merged run back
        if (stack_size > 0) {
            stack[stack_size++] = x;
        } else {
            stack[stack_size++] = x;
        }
    }
    
    free(stack);
}

// Main sorting function as requested
void sort(TableInfo *ti) {
    NonceHashPair *data = ti->data;
    size_t table_size = 1 << ti->table_size;
    size_t hash_size = ti->hash_size;
    size_t pair_size = get_pair_size(hash_size);
    
    timsort(data, table_size, pair_size, hash_size);
}
