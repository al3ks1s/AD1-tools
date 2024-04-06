#include "libad1_tree.h"
#include <bits/types/struct_itimerspec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
build_next_metadata(FILE* ad1_file, ad1_metadata* parent_metadata) {

    if (ad1_file == NULL) {
        printf("Couldn't read file");
        exit(EXIT_FAILURE);
    }

    if (parent_metadata->next_metadata_addr == 0) {
        return;
    }

    ad1_metadata* new_metadata = NULL;
    new_metadata = read_metadata(ad1_file, parent_metadata->next_metadata_addr + AD1_LOGICAL_MARGIN);

    parent_metadata->next_metadata = new_metadata;
    build_next_metadata(ad1_file, new_metadata);
}

void
build_metadata_list(FILE* ad1_file, ad1_item_header* item) {

    if (ad1_file == NULL) {
        printf("Couldn't read file");
        exit(EXIT_FAILURE);
    }

    if (item == NULL) {
        printf("No item");
        exit(EXIT_FAILURE);
    }

    if (item->first_metadata_addr != 0) {
        item->first_metadata = read_metadata(ad1_file, item->first_metadata_addr + AD1_LOGICAL_MARGIN);
        build_next_metadata(ad1_file, item->first_metadata);
    }
}

void
build_next_items(FILE* ad1_file, ad1_item_header* previous_header) {
    if (ad1_file == NULL) {
        printf("Couldn't read file");
        exit(EXIT_FAILURE);
    }

    if (previous_header->first_child_addr != 0) {
        previous_header->first_child = read_item(ad1_file, previous_header->first_child_addr + AD1_LOGICAL_MARGIN);
        previous_header->first_child->parent = previous_header;
        build_next_items(ad1_file, previous_header->first_child);
    }

    if (previous_header->next_item_addr != 0) {
        previous_header->next_item = read_item(ad1_file, previous_header->next_item_addr + AD1_LOGICAL_MARGIN);
        previous_header->next_item->parent = previous_header->parent;
        build_next_items(ad1_file, previous_header->next_item);
    }

    build_metadata_list(ad1_file, previous_header);
}

void
build_item_tree(FILE* ad1_file, ad1_logical_header* logical_header) {
    if (ad1_file == NULL) {
        printf("Couldn't read file");
        exit(EXIT_FAILURE);
    }

    if (logical_header->first_item_addr != 0) {
        logical_header->first_item = read_item(ad1_file, logical_header->first_item_addr + AD1_LOGICAL_MARGIN);
        build_next_items(ad1_file, logical_header->first_item);
    }
}

void
free_all(ad1_segment_header* segment_header, ad1_logical_header* logical_header) {

    free(segment_header);
    segment_header = NULL;

    free_item(logical_header->first_item);
    logical_header->first_item = 0;

    free_metadata(logical_header->logical_metadata);
    logical_header->logical_metadata = 0;

    free(logical_header->data_source_name);
    logical_header->data_source_name = 0;

    free(logical_header);
    logical_header = 0;
}

void
free_item(ad1_item_header* item) {

    if (item == NULL) {
        return;
    }

    if (item->first_metadata != NULL) {
        free_metadata(item->first_metadata);
        item->first_metadata = 0;
    }

    if (item->next_item != NULL) {
        free_item(item->next_item);
        item->next_item = 0;
    }

    if (item->first_child != NULL) {
        free_item(item->first_child);
        item->first_child = 0;
    }

    free(item->item_name);
    item->item_name = 0;

    free(item);
    item = 0;
}

void
free_metadata(ad1_metadata* ad1_metadata) {

    if (ad1_metadata == NULL) {
        return;
    }

    if (ad1_metadata->next_metadata != NULL) {
        free_metadata(ad1_metadata->next_metadata);
        ad1_metadata->next_metadata = 0;
    }

    free(ad1_metadata->data);
    ad1_metadata->data = 0;

    free(ad1_metadata);
    ad1_metadata = 0;
}

int
get_path_length(ad1_item_header* item) {
    int path_length;

    path_length = item->item_name_length;

    if (item->parent != NULL) {
        path_length += get_path_length(item->parent) + 1;
    }

    return path_length;
}

char*
build_item_path(ad1_item_header* item) {
    int path_length;
    char* path;

    path_length = get_path_length(item);

    path = (char*)calloc(path_length + 1, sizeof(char));

    recurse_item_path(item->parent, path);

    memcpy(path + path_length - item->item_name_length, item->item_name, item->item_name_length);

    return path;
}

void
recurse_item_path(ad1_item_header* item, char* buffer) {

    if (item->parent != NULL) {
        recurse_item_path(item->parent, buffer);
    }
    int path_length;
    path_length = get_path_length(item);

    memcpy(buffer + path_length - item->item_name_length, item->item_name, item->item_name_length);

    buffer[path_length] = 47;
}