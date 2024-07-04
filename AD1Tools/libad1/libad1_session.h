#ifndef LIBAD1_SESSION_H
#define LIBAD1_SESSION_H

#include <stdio.h>
#include "libad1_definitions.h"
#include "libad1_file_reader.h"
#include "libad1_reader.h"
#include "libad1_tree.h"

ad1_session* open_ad1_session(char* filepath);

void close_ad1_session(ad1_session* session);

#endif