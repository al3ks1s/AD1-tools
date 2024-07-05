#include "ad1extract.h"
#include <argp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <zlib.h>
#include "libad1/libad1_extract.h"

const char* argp_program_version = "ad1extract v.0.1.0";
const char* argp_program_bug_address = "<al3ks1sss@gmail.com> or https://github.com/al3ks1s/AD1-tools/issues";
static char doc[] = "Extract the content of an AccessData AD1 Logical Image.";
static char args_doc[] = "ad1extract [OPTIONS] -i FILENAME";

static struct argp_option options[] = {
    {"skip-hash", 's', 0, OPTION_ARG_OPTIONAL, "Skip the integrity check of a file on extraction."},
    {"verbose", 'v', 0, OPTION_ARG_OPTIONAL, "Blurt a lotta text."},
    {"metadata", 'm', 0, OPTION_ARG_OPTIONAL, "Apply metadatas to extracted files (eg: timestamps)."},
    {"quiet", 'q', 0, OPTION_ARG_OPTIONAL, "Produce a quiet output."},
    {"output-dir", 'd', "DIR", 0, "Directory to extract the files to."},
    {"input", 'i', "FILE", 0, "Input AD1 file."},
    {0}};

struct arguments {
    enum { ARGNORMAL, ARGVERBOSE, ARGQUIET } mode;

    bool integrity_check;
    bool metadata;

    char* output_dir;
    char* ad1_file_path;
};

static error_t
parse_opt(int key, char* arg, struct argp_state* state) {
    struct arguments* arguments = state->input;
    switch (key) {
        case 'v': arguments->mode = VERBOSE; break;
        case 'q': arguments->mode = QUIET; break;
        case 's': arguments->integrity_check = true; break;
        case 'd': arguments->output_dir = arg; break;
        case 'i': arguments->ad1_file_path = arg; break;
        case 'm': arguments->metadata = true; break;
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
    arguments.integrity_check = false;
    arguments.metadata = false;
    arguments.ad1_file_path = 0;
    arguments.output_dir = 0;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (!arguments.ad1_file_path) {
        argp_help(&argp, stdout, ARGP_HELP_USAGE, "ad1extract");
        exit(EXIT_FAILURE);
    }

    if (!arguments.output_dir) {
        argp_help(&argp, stdout, ARGP_HELP_USAGE, "ad1extract");
        exit(EXIT_FAILURE);
    }

    ad1_session* session;

    session = open_ad1_session(arguments.ad1_file_path);

    session->mode = arguments.mode;

    extract_all(session, arguments.output_dir);

    close_ad1_session(session);

    return 0;
}