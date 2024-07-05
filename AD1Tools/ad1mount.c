#include "ad1mount.h"
#include <argp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const char* argp_program_version = "ad1mount v.0.1.0";
const char* argp_program_bug_address = "<al3ks1sss@gmail.com> or https://github.com/al3ks1s/AD1-tools/issues";
static char doc[] = "Mounts an AccessData AD1 Logical Image as a read only filesystem.";
static char args_doc[] = "ad1verify [OPTIONS] -i FILENAME";

static struct argp_option options[] = {{"verbose", 'v', 0, OPTION_ARG_OPTIONAL, "Blurt a lotta text."},
                                       {"quiet", 'q', 0, OPTION_ARG_OPTIONAL, "Produce a quiet output."},
                                       {"input", 'i', "FILE", 0, "Input AD1 file."},
                                       {"mnt", 'm', "DIR", 0, "Inout AD1 file."},
                                       {0}};

struct arguments {
    enum { ARGNORMAL, ARGVERBOSE, ARGQUIET } mode;

    bool sha1;
    bool full_check;

    char* mount_dir;
    char* ad1_file_path;
};

static error_t
parse_opt(int key, char* arg, struct argp_state* state) {
    struct arguments* arguments = state->input;
    switch (key) {
        case 'v': arguments->mode = VERBOSE; break;
        case 'q': arguments->mode = QUIET; break;
        case 'm': arguments->mount_dir = arg; break;
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
    arguments.ad1_file_path = 0;
    arguments.mount_dir = 0;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (!arguments.ad1_file_path) {
        argp_help(&argp, stdout, ARGP_HELP_USAGE, "ad1mount");
        exit(EXIT_FAILURE);
    }

    if (!arguments.mount_dir) {
        argp_help(&argp, stdout, ARGP_HELP_USAGE, "ad1mount");
        exit(EXIT_FAILURE);
    }

    ad1_session* session;

    session = open_ad1_session(arguments.ad1_file_path);

    session->mode = arguments.mode;

    close_ad1_session(session);

    return 0;
}