#ifndef LIBAD1_EXTRACT_H
#define LIBAD1_EXTRACT_H

#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "libad1_definitions.h"
#include "libad1_file_reader.h"
#include "libad1_hash.h"
#include "libad1_session.h"

void extract_all(ad1_session* session, const char* output_dir);

void extract_file(ad1_session* session, ad1_item_header* item, const char* output_dir);

void apply_metadata(const char* item_path, ad1_metadata* metadata);

#endif