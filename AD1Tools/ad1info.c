#include "ad1info.h"
#include <argp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char* argp_program_version = "ad1info v.0.1.0";
const char* argp_program_bug_address = "<al3ks1sss@gmail.com> or https://github.com/al3ks1s/AD1-tools/issues";
static char doc[] = "Print important information of an AccessData AD1 Logical Image.";
static char args_doc[] = "ad1info [OPTIONS] -i FILENAME";

static struct argp_option options[] = {{"verbose", 'v', 0, OPTION_ARG_OPTIONAL, "Blurt a lotta text."},
                                       {"quiet", 'q', 0, OPTION_ARG_OPTIONAL, "Produce a quiet output."},
                                       {"tree", 't', 0, OPTION_ARG_OPTIONAL, "Produce a tree of the file hierarchy."},
                                       {"input", 'i', "FILE", 0, "Input AD1 file."},
                                       {0}};

struct arguments {
    enum { NORMAL, VERBOSE, QUIET } mode;

    char* ad1_file_path;
    bool tree;
};

static error_t
parse_opt(int key, char* arg, struct argp_state* state) {
    struct arguments* arguments = state->input;
    switch (key) {
        case 'v': arguments->mode = VERBOSE; break;
        case 'q': arguments->mode = QUIET; break;
        case 'i': arguments->ad1_file_path = arg; break;
        case 't': arguments->tree = true; break;
        case ARGP_KEY_ARG: return 0;
        default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

static struct arguments arguments;

void
print_info(ad1_session* session) {
    printf("%s\n", argp_program_version);
    printf("\n");
    printf("AD1 Segments info");
    print_segment_header(session->segment_header);
    printf("\n");

    printf("AD1 logical image info");
    print_logical_header(session->logical_header);
    printf("\n");

    if (arguments.tree) {
        print_tree(session->logical_header->first_item, 0);
    }
}

int
main(int argc, char* argv[]) {

    arguments.mode = NORMAL;
    arguments.ad1_file_path = 0;
    arguments.tree = false;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (!arguments.ad1_file_path) {
        argp_help(&argp, stdout, ARGP_HELP_USAGE, "ad1info");
        exit(EXIT_FAILURE);
    }

    ad1_session* session;

    session = open_ad1_session(arguments.ad1_file_path);

    print_info(session);

    close_ad1_session(session);

    return 0;
}