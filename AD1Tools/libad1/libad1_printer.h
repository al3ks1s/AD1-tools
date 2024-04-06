#ifndef AD1_PRINTER_H
#define AD1_PRINTER_H

#include <stdio.h>
#include <stdlib.h>
#include "libad1_definitions.h"
#include "libad1_file_reader.h"

void print_segment_header(ad1_segment_header* segment_header);

void print_logical_header(ad1_logical_header* logical_header);

void print_item_header(ad1_item_header* item_header);

void print_metadata(ad1_metadata* metadata);

void print_tree(ad1_item_header* local_root_header, int depth_level);

void print_paths(ad1_item_header* local_root_header);

void print_parent(ad1_item_header* parent);

#endif