CC = gcc
CFLAGS = -Wall -Wextra -O3 -Iinclude
SRC = src/table_gen.c \
      blake/blake3.c \
      blake/blake3_dispatch.c \
      blake/blake3_portable.c \
      blake/blake3_sse2_x86-64_unix.S \
      blake/blake3_sse41_x86-64_unix.S \
      blake/blake3_avx2_x86-64_unix.S \
      blake/blake3_avx512_x86-64_unix.S
TARGET = table_gen

# Phony targets
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
