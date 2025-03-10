CC = gcc
CFLAGS = -Wall -Wextra -O3 -Iinclude

# Common Blake3 files
BLAKE_SRC = blake/blake3.c \
            blake/blake3_dispatch.c \
            blake/blake3_portable.c \
            blake/blake3_sse2_x86-64_unix.S \
            blake/blake3_sse41_x86-64_unix.S \
            blake/blake3_avx2_x86-64_unix.S \
            blake/blake3_avx512_x86-64_unix.S

# Sources for table_gen
# TABLE_SRC = src/table_gen.c $(BLAKE_SRC)

# Sources for save_table
SAVE_SRC = src/test.c src/save_table.c src/table_gen.c src/sort_table.c $(BLAKE_SRC)

# Targets
# TABLE_TARGET = table_gen
SAVE_TARGET = test

# Phony targets
.PHONY: all clean table save

# Default target (build both)
# all: table save

# Build table_gen
# table: $(TABLE_TARGET)

# Build save_table
save: $(SAVE_TARGET)

# Rule for building table_gen
#$(TABLE_TARGET): $(TABLE_SRC)
#	$(CC) $(CFLAGS) -o $@ $^

# Rule for building save_table
$(SAVE_TARGET): $(SAVE_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up
clean:
#	rm -f $(TABLE_TARGET) $(SAVE_TARGET)
	rm -f $(SAVE_TARGET)
