#include "libad1_tree.h"
#include <bits/types/struct_itimerspec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
build_next_metadata(ad1_session* session, ad1_metadata* parent_metadata) {

    if (parent_metadata->next_metadata_addr == 0) {
        return;
    }

    ad1_metadata* new_metadata = NULL;
    new_metadata = arbitrary_read_metadata(session, parent_metadata->next_metadata_addr);

    parent_metadata->next_metadata = new_metadata;
    build_next_metadata(session, new_metadata);
}

void
build_metadata_list(ad1_session* session, ad1_item_header* item) {

    if (item == NULL) {
        printf("No item");
        exit(EXIT_FAILURE);
    }

    if (item->first_metadata_addr != 0) {
        item->first_metadata = arbitrary_read_metadata(session, item->first_metadata_addr);
        build_next_metadata(session, item->first_metadata);
    }
}

void
build_next_items(ad1_session* session, ad1_item_header* previous_header) {

    if (previous_header->first_child_addr != 0) {
        previous_header->first_child = arbitrary_read_item(session, previous_header->first_child_addr);
        previous_header->first_child->parent = previous_header;
        build_next_items(session, previous_header->first_child);
    }

    if (previous_header->next_item_addr != 0) {
        previous_header->next_item = arbitrary_read_item(session, previous_header->next_item_addr);
        previous_header->next_item->parent = previous_header->parent;
        build_next_items(session, previous_header->next_item);
    }

    build_metadata_list(session, previous_header);
}

void
build_item_tree(ad1_session* session) {

    if (session->mode == VERBOSE) {
        printf("%s\n", "Building the tree out of the ad1 file");
    }

    if (session->logical_header->first_item_addr != 0) {
        session->logical_header->first_item = arbitrary_read_item(session, session->logical_header->first_item_addr);
        build_next_items(session, session->logical_header->first_item);
    }
}

void
free_all(ad1_segment_header* segment_header, ad1_logical_header* logical_header) {

    if (segment_header != 0) {
        free(segment_header);
        segment_header = NULL;
    }

    if (logical_header != 0) {
        free_item(logical_header->first_item);
        logical_header->first_item = 0;

        free_metadata(logical_header->logical_metadata);
        logical_header->logical_metadata = 0;

        free(logical_header->data_source_name);
        logical_header->data_source_name = 0;

        free(logical_header);
        logical_header = 0;
    }
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

    if (item->parent != NULL) {
        recurse_item_path(item->parent, path);
    }

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
