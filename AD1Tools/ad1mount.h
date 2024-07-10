#define _FILE_OFFSET_BITS 64
#ifndef AD1_MOUNT_H
#define AD1_MOUNT_H

#include <stdbool.h>
#include "libad1/libad1_fuse.h"
#include "libad1/libad1_printer.h"
#include "libad1/libad1_reader.h"
#include "libad1/libad1_session.h"
#include "libad1/libad1_tree.h"

#include <argp.h>
static error_t parse_opt(int key, char* arg, struct argp_state* state);

#endif