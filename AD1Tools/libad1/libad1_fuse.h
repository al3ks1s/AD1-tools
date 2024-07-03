#ifndef LIBAD1_FUSE_H
#define LIBAD1_FUSE_H

#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

//#include <fuse.h>

#include "libad1_definitions.h"
#include "libad1_file_reader.h"
#include "libad1_reader.h"
#include "libad1_tree.h"

#define BLOCK_SIZE 1024

typedef struct ad1_file {

} ad1_file;

//struct fuse_operations ad1_oper = {.getattr = bb_getattr, .readlink = bb_readlink, .open = bb_open, .read = bb_read};

#endif