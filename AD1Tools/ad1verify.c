#include "ad1verify.h"
#include <argp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char* argp_program_version = "ad1verify v.0.0.1";
const char* argp_program_bug_address = "<al3ks1sss@gmail.com> or https://github.com/al3ks1s/AD1-tools/issues";
static char doc[] = "Checks the integrity of an AccessData AD1 Logical Image.";
static char args_doc[] = "ad1verify [OPTIONS] -i FILENAME";

static struct argp_option options[] = {
    {"full-ckeck", 'f', 0, OPTION_ARG_OPTIONAL, "Check all files hashes against their saved hash."},
    {"sha1", 's', 0, OPTION_ARG_OPTIONAL, "Compare sha1 hashes instead of md5."},
    {"input", 'i', "FILE", 0, "Input AD1 file."},
    {"verbose", 'v', 0, OPTION_ARG_OPTIONAL, "Blurt a lotta text."},
    {"quiet", 'q', 0, OPTION_ARG_OPTIONAL, "Produce a quiet output."},
    {0}};

struct arguments {
    enum { NORMAL, VERBOSE, QUIET } mode;

    bool sha1;
    bool full_check;

    char* ad1_file_path;
};

static error_t
parse_opt(int key, char* arg, struct argp_state* state) {
    struct arguments* arguments = state->input;
    switch (key) {
        case 'v': arguments->mode = VERBOSE; break;
        case 'q': arguments->mode = QUIET; break;
        case 's': arguments->sha1 = true; break;
        case 'f': arguments->full_check = true; break;
        case 'i': arguments->ad1_file_path = arg; break;
        case ARGP_KEY_ARG: return 0;
        default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

int
main(int argc, char* argv[]) {

    struct arguments arguments;

    arguments.mode = NORMAL;
    arguments.full_check = false;
    arguments.sha1 = false;
    arguments.ad1_file_path = 0;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (!arguments.ad1_file_path) {
        argp_help(&argp, stdout, ARGP_HELP_USAGE, "ad1verify");
        exit(EXIT_FAILURE);
    }

    ad1_session* session;

    session = open_ad1_session(arguments.ad1_file_path);

    if (arguments.full_check) {
        if (!arguments.sha1) {
            printf("Starting md5 check for all files.\n");
            recurse_md5(session->ad1_file, session->logical_header->first_item);
        } else {
            printf("Starting sha1 check for all files.\n");
            recurse_sha1(session->ad1_file, session->logical_header->first_item);
        }
    } else {
        printf("Not in yet\n");
    }

    close_ad1_session(session);

    return 0;
}