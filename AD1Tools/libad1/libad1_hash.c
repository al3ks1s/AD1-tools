#include "libad1_hash.h"
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libad1_tree.h"

void
md5_hash(unsigned char* data, unsigned long data_size, unsigned char* output_buffer) {

    EVP_MD_CTX* ctx;

    ctx = EVP_MD_CTX_new();
    EVP_DigestInit(ctx, EVP_md5());

    EVP_DigestUpdate(ctx, data, data_size);

    EVP_DigestFinal(ctx, output_buffer, (unsigned int*)&data_size);

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

    EVP_DigestFinal(ctx, output_buffer, (unsigned int*)&data_size);

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
check_md5(ad1_session* session, ad1_item_header* item) {

    ad1_metadata* metadata;

    unsigned char *item_metadata_hash, calculated_hash[MD5_DIGEST_LENGTH], calculated_hash_string[MD5_STRING_LENGTH];
    unsigned char* file_data;

    metadata = item->first_metadata;

    while (metadata->next_metadata != 0 && (metadata->category != HASH_INFO || metadata->key != MD5_HASH)) {
        metadata = metadata->next_metadata;
    }

    if (metadata == NULL) {
        return HASH_ERR;
    }

    item_metadata_hash = metadata->data;

    file_data = read_file_data(session, item);

    md5_hash(file_data, item->decompressed_size, calculated_hash);

    md5_to_string(calculated_hash, calculated_hash_string);

    if (strncmp((char*)item_metadata_hash, (char*)calculated_hash_string, MD5_STRING_LENGTH - 1) == 0) {
        return HASH_OK;
    } else {
        return HASH_NOK;
    }
}

int
check_sha1(ad1_session* session, ad1_item_header* item) {
    ad1_metadata* metadata;

    unsigned char *item_metadata_hash, calculated_hash[SHA1_DIGEST_LENGTH], calculated_hash_string[SHA1_STRING_LENGTH];
    unsigned char* file_data;

    metadata = item->first_metadata;

    while (metadata->next_metadata != 0 && (metadata->category != HASH_INFO || metadata->key != SHA1_HASH)) {
        metadata = metadata->next_metadata;
    }

    if (metadata == NULL) {
        return HASH_ERR;
    }

    item_metadata_hash = metadata->data;

    file_data = read_file_data(session, item);

    sha1_hash(file_data, item->decompressed_size, calculated_hash);

    sha1_to_string(calculated_hash, calculated_hash_string);

    if (strncmp((char*)item_metadata_hash, (char*)calculated_hash_string, SHA1_STRING_LENGTH - 1) == 0) {
        return HASH_OK;
    } else {
        return HASH_NOK;
    }
}

int
recurse_md5(ad1_session* session, ad1_item_header* item) {

    int ret;

    if (item->item_type != AD1_FOLDER_SIGNATURE) {
        ret = check_md5(session, item);

        char* item_path = build_item_path(item);

        switch (ret) {
            case HASH_OK: printf("MD5 OK for file : %s\n", item_path); break;
            case HASH_ERR: printf("MD5 ERROR for file : %s\n", item_path); break;
            case HASH_NOK: printf("MD5 NOK for file : %s\n", item_path); break;
        }

        free(item_path);
    }

    if (item->first_child != 0) {
        recurse_md5(session, item->first_child);
    }

    if (item->next_item != 0) {
        recurse_md5(session, item->next_item);
    }

    return ret;
}

int
recurse_sha1(ad1_session* session, ad1_item_header* item) {
    int ret;

    if (item->item_type != AD1_FOLDER_SIGNATURE) {
        ret = check_sha1(session, item);

        char* item_path = build_item_path(item);

        switch (ret) {
            case HASH_OK: printf("SHA1 OK for file : %s\n", item_path); break;
            case HASH_ERR: printf("SHA1 ERROR for file : %s\n", item_path); break;
            case HASH_NOK: printf("SHA1 NOK for file : %s\n", item_path); break;
        }

        free(item_path);
    }

    if (item->first_child != 0) {
        recurse_sha1(session, item->first_child);
    }

    if (item->next_item != 0) {
        recurse_sha1(session, item->next_item);
    }

    return ret;
}

void
check_file_md5(ad1_session* session) {}

void
check_file_sha1(ad1_session* session) {}