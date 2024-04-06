#include "libad1_reader.h"
#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
read_ad1_string(FILE* ad1_file, char* buf, int length, unsigned long offset) {
    if (ad1_file == NULL) {
        printf("Cannot read from file");
        exit(EXIT_FAILURE);
    }

    fseek(ad1_file, offset, SEEK_SET);
    fgets(buf, length + 1, ad1_file);
}

unsigned int
read_int_little_endian(FILE* ad1_file, unsigned long offset) {

    unsigned int le_value = 0;

    if (ad1_file == NULL) {
        printf("Cannot read from file");
        exit(EXIT_FAILURE);
    }

    fseek(ad1_file, offset, SEEK_SET);
    fread(&le_value, 4, 1, ad1_file);

    return (int)le32toh(le_value);
}

unsigned int
read_long_little_endian(FILE* ad1_file, unsigned long offset) {

    unsigned long le_value = 0;

    if (ad1_file == NULL) {
        printf("Cannot read from file");
        exit(EXIT_FAILURE);
    }

    fseek(ad1_file, offset, SEEK_SET);
    fread(&le_value, 8, 1, ad1_file);

    return (long)le64toh(le_value);
}

ad1_segment_header*
read_segmented_header(FILE* ad1_file) {

    if (ad1_file == NULL) {
        printf("File not found for segment header");
        exit(EXIT_FAILURE);
    }

    ad1_segment_header* segment_header = NULL;

    if (segment_header == NULL) {
        segment_header = (ad1_segment_header*)calloc(1, sizeof(ad1_segment_header));
    }

    // Read signature
    fseek(ad1_file, 0, SEEK_SET);
    fread(&(*segment_header->signature), 1, 16, ad1_file);

    if (strcmp(segment_header->signature, "ADSEGMENTEDFILE")) {
        printf("The file is not of AD1 format.\n");
        exit(EXIT_FAILURE);
    }

    segment_header->segment_index = read_int_little_endian(ad1_file, 0x18);

    segment_header->segment_number = read_int_little_endian(ad1_file, 0x1c);

    segment_header->fragments_size = read_int_little_endian(ad1_file, 0x22);

    segment_header->header_size = read_int_little_endian(ad1_file, 0x28);

    return segment_header;
}

ad1_logical_header*
read_logical_header(FILE* ad1_file) {
    if (ad1_file == NULL) {
        printf("File not found for logical header");
        exit(EXIT_FAILURE);
    }

    ad1_logical_header* logical_header = NULL;

    logical_header = (ad1_logical_header*)calloc(1, sizeof(ad1_logical_header));

    read_ad1_string(ad1_file, logical_header->signature, 15, AD1_LOGICAL_MARGIN);

    logical_header->image_version = read_int_little_endian(ad1_file, 0x210);
    logical_header->zlib_chunk_size = read_int_little_endian(ad1_file, 0x218);
    logical_header->logical_metadata_addr = read_long_little_endian(ad1_file, 0x21c);
    logical_header->first_item_addr = read_long_little_endian(ad1_file, 0x224);
    logical_header->data_source_name_length = read_int_little_endian(ad1_file, 0x22c);

    read_ad1_string(ad1_file, logical_header->ad_signature, 3, 0x230);

    logical_header->data_source_name_addr = read_long_little_endian(ad1_file, 0x234);
    logical_header->attrguid_footer_addr = read_long_little_endian(ad1_file, 0x23c);
    logical_header->locsguid_footer_addr = read_long_little_endian(ad1_file, 0x24c);

    logical_header->data_source_name = (char*)calloc(logical_header->data_source_name_length + 1, sizeof(char));
    read_ad1_string(ad1_file, logical_header->data_source_name, logical_header->data_source_name_length, 0x25c);

    return logical_header;
}

ad1_item_header*
read_item(FILE* ad1_file, unsigned long offset) {
    if (ad1_file == NULL) {
        printf("Couldn't read file");
        exit(EXIT_FAILURE);
    }

    ad1_item_header* item_header = NULL;

    item_header = (ad1_item_header*)calloc(1, sizeof(ad1_item_header));

    item_header->next_item_addr = read_long_little_endian(ad1_file, offset);
    item_header->first_child_addr = read_long_little_endian(ad1_file, offset + 0x08);
    item_header->first_metadata_addr = read_long_little_endian(ad1_file, offset + 0x10);
    item_header->zlib_metadata_addr = read_long_little_endian(ad1_file, offset + 0x18);
    item_header->decompressed_size = read_long_little_endian(ad1_file, offset + 0x20);
    item_header->item_type = read_int_little_endian(ad1_file, offset + 0x28);
    item_header->item_name_length = read_int_little_endian(ad1_file, offset + 0x2c);

    item_header->item_name = (char*)calloc(item_header->item_name_length + 1, sizeof(char));
    read_ad1_string(ad1_file, item_header->item_name, item_header->item_name_length, offset + 0x30);

    item_header->parent_folder = read_long_little_endian(ad1_file, offset + 0x30 + item_header->item_name_length);

    return item_header;
}

ad1_metadata*
read_metadata(FILE* ad1_file, unsigned long offset) {
    if (ad1_file == NULL) {
        printf("Couldn't read the file");
        exit(EXIT_FAILURE);
    }

    ad1_metadata* metadata = NULL;
    metadata = (ad1_metadata*)calloc(1, sizeof(ad1_metadata));

    metadata->next_metadata_addr = read_long_little_endian(ad1_file, offset);
    metadata->category = read_int_little_endian(ad1_file, offset + 0x08);
    metadata->key = read_int_little_endian(ad1_file, offset + 0x0c);
    metadata->data_length = read_int_little_endian(ad1_file, offset + 0x10);

    metadata->data = (char*)calloc(metadata->data_length + 1, sizeof(char));
    read_ad1_string(ad1_file, metadata->data, metadata->data_length, offset + 0x14);

    return metadata;
}