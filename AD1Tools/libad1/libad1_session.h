#ifndef LIBAD1_SESSION_H
#define LIBAD1_SESSION_H

#include <stdio.h>
#include "libad1_definitions.h"
#include "libad1_file_reader.h"
#include "libad1_reader.h"
#include "libad1_tree.h"

typedef struct ad1_session {
    ad1_segment_header* segment_header;
    ad1_logical_header* logical_header;
    FILE* ad1_file;
} ad1_session;

ad1_session* open_ad1_session(char* filepath);

void close_ad1_session(ad1_session* session);

#endif