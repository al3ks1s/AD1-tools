#ifndef AD1_INFO_H
#define AD1_INFO_H

#include <argp.h>
#include <stdbool.h>
#include "libad1/libad1_printer.h"
#include "libad1/libad1_session.h"
#include "libad1/libad1_tree.h"

void print_info(ad1_session* session);

static error_t parse_opt(int key, char* arg, struct argp_state* state);

#endif