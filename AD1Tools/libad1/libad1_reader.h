#ifndef LIBAD1_READER_H
#define LIBAD1_READER_H

#include <stdio.h>
#include "libad1_definitions.h"

void arbitrary_read(ad1_session* session, unsigned char* buf, unsigned long length, unsigned long offset);

void arbitrary_read_string(ad1_session* session, unsigned char* buf, int length, unsigned long offset);

short arbitrary_read_short_little_endian(ad1_session* session, unsigned long offset);

int arbitrary_read_int_little_endian(ad1_session* session, unsigned long offset);

long arbitrary_read_long_little_endian(ad1_session* session, unsigned long offset);

ad1_item_header* arbitrary_read_item(ad1_session* session, unsigned long offset);

ad1_metadata* arbitrary_read_metadata(ad1_session* session, unsigned long offset);

// Legacy readers, useful for initial headers reading before the session is setup.

void read_string(FILE* ad1_file, unsigned char* buf, int length, unsigned long offset);

short read_short_little_endian(FILE* ad1_file, unsigned long offset);

int read_int_little_endian(FILE* ad1_file, unsigned long offset);

long read_long_little_endian(FILE* ad1_file, unsigned long offset);

ad1_segment_header* read_segmented_header(FILE* ad1_file);

ad1_logical_header* read_logical_header(FILE* ad1_file);

ad1_encrypt_header* read_encrypt_header(FILE* ad1_file);

ad1_item_header* read_item(FILE* ad1_file, unsigned long offset);

ad1_metadata* read_metadata(FILE* ad1_file, unsigned long offset);

#endif