#include "libad1_session.h"
#include <stdio.h>
#include <stdlib.h>

ad1_session*
open_ad1_session(char* filepath) {

    // Don't forget to close the session after you're done with the file, or else you'll leak a lot of memory.

    ad1_session* session;

    session = (ad1_session*)calloc(1, sizeof(ad1_session));

    session->ad1_file = fopen(filepath, "rb");

    if (session->ad1_file == NULL) {
        printf("Couldn't open file");
        free(session);
        session = 0;
        exit(EXIT_FAILURE);
    }

    session->segment_header = read_segmented_header(session->ad1_file);
    session->logical_header = read_logical_header(session->ad1_file);

    build_item_tree(session->ad1_file, session->logical_header);

    return session;
}

void
close_ad1_session(ad1_session* session) {

    free_all(session->segment_header, session->logical_header);

    fclose(session->ad1_file);

    free(session);
}