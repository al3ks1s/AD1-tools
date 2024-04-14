#include "libad1_extract.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void
extract_all(ad1_session* session, const char* output_dir) {

    extract_file(session->ad1_file, session->logical_header->first_item, output_dir);
}

void
extract_file(FILE* ad1_file, ad1_item_header* item, const char* root_folder) {

    int root_folder_length;
    char *local_item_path, *complete_path;

    local_item_path = build_item_path(item);

    root_folder_length = strlen(root_folder);

    complete_path = (char*)calloc(root_folder_length + get_path_length(item) + 2, sizeof(char));

    memcpy(complete_path, root_folder, root_folder_length);
    complete_path[root_folder_length] = 47;
    memcpy(complete_path + root_folder_length + 1, local_item_path, get_path_length(item));

    /*
    fprintf(stdout, "\rExtracting : %s", complete_path);
    fflush(stdout);
    //*/

    if (item->item_type == AD1_FOLDER_SIGNATURE) {
        if (mkdir(complete_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) == -1) {}
    } else if (item->item_type == 0) {
        unsigned char* file_data;

        FILE* output_file;

        output_file = fopen(complete_path, "wb");
        if (output_file == NULL) {
            printf("Couldn't create and open file : %s\n", complete_path);
            return;
        }

        file_data = read_file_data(ad1_file, item);

        fwrite(file_data, sizeof(char), item->decompressed_size, output_file);

        fclose(output_file);
        free(file_data);
    }

    free(local_item_path);
    local_item_path = 0;

    free(complete_path);
    complete_path = NULL;

    if (item->first_child != NULL) {
        extract_file(ad1_file, item->first_child, root_folder);
    }

    if (item->next_item != NULL) {
        extract_file(ad1_file, item->next_item, root_folder);
    }
}