#include "libad1_encryption.h"
#include <stdio.h>
#include <stdlib.h>

ad1_encrypt_header*
read_encrypt_header(FILE* encrypted_file) {

    if (encrypted_file == NULL) {
        printf("File not found for encryption header");
        exit(EXIT_FAILURE);
    }

    ad1_encrypt_header* encrypt_header = NULL;
    encrypt_header = (ad1_encrypt_header*)calloc(1, sizeof(ad1_encrypt_header));

    // Read signature
    fseek(encrypted_file, 0, SEEK_SET);
    fread(&(*encrypt_header->signature), 1, 8, encrypted_file);

    if (strcmp((char*)encrypt_header->signature, "ADCRYPT")) {
        printf("The file is not of AD1 format.\n");
        free(encrypt_header);
        return 0;
    }

    encrypt_header->version = read_int_little_endian(encrypted_file, 0x08);
    encrypt_header->total_size = read_int_little_endian(encrypted_file, 0x0c); // Should be 512
    encrypt_header->pass_count = read_short_little_endian(encrypted_file, 0x10);
    encrypt_header->raw_key_count = read_short_little_endian(encrypted_file, 0x12);
    encrypt_header->cert_count = read_short_little_endian(encrypted_file, 0x14);
    encrypt_header->reserved = read_short_little_endian(encrypted_file, 0x16);
    encrypt_header->key_algorithm = read_int_little_endian(encrypted_file, 0x18);
    encrypt_header->hash_algorithm = read_int_little_endian(encrypted_file, 0x1c);
    encrypt_header->iteration_count = read_int_little_endian(encrypted_file, 0x20);
    encrypt_header->salt_length = read_int_little_endian(encrypted_file, 0x24);
    encrypt_header->ekey_length = read_int_little_endian(encrypted_file, 0x28);
    encrypt_header->hmac_length = read_int_little_endian(encrypted_file, 0x2c);

    encrypt_header->salt = (unsigned char*)calloc(encrypt_header->salt_length, sizeof(char));
    encrypt_header->encrypted_key = (unsigned char*)calloc(encrypt_header->ekey_length, sizeof(char));
    encrypt_header->hmac = (unsigned char*)calloc(encrypt_header->hmac_length, sizeof(char));

    read_string(encrypted_file, encrypt_header->salt, encrypt_header->salt_length, 0x30);
    read_string(encrypted_file, encrypt_header->encrypted_key, encrypt_header->ekey_length,
                0x30 + encrypt_header->salt_length);
    read_string(encrypted_file, encrypt_header->hmac, encrypt_header->hmac_length,
                0x30 + encrypt_header->salt_length + encrypt_header->ekey_length);

    return encrypt_header;
}

void
print_encrypt_header(ad1_encrypt_header* header) {

    printf("Signature\t%s\n", header->signature);
    printf("Version\t%d\n", header->version);
    printf("Tot-size\t%d\n", header->total_size);
    printf("Pass count\t%d\n", header->pass_count);
    printf("Key count\t%d\n", header->raw_key_count);
    printf("Cert count\t%d\n", header->cert_count);
    printf("Reserved\t%d\n", header->reserved);

    switch (header->key_algorithm) {
        case AES128: printf("Key algo : \tAES 128\n"); break;
        case AES192: printf("Key algo : \tAES 192\n"); break;
        case AES256: printf("Key algo : \tAES 256\n"); break;
    }

    switch (header->hash_algorithm) {
        case SHA256: printf("Hash algo : \tSHA 256\n"); break;
        case SHA512: printf("Hash algo : \tSHA 512\n"); break;
    }

    printf("Iter count\t%d\n", header->iteration_count);
    printf("Salt len\t%d\n", header->salt_length);
    printf("Key length\t%d\n", header->ekey_length);
    printf("HMAC len\t%d\n", header->hmac_length);
    /*
    printf("Salt : \t%s\n", hex_to_string(header->salt, header->salt_length * 2));
    printf("Key : \t%s\n", hex_to_string(header->encrypted_key, header->ekey_length * 2));
    printf("HMAC : \t%s\n", hex_to_string(header->hmac, header->hmac_length * 2));
    */
}

char*
hex_to_string(unsigned char* input, char* output, int length) {

    for (int i = 0; i < length - 1; i++) {
        sprintf((char*)(output + (2 * i)), "%02x", input[i]);
    }
    output[length] = 0;
    return output;
}