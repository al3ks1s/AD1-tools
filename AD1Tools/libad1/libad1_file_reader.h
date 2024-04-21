#ifndef LIBAD1_FILE_READER_H
#define LIBAD1_FILE_READER_H

#include <assert.h>
#include <stdio.h>
#include "libad1_definitions.h"
#include "libad1_reader.h"

unsigned char* read_file_data(FILE* ad1_file, ad1_item_header* ad1_item);

long read_zlib_chunk(FILE* ad1_file, unsigned char* output_data_ptr, unsigned long offset, unsigned int zlib_chunk_size,
                     unsigned long decompressed_size);

long zlib_inflate(unsigned char* compressed_data, unsigned int data_size, unsigned char* output,
                  unsigned long decompressed_size);

#endif