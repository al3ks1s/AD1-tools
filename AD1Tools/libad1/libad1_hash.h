#ifndef AD1_HASH_H
#define AD1_HASH_H

#include <openssl/evp.h>
#include <openssl/md5.h>
#include <stdio.h>
#include "libad1_definitions.h"
#include "libad1_file_reader.h"

#define MD5_STRING_LENGTH  33

#define SHA1_DIGEST_LENGTH 20
#define SHA1_STRING_LENGTH 41

void md5_hash(unsigned char* data, unsigned long data_size, unsigned char* output_buffer);

void md5_to_string(unsigned char* md5_hash, unsigned char output_buffer[MD5_STRING_LENGTH]);

void sha1_hash(unsigned char* data, unsigned long data_size, unsigned char* output_buffer);

void sha1_to_string(unsigned char* sha1_hash, unsigned char output_buffer[SHA1_STRING_LENGTH]);

int check_md5(FILE* ad1_file, ad1_item_header* item);

int check_sha1(FILE* ad1_file, ad1_item_header* item);

int recurse_md5(FILE* ad1_file, ad1_item_header* item);

int recurse_sha1(FILE* ad1_file, ad1_item_header* item);

enum hash_check { HASH_OK, HASH_NOK, HASH_ERR };
#endif