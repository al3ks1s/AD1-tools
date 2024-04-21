#define _XOPEN_SOURCE 700
#include <time.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <utime.h>
#include "libad1_extract.h"

void
extract_all(ad1_session* session, const char* output_dir) {

    if (mkdir(output_dir, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) == -1) {
        switch (errno) {
            case EACCES: printf("You don't have access to %s.", output_dir); return;
            case ELOOP: printf("Symlink loop on %s.\n", output_dir); return;
            case EMLINK: printf("Link count exceeds max link count.\n"); return;
            case EEXIST: break;
            case ENAMETOOLONG:
                printf("Path length exceeds %d or a compondent length exceeds %d\n.", PATH_MAX, NAME_MAX);
                return;
            case ENOENT: printf("Parent path does not exists.\n"); return;
            case ENOTDIR: printf("Parent path not a directory.\n"); return;
            case EROFS: printf("Filesystem is read-only.\n"); return;
            case ENOSPC: printf("Not enough space on the filesystem to create directory.\n"); return;
        }
    }

    extract_file(session->ad1_file, session->logical_header->first_item, output_dir);
}

void
extract_file(FILE* ad1_file, ad1_item_header* item, const char* output_dir) {

    // Build entire item path with the output folder prefix
    int output_dir_length;
    char *local_item_path, *complete_path;

    local_item_path = build_item_path(item);

    output_dir_length = strlen(output_dir);

    complete_path = (char*)calloc(output_dir_length + get_path_length(item) + 2, sizeof(char));

    memcpy(complete_path, output_dir, output_dir_length);
    complete_path[output_dir_length] = 47;
    memcpy(complete_path + output_dir_length + 1, local_item_path, get_path_length(item));

    /*
    fprintf(stdout, "\rExtracting : %s", complete_path);
    fflush(stdout);
    //*/

    // There's probably a lot to improve on that one, like the program not extracting files next to a folder it doesn't manage to create, not gonna do that today tho
    if (item->item_type == AD1_FOLDER_SIGNATURE) {
        if (mkdir(complete_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) == -1) {

            free(local_item_path);
            local_item_path = NULL;

            free(complete_path);
            complete_path = NULL;

            switch (errno) {
                case EACCES: printf("You don't have access to %s.", complete_path); return;
                case ELOOP: printf("Symlink loop on %s.\n", complete_path); return;
                case EMLINK: printf("Link count exceeds max link count for path %s.\n", complete_path); return;
                case EEXIST: break;
                case ENAMETOOLONG:
                    printf("Path length exceeds %d or a compondent length exceeds %d\n.", PATH_MAX, NAME_MAX);
                    return;
                case ENOENT: printf("Parent path for %s does not exists.\n", item->item_name); return;
                case ENOTDIR: printf("Parent path for %s not a directory.\n", item->item_name); return;
                case EROFS: printf("Filesystem is read-only.\n"); return;
                case ENOSPC: printf("Not enough space on the filesystem to create directory.\n"); return;
            }
        }
    } else if (item->item_type == 0) {
        unsigned char* file_data;

        FILE* output_file;

        output_file = fopen(complete_path, "wb");
        if (output_file != NULL) {
            file_data = read_file_data(ad1_file, item);

            fwrite(file_data, sizeof(char), item->decompressed_size, output_file);

            fclose(output_file);
        } else {
            printf("Couldn't create file : %s\n", complete_path);
        }

        free(file_data);
    }

    apply_metadata(complete_path, item->first_metadata);

    free(local_item_path);
    local_item_path = NULL;

    free(complete_path);
    complete_path = NULL;

    if (item->first_child != NULL) {
        extract_file(ad1_file, item->first_child, output_dir);
    }

    if (item->next_item != NULL) {
        extract_file(ad1_file, item->next_item, output_dir);
    }
}

void
apply_metadata(const char* item_path, ad1_metadata* metadata) {

    struct timeval times[2] = {0};

    while (metadata != NULL) {

        switch (metadata->category) {
            case HASH_INFO: break;
            case ITEM_TYPE: break;
            case ITEM_SIZE: break;
            case WINDOWS_FLAGS: break;
            case TIMESTAMP:
                switch (metadata->key) {
                    case ACCESS: parse_timestamp(times, metadata->data); break;
                    case CHANGE: break;
                    case MODIFIED: parse_timestamp(times + 1, metadata->data); break;
                };
        }

        metadata = metadata->next_metadata;
    };

    utimes(item_path, times);
}

void
parse_timestamp(struct timeval* time, const char* time_s) {

    struct tm time_tm;

    strptime(time_s, "%Y%m%dT%H%M%S", &time_tm);

    time->tv_sec = mktime(&time_tm);

    return;
}