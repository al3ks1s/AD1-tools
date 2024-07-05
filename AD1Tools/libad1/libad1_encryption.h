#ifndef LIBAD1_ENCRYPT_H
#define LIBAD1_ENCRYPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libad1_definitions.h"
#include "libad1_reader.h"

ad1_encrypt_header* read_encrypt_header(FILE* encrypted_file);

void print_encrypt_header(ad1_encrypt_header* header);

char* hex_to_string(unsigned char* input, char* output, int length);

#endif