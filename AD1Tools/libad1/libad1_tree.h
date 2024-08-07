#ifndef LIBAD1_TREE_H
#define LIBAD1_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include "libad1_definitions.h"
#include "libad1_hash.h"
#include "libad1_reader.h"

void build_next_metadata(ad1_session* session, ad1_metadata* parent_metadata);

void build_metadata_list(ad1_session* session, ad1_item_header* item);

void build_next_item(ad1_session* session, ad1_item_header* previous_header);

ad1_item_header* build_next_child(ad1_session* session, ad1_item_header* parent_header);

void build_item_tree(ad1_session* session);

void free_all(ad1_segment_header* segment_header, ad1_logical_header* logical_header);

void free_item(ad1_item_header* item);

void free_metadata(ad1_metadata* ad1_metadata);

int get_path_length(ad1_item_header* item);

char* build_item_path(ad1_item_header* item);

void recurse_item_path(ad1_item_header* item, char* buffer);

#endif