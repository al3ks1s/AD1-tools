#include "libad1_fuse.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

ad1_session* fuse_session;
ad1_array_entry* item_array;
unsigned long item_array_index;

void
build_item_array() {

    ad1_item_header* first_item = fuse_session->logical_header->first_item;
    item_array = 0;
    item_array_index = 0;

    if (first_item == 0) {
        return;
    }

    item_array = calloc(fuse_session->item_number, sizeof(ad1_array_entry));

    recurse_build_array(first_item);
}

void
recurse_build_array(ad1_item_header* item) {
    item_array[item_array_index].path = build_item_path(item);
    item_array[item_array_index].item = item;

    item_array_index += 1;

    if (item->first_child != 0) {
        recurse_build_array(item->first_child);
    }

    if (item->next_item != 0) {
        recurse_build_array(item->next_item);
    }
}

ad1_item_header*
path_to_item(const char* path) {

    int i = 0;

    while (i < fuse_session->item_number) {
        if (strcmp(path + 1, item_array[i].path) == 0) {
            return item_array[i].item;
        }

        i++;
    }

    return 0;
}

int
ad1_getattr(const char* path, struct stat* stbuf) {

    if (strcmp(path, "/") == 0) {

        stbuf->st_uid = getuid();
        stbuf->st_gid = getgid();

        stbuf->st_mode = S_IFDIR | 0555;

        stbuf->st_size = 4096;

        stbuf->st_nlink = 1;

        return 0;
    }

    ad1_item_header* item = path_to_item(path);

    if (item == 0) {
        return -ENOENT;
    }

    stat_ad1_file(fuse_session, item, stbuf);

    return 0;
}

int
ad1_readlink(const char* path, char* buf, size_t target_size) {
    return -ENOENT;

    ad1_item_header* item = path_to_item(path);

    if (item == 0) {
        return -ENOENT;
    }

    strcpy(buf, item->item_name);

    return 0;
}

int
ad1_open(const char* path, struct fuse_file_info* fi) {

    ad1_item_header* item = path_to_item(path);

    if (item == 0) {
        return -ENOENT;
    }

    return 0;
}

int
ad1_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {

    ad1_item_header* item = path_to_item(path);

    if (item == 0) {
        return -ENOENT;
    }

    unsigned char* data = read_file_data(fuse_session, item);

    int data_size = (offset + size > item->decompressed_size) ? data_size = item->decompressed_size - offset : size;

    memcpy(buf, data + offset, data_size);

    //free(data);

    return data_size;
}

int
ad1_readdir(const char* path, void* buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) {

    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);

    ad1_item_header* current_item;

    if (strcmp(path, "/") == 0) {
        current_item = fuse_session->logical_header->first_item;
    } else {
        ad1_item_header* item = path_to_item(path);

        if (item == 0) {
            return -ENOENT;
        }

        current_item = item->first_child;
    }

    while (current_item != NULL) {

        struct stat stbuf;

        stat_ad1_file(fuse_session, current_item, &stbuf);
        filler(buffer, current_item->item_name, &stbuf, 0);
        current_item = current_item->next_item;
    }

    return 0;
}

int
fuse_init(ad1_session* ad_session) {

    fuse_session = ad_session;
    build_item_array();

    return 0;
}

void
ad1_destroy(void* private_data) {

    for (int i = 0; i < fuse_session->item_number; i++) {
        free(item_array[i].path);
    }

    free(item_array);

    close_ad1_session(fuse_session);
}

struct fuse_operations ad1_operations = {.getattr = ad1_getattr,
                                         .open = ad1_open,
                                         .read = ad1_read,
                                         .readdir = ad1_readdir,
                                         .destroy = ad1_destroy,
                                         .readlink = ad1_readlink};
