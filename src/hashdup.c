/*********************************************************************
 * Copyright (c) 2011 Jan Pomikalek                                  *
 * All rights reserved.                                              *
 *                                                                   *
 * This software is licensed as described in the file COPYING, which *
 * you should have received as part of this distribution.            *
 *********************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <Judy.h>
#include "buzhash.h"
#include "version.h"

#define OUTPUT_FILE "duphashes"

// options
char *Output_file = OUTPUT_FILE;
int Quiet = 0;
FILE* Input;
long int Input_size;

void print_usage(FILE *stream) {
    fprintf(stream, "\
Usage: hashdup [OPTIONS] FILE [FILE...]\n\
Identify duplicate hashes.\n\
\n\
 -o FILE   output file (default: %s)\n\
 -q        quiet; suppress all output except for errors\n\
\n\
 -V        print version information and exit\n\
 -h        display this help and exit\n\
\n\
Project home page: <http://code.google.com/p/onion/>\n",
        OUTPUT_FILE);
}

void print_progress(unsigned long int processed_bytes, float percent_done) {
    time_t now;
    struct rusage usage;
    time(&now);
    getrusage(RUSAGE_SELF, &usage);
    fprintf(stderr, "[%.24s] hashdup: %6li MB processed", ctime(&now),
            processed_bytes / (1024 * 1024));
    if (percent_done >= 0)
        fprintf(stderr, " (%6.2f%%)", percent_done);
    fprintf(stderr, "\t%6li MB RAM used", usage.ru_maxrss / 1024);
    fprintf(stderr, "\n");
}

int main(int argc, char **argv) {
    // get options
    int c;
    while ((c = getopt(argc, argv, "o:qVh")) != -1) {
        errno = 0;
        switch (c) {
            case 'o':
                Output_file = optarg;
                break;
            case 'q':
                Quiet = 1;
                break;
            case 'V':
                print_version("hashdup");
                return 0;
            case 'h':
                print_usage(stdout);
                return 0;
            case '?':
                print_usage(stderr);
                return 1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "No input.\n");
        print_usage(stderr);
        return 1;
    }

    // output file
    errno = 0;
    FILE* output_fp = fopen(Output_file, "w");
    if (errno != 0) {
        fprintf(stderr, "Unable to open %s for writing.\n", Output_file);
        return 1;
    }

    // judy
    int judy_rc;
    Pvoid_t judy1 = (Pvoid_t) NULL;
    Pvoid_t judy2 = (Pvoid_t) NULL;

    // other variables
    unsigned long int processed_bytes = 0;
    int input_files_count = argc - optind;

    // for all input files
    int i;
    for (i=optind; i<argc; i++) {
        // open file
        char* filename = argv[i];
        errno = 0;
        FILE* input_fp = fopen(filename, "r");
        if (errno != 0) {
            fprintf(stderr, "Unable to open %s for reading.\n", filename);
            return 1;
        }

        // estimate total input size
        fseek(input_fp, 0L, SEEK_END);
        unsigned long int file_size = ftell(input_fp);
        fseek(input_fp, 0L, SEEK_SET);
        int processed_files = i - optind;
        int est_input_size;
        if (i == argc - 1) {
            // processing the last file
            est_input_size = processed_bytes + file_size;
        }
        else {
            est_input_size = (processed_bytes + file_size) / 
                    (processed_files + 1) * input_files_count;
        }

        // reset judy
        J1FA(judy_rc, judy1);
        J1FA(judy_rc, judy2);

        // for each hash in the file
        hash_t hash;
        while (fread(&hash, sizeof(hash), 1, input_fp)) {
            J1S(judy_rc, judy1, hash);
            if (!judy_rc) {
                // if found in judy1, test judy2
                J1S(judy_rc, judy2, hash);
                if (judy_rc) {
                    // if not found in judy2, send to output
                    // (duplicate encountered for the first time)
                    fwrite(&hash, sizeof(hash), 1, output_fp);
                }
            }
            processed_bytes+= sizeof(hash);

            // print progress information
            if (!Quiet && processed_bytes % (10000000 * sizeof(hash)) == 0) {
                float percent_done = -1;
                if (est_input_size > 0)
                    percent_done = 100.0 * processed_bytes / est_input_size;
                print_progress(processed_bytes, percent_done);
            }
        }

        fclose(input_fp);
    }

    // print progress information
    if (!Quiet)
        print_progress(processed_bytes, 100);

    fclose(output_fp);

    return 0;
}
