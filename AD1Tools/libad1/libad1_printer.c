#include <stdio.h>
#include "libad1_definitions.h"
#include "libad1_printer.h"

void
print_segment_header(ad1_segment_header* segment_header) {

    printf("\n");

    printf("Signature \t: %s\n", segment_header->signature);
    printf("Segmment Index \t: %d\n", segment_header->segment_number);
    printf("Segment number \t: %d\n", segment_header->segment_number);
    printf("Frag size \t: %d\n", segment_header->fragments_size);
    printf("Header size \t: %d\n", segment_header->header_size);
}

void
print_logical_header(ad1_logical_header* logical_header) {

    printf("\n");

    printf("Signature \t\t\t: %s\n", logical_header->signature);
    printf("Version   \t\t\t: %d\n", logical_header->image_version);
    printf("Zlib chunk size \t\t: %d\n", logical_header->zlib_chunk_size);
    printf("Metadata address \t\t: %016lx\n", logical_header->logical_metadata_addr);
    printf("First data address \t\t: %016lx\n", logical_header->first_item_addr);
    printf("Data source name length  \t: %d\n", logical_header->data_source_name_length);
    printf("Signature \t\t\t: %s\n", logical_header->ad_signature);
    printf("Data source name address \t: %016lx\n", logical_header->data_source_name_addr);
    printf("ATTRGUID Footer address \t: %016lx\n", logical_header->attrguid_footer_addr);
    printf("LOCSGUID Footer address \t: %016lx\n", logical_header->locsguid_footer_addr);
    printf("Data source name \t\t: %s\n", logical_header->data_source_name);
}

void
print_item_header(ad1_item_header* item_header) {

    printf("\n");

    printf("Next item \t: %016lx\n", item_header->next_item_addr);
    printf("First child \t: %016lx\n", item_header->first_child_addr);
    printf("Metadata addr \t: %016lx\n", item_header->first_metadata_addr);
    printf("Zlib data addr \t: %016lx\n", item_header->zlib_metadata_addr);
    printf("Decomp size \t: %ld\n", item_header->decompressed_size);
    printf("Item type \t: %d\n", item_header->item_type);
    printf("Item name len \t: %d\n", item_header->item_name_length);
    printf("Item name \t: %s\n", item_header->item_name);
    printf("Parent folder \t: %016lx\n", item_header->parent_folder);

    if (item_header->next_item_addr != 0) {
        print_item_header(item_header->next_item);
    }

    if (item_header->first_child_addr != 0) {
        print_item_header(item_header->first_child);
    }
}

void
print_metadata(ad1_metadata* metadata) {

    printf("\n");

    printf("Next meta \t: %016lx\n", metadata->next_metadata_addr);
    printf("Category \t: %x\n", metadata->category);
    printf("Key      \t: %x\n", metadata->key);
    printf("Data length \t: %d\n", metadata->data_length);
    printf("Data     \t: %s\n", metadata->data);

    if (metadata->next_metadata != 0) {
        print_metadata(metadata->next_metadata);
    }
}

void
print_tree(ad1_item_header* local_root_header, int cur_depth_level) {
    int i;
    for (i = 0; i < cur_depth_level - 1; i++) {
        printf("    ");
    }

    if (i < cur_depth_level) {
        printf("└───");
    }

    printf("%s\n", local_root_header->item_name);
    if (local_root_header->first_child_addr != 0) {
        print_tree(local_root_header->first_child, cur_depth_level + 1);
    }

    if (local_root_header->next_item_addr != 0) {
        print_tree(local_root_header->next_item, cur_depth_level);
    }

    return;
}

// Pretty much useless now
void
print_paths(ad1_item_header* local_root_header) {

    if (local_root_header->first_child != NULL) {
        print_paths(local_root_header->first_child);
    }

    if (local_root_header->next_item != NULL) {
        print_paths(local_root_header->next_item);
    }
    if (local_root_header->item_type == 0) {

        if (local_root_header->parent != NULL) {
            print_parent(local_root_header->parent);
        }
        printf("%s\n", local_root_header->item_name);
    }
}

// Pretty much useless now
void
print_parent(ad1_item_header* parent) {
    if (parent->parent != NULL) {
        print_parent(parent->parent);
    }

    printf("%s/", parent->item_name);
}
