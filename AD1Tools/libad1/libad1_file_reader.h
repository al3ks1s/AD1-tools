#ifndef AD1_FILE_READER_H
#define AD1_FILE_READER_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "libad1_definitions.h"
#include "libad1_reader.h"

unsigned char* read_file_data(FILE* ad1_file, ad1_item_header* ad1_item);

long read_zlib_chunk(FILE* ad1_file, unsigned char* output_data_ptr, unsigned long offset, unsigned int zlib_chunk_size,
                     unsigned long decompressed_size);

long zlib_inflate(unsigned char* compressed_data, unsigned int data_size, unsigned char* output,
                  unsigned long decompressed_size);

bool check_md5(ad1_item_header* item, const char* md5_hash);

bool check_sha1(ad1_item_header* item, const char* sha1_hash);

#endif