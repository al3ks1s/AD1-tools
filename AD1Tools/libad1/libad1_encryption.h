#ifndef LIBAD1_ENCRYPT_H
#define LIBAD1_ENCRYPT_H

struct ad1_encrypted_header {
    char signature[8];

    unsigned int version;
    unsigned int total_size;

    short pass_count;
    short raw_key_count;
    short cert_count;

    unsigned short reserved;

    unsigned int key_algorithm;
    unsigned int hash_algorithm;

    unsigned int iterations;

    unsigned int salt_length;
    unsigned int key_length;
    unsigned int hmac_length;

    unsigned char* salt;
    unsigned char* key;
    unsigned char* hmac;
};

enum encryption_algo { AES128, AES192, AES256 };

enum encryption_hash { SHA256, SHA512 };

#endif