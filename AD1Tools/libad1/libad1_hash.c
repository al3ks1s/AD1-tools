#include "libad1_hash.h"
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <stdio.h>
#include <string.h>
#include "libad1_tree.h"

void
md5_hash(unsigned char* data, unsigned long data_size, unsigned char* output_buffer) {

    EVP_MD_CTX* ctx;

    ctx = EVP_MD_CTX_new();
    EVP_DigestInit(ctx, EVP_md5());

    EVP_DigestUpdate(ctx, data, data_size);

    EVP_DigestFinal(ctx, output_buffer, &data_size);

    EVP_MD_CTX_free(ctx);
}

void
md5_to_string(unsigned char* md5_hash, unsigned char output_buffer[MD5_STRING_LENGTH]) {
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf((char*)(output_buffer + (2 * i)), "%02x", md5_hash[i]);
    }
    output_buffer[MD5_STRING_LENGTH] = 0;
}

void
sha1_hash(unsigned char* data, unsigned long data_size, unsigned char* output_buffer) {

    EVP_MD_CTX* ctx;

    ctx = EVP_MD_CTX_new();

    EVP_DigestInit(ctx, EVP_sha1());

    EVP_DigestUpdate(ctx, data, data_size);

    EVP_DigestFinal(ctx, output_buffer, &data_size);

    EVP_MD_CTX_free(ctx);
}

void
sha1_to_string(unsigned char* sha1_hash, unsigned char output_buffer[SHA1_STRING_LENGTH]) {
    for (int i = 0; i < SHA1_DIGEST_LENGTH; i++) {
        sprintf((char*)(output_buffer + (2 * i)), "%02x", sha1_hash[i]);
    }
    output_buffer[SHA1_STRING_LENGTH] = 0;
}

int
check_md5(FILE* ad1_file, ad1_item_header* item) {

    ad1_metadata* metadata;

    unsigned char *item_metadata_hash, calculated_hash[MD5_DIGEST_LENGTH], calculated_hash_string[MD5_STRING_LENGTH];
    unsigned char* file_data;

    metadata = item->first_metadata;

    while (metadata->category != HASH_INFO || metadata->key != MD5_HASH) {
        metadata = metadata->next_metadata;
    }

    if (metadata->next_metadata == NULL) {
        return HASH_ERR;
    }

    item_metadata_hash = metadata->data;

    file_data = read_file_data(ad1_file, item);

    md5_hash(file_data, item->decompressed_size, calculated_hash);

    md5_to_string(calculated_hash, calculated_hash_string);

    if (strncmp(item_metadata_hash, calculated_hash_string, MD5_STRING_LENGTH - 1) == 0) {
        return HASH_OK;
    } else {
        return HASH_NOK;
    }
}

int
check_sha1(FILE* ad1_file, ad1_item_header* item) {
    ad1_metadata* metadata;

    unsigned char *item_metadata_hash, calculated_hash[SHA1_DIGEST_LENGTH], calculated_hash_string[SHA1_STRING_LENGTH];
    unsigned char* file_data;

    metadata = item->first_metadata;

    while ((metadata->category != HASH_INFO || metadata->key != SHA1_HASH)) {
        metadata = metadata->next_metadata;
    }

    if (metadata->next_metadata == NULL) {
        return HASH_ERR;
    }

    item_metadata_hash = metadata->data;

    file_data = read_file_data(ad1_file, item);

    sha1_hash(file_data, item->decompressed_size, calculated_hash);

    sha1_to_string(calculated_hash, calculated_hash_string);

    if (strncmp(item_metadata_hash, calculated_hash_string, SHA1_STRING_LENGTH - 1) == 0) {
        return HASH_OK;
    } else {
        return HASH_NOK;
    }
}

int
recurse_md5(FILE* ad1_file, ad1_item_header* item) {

    int ret;

    if (item->item_type == 0) {
        ret = check_md5(ad1_file, item);

        switch (ret) {
            case HASH_OK: printf("File : %s OK\n", build_item_path(item)); break;
            case HASH_ERR: printf("File : %s ERROR\n", build_item_path(item)); return ret;
            case HASH_NOK: printf("File : %s Hash differs\n", build_item_path(item)); return ret;
        }
    }

    if (item->first_child != 0) {
        recurse_md5(ad1_file, item->first_child);
    }

    if (item->next_item != 0) {
        recurse_md5(ad1_file, item->next_item);
    }

    return ret;
}

int
recurse_sha1(FILE* ad1_file, ad1_item_header* item) {
    int ret;

    if (item->item_type == 0) {
        ret = check_sha1(ad1_file, item);

        switch (ret) {
            case HASH_OK: printf("%s OK\n", build_item_path(item)); break;
            case HASH_ERR: printf("%s ERROR\n", build_item_path(item)); return ret;
            case HASH_NOK: printf("%s Hash differs\n", build_item_path(item)); return ret;
        }
    }

    if (item->first_child != 0) {
        recurse_sha1(ad1_file, item->first_child);
    }

    if (item->next_item != 0) {
        recurse_sha1(ad1_file, item->next_item);
    }

    return ret;
}