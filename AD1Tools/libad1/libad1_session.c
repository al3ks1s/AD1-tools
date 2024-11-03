#include "libad1_session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

ad1_session*
open_ad1_session(char* filep) {

    // Don't forget to close the session after you're done with the file, or else you'll leak a lot of memory.

    char filepath[PATH_MAX] = {0};

    realpath(filep, filepath);

    ad1_session* session;

    session = calloc(1, sizeof(ad1_session));

    FILE* headers_file = fopen(filepath, "rb");

    if (headers_file == NULL) {
        printf("Couldn't open file %s\n", filep);
        free(session);
        session = 0;
        exit(EXIT_FAILURE);
    }

    session->segment_header = read_segmented_header(headers_file);

    if (session->segment_header == 0) {
        close_ad1_session(session);
        exit(EXIT_FAILURE);
    }

    session->logical_header = read_logical_header(headers_file);

    fclose(headers_file);

    session->ad1_files = calloc(session->segment_header->segment_number, sizeof(ad1_file*));

    for (int i = 1; i <= session->segment_header->segment_number; i++) {

        session->ad1_files[i - 1] = calloc(1, sizeof(ad1_file));

        // Create filepaths for adX up to roughly INT_MAX, should be alright
        char extension[11] = {0};
        sprintf(extension, "%d", i);
        int extension_length = strlen(extension);

        session->ad1_files[i - 1]->filepath = calloc(strlen(filepath) + extension_length, sizeof(char));
        strcpy(session->ad1_files[i - 1]->filepath, filepath);
        memcpy(session->ad1_files[i - 1]->filepath + strlen(filepath) - 1, extension, extension_length);

        session->ad1_files[i - 1]->adfile = fopen(session->ad1_files[i - 1]->filepath, "rb");

        if (session->ad1_files[i - 1] == NULL) {
            printf("Couldn't find or open file : %s\n. ad1tools needs all segments to be in the same folder to "
                   "prolerly work.\n",
                   session->ad1_files[i - 1]->filepath);
            close_ad1_session(session);
            exit(EXIT_FAILURE);
        }

        session->ad1_files[i - 1]->segment_index = i;

        fseek(session->ad1_files[i - 1]->adfile, 0, SEEK_END);
        session->ad1_files[i - 1]->size = ftell(session->ad1_files[i - 1]->adfile) - AD1_LOGICAL_MARGIN;

        //fclose(session->ad1_files[i - 1]->adfile);
    }
    build_item_tree(session);

    return session;
}

void
close_ad1_session(ad1_session* session) {

    free_cache();

    for (int i = 0; i < session->segment_header->segment_number; i++) {

        fclose(session->ad1_files[i]->adfile);
        free(session->ad1_files[i]->filepath);
        free(session->ad1_files[i]);
    }

    free(session->ad1_files);

    free_all(session->segment_header, session->logical_header);

    free(session);
}
