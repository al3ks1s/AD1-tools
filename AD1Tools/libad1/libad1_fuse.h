#define _FILE_OFFSET_BITS 64

#ifndef LIBAD1_FUSE_H
#define LIBAD1_FUSE_H

#define FUSE_USE_VERSION 30

#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <fuse.h>

//#include <fuse.h>

#include "libad1_definitions.h"
#include "libad1_file_reader.h"
#include "libad1_reader.h"
#include "libad1_session.h"
#include "libad1_tree.h"

typedef struct ad1_array_entry {
    char* path;
    ad1_item_header* item;
} ad1_array_entry;

void build_item_array();

void recurse_build_array(ad1_item_header* item);

ad1_item_header* path_to_item(const char*);

int ad1_getattr(const char* path, struct stat* stbuf);

int ad1_open(const char* path, struct fuse_file_info* fi);

int ad1_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi);

int ad1_readdir(const char* path, void* buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi);

void ad1_destroy(void* private_data);

int fuse_init(ad1_session* session);

#endif