#include <argp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "ad1decrypt.h"

const char* argp_program_version = "ad1decrypt v.0.1.0";
const char* argp_program_bug_address = "<al3ks1sss@gmail.com> or https://github.com/al3ks1s/AD1-tools/issues";
static char doc[] = "Print important information of an AccessData AD1 Logical Image.";
static char args_doc[] = "ad1decrypt [OPTIONS] -i FILENAME";

static struct argp_option options[] = {
    {"verbose", 'v', 0, OPTION_ARG_OPTIONAL, "Blurt a lotta text."},
    {"quiet", 'q', 0, OPTION_ARG_OPTIONAL, "Produce a quiet output."},
    {"password", 'p', "PASSWORD", OPTION_ARG_OPTIONAL, "Password to open the image."},
    {"output", 'o', "FILE", OPTION_ARG_OPTIONAL, "Path of the decrypted output image."},
    {"input", 'i', "FILE", 0, "Input AD1 file."},
    {0}};

struct arguments {
    enum { ARGNORMAL, ARGVERBOSE, ARGQUIET } mode;

    char* ad1_file_path;
    char* ad1_output;
    char* password;
};

static error_t
parse_opt(int key, char* arg, struct argp_state* state) {
    struct arguments* arguments = state->input;
    switch (key) {
        case 'v': arguments->mode = VERBOSE; break;
        case 'q': arguments->mode = QUIET; break;
        case 'i': arguments->ad1_file_path = arg; break;
        case 'o': arguments->ad1_output = arg; break;
        case 'p': arguments->password = arg; break;

        case ARGP_KEY_ARG: return 0;
        default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

static struct arguments arguments;

int
main(int argc, char* argv[]) {

    arguments.mode = NORMAL;
    arguments.ad1_file_path = 0;
    arguments.ad1_output = 0;
    arguments.password = 0;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (!arguments.ad1_file_path) {
        argp_help(&argp, stdout, ARGP_HELP_USAGE, "ad1decrypt");
        exit(EXIT_FAILURE);
    }

    if (!arguments.password) {
        argp_help(&argp, stdout, ARGP_HELP_USAGE, "ad1decrypt");
        exit(EXIT_FAILURE);
    }

    FILE* encrypted_file = 0;

    encrypted_file = fopen(arguments.ad1_file_path, "rb");

    ad1_encrypt_header* eheader = read_encrypt_header(encrypted_file);

    print_encrypt_header(eheader);

    return 0;
}