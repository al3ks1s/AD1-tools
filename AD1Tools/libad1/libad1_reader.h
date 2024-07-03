#ifndef LIBAD1_READER_H
#define LIBAD1_READER_H

#include <stdio.h>
#include "libad1_definitions.h"

void read_string(FILE* ad1_file, char* buf, int length, unsigned long offset);

unsigned int read_short_little_endian(FILE* ad1_file, unsigned long offset);

unsigned int read_int_little_endian(FILE* ad1_file, unsigned long offset);

unsigned int read_long_little_endian(FILE* ad1_file, unsigned long offset);

ad1_segment_header* read_segmented_header(FILE* ad1_file);

ad1_logical_header* read_logical_header(FILE* ad1_file);

ad1_encrypt_header* read_encrypt_header(FILE* ad1_file);

ad1_item_header* read_item(FILE* ad1_file, unsigned long offset);

ad1_metadata* read_metadata(FILE* ad1_file, unsigned long offset);

#endif