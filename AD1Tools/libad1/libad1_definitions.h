#ifndef LIBAD1_DEF_H
#define LIBAD1_DEF_H

#include <stdint.h>
#include <stdio.h>

#define AD1_LOGICAL_MARGIN   512

#define AD1_FOLDER_SIGNATURE 0x05

typedef struct ad1_metadata {
    unsigned long next_metadata_addr;
    unsigned int category;
    unsigned int key;
    unsigned int data_length;

    unsigned char* data;

    struct ad1_metadata* next_metadata;

} ad1_metadata;

typedef struct ad1_item_header {
    unsigned long next_item_addr;
    unsigned long first_child_addr;

    unsigned long first_metadata_addr;

    unsigned long zlib_metadata_addr;

    unsigned long decompressed_size;
    unsigned int item_type;

    unsigned int item_name_length;

    unsigned long parent_folder;

    unsigned char* item_name;

    struct ad1_item_header* next_item;
    struct ad1_item_header* first_child;
    struct ad1_item_header* parent;

    struct ad1_metadata* first_metadata;

} ad1_item_header;

typedef struct ad1_segment_header {

    unsigned char signature[16];
    unsigned int segment_index;
    unsigned int segment_number;
    unsigned int fragments_size;
    unsigned int header_size;

} ad1_segment_header;

typedef struct ad1_logical_header {

    unsigned char signature[16];
    unsigned int image_version;

    unsigned int some_data;

    unsigned int zlib_chunk_size;

    unsigned long logical_metadata_addr;
    unsigned long first_item_addr;

    unsigned int data_source_name_length;

    unsigned char ad_signature[4];
    unsigned long data_source_name_addr;

    unsigned long attrguid_footer_addr;
    unsigned long locsguid_footer_addr;

    unsigned char* data_source_name;

    struct ad1_item_header* first_item;
    struct ad1_metadata* logical_metadata;

} ad1_logical_header;

typedef struct ad1_encrypt_header {
    unsigned char signature[8];

    unsigned int version;
    unsigned int total_size;

    short pass_count;    // -1 if unknown
    short raw_key_count; // -1 if unknown
    short cert_count;    // -1 if unknown

    unsigned short reserved; // Must be 0

    unsigned int key_algorithm;  // 1 = AES-128, 2 = AES-192, 3 = AES-256 (default)
    unsigned int hash_algorithm; // 1 = SHA-256, 2 = SHA-512

    unsigned int iteration_count; // Default: 4000
    unsigned int salt_length;
    unsigned int ekey_length; // Encrypted key
    unsigned int hmac_length;

    unsigned char* salt;
    unsigned char* encrypted_key;
    unsigned char* hmac;

} ad1_encrypt_header;

typedef struct ad1_file {

    unsigned long size;
    unsigned int segment_index;

    char* filepath;
    FILE* adfile;

} ad1_file;

typedef struct ad1_session {
    ad1_segment_header* segment_header;
    ad1_logical_header* logical_header;

    ad1_file** ad1_files;

} ad1_session;

enum category { HASH_INFO = 0x01, ITEM_TYPE = 0x02, ITEM_SIZE = 0x03, WINDOWS_FLAGS = 0x04, TIMESTAMP = 0x05 };

enum ad_hash_key { MD5_HASH = 0x5001, SHA1_HASH = 0x5002, DATA_SOURCE_NAME = 0x10002 };

enum ad_item_type_key { UNKNOWN = 0x01, CHAR_ITEM_TYPE = 0x02 };

enum ad_item_type_value {
    REFULAR_FILE = 0x31,
    PLACEHOLDER = 0x32,
    REGULAR_FOLDER = 0x33,
    FILESYSTEM_METADATA = 0x34,
    FILESLACK = 0x36,
    SYMLINK = 0x39
};

enum ad_item_size_key { FILE_SIZE = 0x03, MAX_SIZE = 0x04, UNKNOWN_1 = 0x2002, UNKNOWN_2 = 0x2003 };

enum ad_windows_flag_key {
    ENCRYPTED = 0x0D,
    COMPRESSED = 0x0E,
    UNKNOWN_FLAG_1 = 0x1E,
    HIDDEN = 0x1002,
    UNKNOWN_FLAG_2 = 0x1003,
    READ_ONLY = 0x1004,
    READY_TO_ARCHIVE = 0x1005
};

enum ad_timestamp_key { ACCESS = 0x07, MODIFIED = 0x08, CHANGE = 0x09 };

enum ad_encrypt_algo { AES128 = 1, AES192 = 2, AES256 = 3 };

enum ad_hash_algo { SHA256 = 1, SHA512 = 2 };

#endif
