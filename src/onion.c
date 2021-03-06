/*********************************************************************
 * Copyright (c) 2011-2015 Jan Pomikalek                             *
 * All rights reserved.                                              *
 *                                                                   *
 * This software is licensed as described in the file COPYING, which *
 * you should have received as part of this distribution.            *
 *********************************************************************/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <Judy.h>
#include "buzhash.h"
#include "version.h"

#define BITMASK_HIGH63 0xfffffffffffffffeul

#define NGRAM_SIZE 7
#define DUPL_THRES 0.5
#define DOC_TAG "doc"
#define PAR_TAG "p"
#define TRIM_HASHES 64
#define MAX_STUB_LENGTH 20
#define BUFFER_SIZE 16777216

// options
int Ngram_size = NGRAM_SIZE;
float Dupl_thres = DUPL_THRES;
char* Doc_tag = DOC_TAG;
char* Par_tag = PAR_TAG;
int Strip_dupl = 0;
int No_smoothing = 0;
int Trim_hashes = TRIM_HASHES;
int Max_stub_length = MAX_STUB_LENGTH;
long Buffer_size = BUFFER_SIZE;
int Quiet = 0;
char* Dupl_hashes_path = NULL;
FILE* Input;
long int Input_size;

void print_usage(FILE *stream) {
    fprintf(stream, "\
Usage: onion [OPTIONS] [FILE]\n\
Mark duplicate text parts in the input vertical file.\n\
\n\
 -f FILE   hashes of duplicate n-grams\n\
 -n NUM    n-gram length (default: %i)\n\
 -t NUM    duplicate content threshold (default: %.1f)\n\
 -d STR    document tag (default: %s)\n\
 -p STR    paragraph tag (default: %s)\n\
 -s        strip duplicate parts (rather than mark)\n\
 -m        no smoothing\n\
 -T NUM    trim n-gram hashes to NUM bits (default: %i)\n\
 -l NUM    max stub length (default: %i)\n\
 -b NUM    buffer size, in bytes (default: %i)\n\
 -q        quiet; suppress all output except for errors\n\
\n\
 -V        print version information and exit\n\
 -h        display this help and exit\n\
\n\
With no FILE, or when FILE is -, read standard input.\n\
Output is written to standard output.\n\
\n\
Project home page: <http://code.google.com/p/onion/>\n",
        NGRAM_SIZE, DUPL_THRES, DOC_TAG, PAR_TAG,
        TRIM_HASHES, MAX_STUB_LENGTH, BUFFER_SIZE);
}

void print_progress(char* task_descr, unsigned long int processed_bytes,
        float percent_done) {
    time_t now;
    struct rusage usage;
    time(&now);
    getrusage(RUSAGE_SELF, &usage);
    fprintf(stderr, "[%.24s] onion: %s: %6li MB processed", ctime(&now),
            task_descr, processed_bytes / (1024 * 1024));
    if (percent_done >= 0)
        fprintf(stderr, " (%6.2f%%)", percent_done);
    fprintf(stderr, "\t%6li MB RAM used", usage.ru_maxrss / 1024);
    fprintf(stderr, "\n");
}

int main(int argc, char **argv) {
    // get options
    int c;
    char *endptr;
    while ((c = getopt(argc, argv, "f:n:t:d:p:smT:l:b:qVh")) != -1) {
        errno = 0;
        switch (c) {
            case 'f':
                Dupl_hashes_path = optarg;
                break;
            case 'n':
                Ngram_size = strtol(optarg, &endptr, 10);
                if (errno != 0 || *endptr != '\0') {
                    fprintf(stderr, "Integer value expected for -n, got: %s\n", optarg);
                    print_usage(stderr);
                    return 1;
                }
                break;
            case 't':
                Dupl_thres = strtod(optarg, &endptr);
                if (errno != 0 || *endptr != '\0') {
                    fprintf(stderr, "Float value expected for -t, got: %s\n", optarg);
                    print_usage(stderr);
                    return 1;
                }
                break;
            case 'd':
                Doc_tag = optarg;
                break;
            case 'p':
                Par_tag = optarg;
                break;
            case 's':
                Strip_dupl = 1;
                break;
            case 'm':
                No_smoothing = 1;
                break;
            case 'T':
                Trim_hashes = strtol(optarg, &endptr, 10);
                if (errno != 0 || *endptr != '\0') {
                    fprintf(stderr, "Integer value expected for -T, got: %s\n", optarg);
                    print_usage(stderr);
                    return 1;
                }
                break;
            case 'l':
                Max_stub_length = strtol(optarg, &endptr, 10);
                if (errno != 0 || *endptr != '\0') {
                    fprintf(stderr, "Integer value expected for -l, got: %s\n", optarg);
                    print_usage(stderr);
                    return 1;
                }
                break;
            case 'b':
                Buffer_size = strtol(optarg, &endptr, 10);
                if (errno != 0 || *endptr != '\0') {
                    fprintf(stderr, "Integer value expected for -b, got: %s\n", optarg);
                    print_usage(stderr);
                    return 1;
                }
                break;
            case 'q':
                Quiet = 1;
                break;
            case 'V':
                print_version("onion");
                return 0;
            case 'h':
                print_usage(stdout);
                return 0;
            case '?':
                print_usage(stderr);
                return 1;
        }
    }

    Input = stdin;
    Input_size = -1;
    if (optind < argc) {
        char* filename = argv[optind];
        if (strcmp(filename, "-") != 0) {
            errno = 0;
            Input = fopen(filename, "r");
            if (errno != 0) {
                fprintf(stderr, "Unable to open %s for reading.\n", filename);
                return 1;
            }
            fseek(Input, 0L, SEEK_END);
            Input_size = ftell(Input);
            fseek(Input, 0L, SEEK_SET);
        }
    }

    unsigned long int total_processed_bytes = 0;

    // patterns
    char *doc_tag = (char*) malloc((strlen(Doc_tag) + 1 + 1) * sizeof(char));
    strcat(strcpy(doc_tag, "<"), Doc_tag);
    char *doc_end_tag = (char*) malloc((strlen(Doc_tag) + 3 + 1) * sizeof(char));
    strcat(strcat(strcpy(doc_end_tag, "</"), Doc_tag), ">");
    char *par_tag = (char*) malloc((strlen(Par_tag) + 1 + 1) * sizeof(char));
    strcat(strcpy(par_tag, "<"), Par_tag);
    char *par_end_tag = (char*) malloc((strlen(Par_tag) + 3 + 1) * sizeof(char));
    strcat(strcat(strcpy(par_end_tag, "</"), Par_tag), ">");

    int doc_tag_len = strlen(doc_tag);
    int doc_end_tag_len = strlen(doc_end_tag);
    int par_tag_len = strlen(par_tag);
    int par_end_tag_len = strlen(par_end_tag);

    // bitmask for trimming ngram hashes
    hash_t hash_bitmask = 0xfffffffffffffffful;
    int bitshift = 64 - Trim_hashes;
    if (bitshift > 0)
        hash_bitmask >>= bitshift;

    // data structures
    int buffer_size = 0;
    int buffer_content = 0;
    char* buffer = (char*) malloc((Buffer_size+1) * sizeof(char));
    char** tokens = (char**) malloc((Buffer_size+1) * sizeof(char*));
    int* pars = (int*) malloc((Buffer_size+1) * sizeof(int)); // array of starting tokens
    int* par_len = (int*) malloc((Buffer_size+1) * sizeof(int));
    char* bad_par = (char*) malloc((Buffer_size+1) * sizeof(char));
    int* docs = (int*) malloc((Buffer_size+1) * sizeof(int)); // array of starting pars 
    int token_count, par_count, doc_count;

    // buzhash
    buzhash_buffer_t bh_buffer;
    buzhash_init_buffer(&bh_buffer, Ngram_size);

    // judy - for global duplicates
    int judy_rc;
    Pvoid_t judy = (Pvoid_t) NULL;

    // ljudy - for local (document level) duplicates
    Pvoid_t ljudy = (Pvoid_t) NULL;

    // read hashes of duplicate n-grams if available
    int have_dupl_ngrams = 0;
    if (Dupl_hashes_path != NULL) {
        have_dupl_ngrams = 1;
        errno = 0;
        FILE* ngrams_fp = fopen(Dupl_hashes_path, "r");
        if (errno != 0) {
            fprintf(stderr, "Unable to open %s for reading.\n", Dupl_hashes_path);
            return 1;
        }
        fseek(ngrams_fp, 0L, SEEK_END);
        unsigned long int ngrams_size = ftell(ngrams_fp);
        fseek(ngrams_fp, 0L, SEEK_SET);

        unsigned long int bytes_read = 0;
        hash_t hash;
        while (fread(&hash, sizeof(hash), 1, ngrams_fp)) {
            bytes_read+= sizeof(hash);
            hash_t masked_hash = hash & hash_bitmask;
            // store only the 63 most significant bits of the hash;
            // reserve the last bit as a flag (seen / unseen)
            J1S(judy_rc, judy, masked_hash & BITMASK_HIGH63);

            // print progress information
            if (!Quiet && bytes_read % (10000000 * sizeof(hash)) == 0) {
                float percent_done = -1;
                if (ngrams_size > 0)
                    percent_done = 100.0 * bytes_read / ngrams_size;
                print_progress("reading hashes", bytes_read, percent_done);
            }
        }

        if (!Quiet)
            print_progress("reading hashes", bytes_read, 100);
    }

    while (!feof(Input)) {
        int bytes_read = fread(buffer+buffer_content, sizeof(char), Buffer_size-buffer_content, Input);

        buffer_size = buffer_content + bytes_read;
        buffer[buffer_size] = '\0'; // make it a string
        char* buffer_pos = buffer;

        // find tokens
        token_count = 0;
        tokens[token_count++] = buffer_pos++;
        while ((buffer_pos = strchr(buffer_pos, '\n')) != NULL) {
            buffer_pos[0] = '\0';
            tokens[token_count++] = ++buffer_pos;
        }

        // find docs and paragraphs
        doc_count = 0;
        par_count = 0;
        docs[doc_count++] = 0;
        pars[par_count++] = 0;
        int start_doc_next = 0;
        int start_par_next = 0;
        int i;
        for (i=1; i<token_count; i++) {
            // "<doc>" or "<doc "
            if (start_doc_next || (strncmp(tokens[i], doc_tag, doc_tag_len) == 0 &&
                    (tokens[i][doc_tag_len] == ' ' || tokens[i][doc_tag_len] == '>'))) {
                docs[doc_count++] = par_count;
                pars[par_count++] = i;
                start_doc_next = 0;
            }
            // "</doc>"
            else if (strncmp(tokens[i], doc_end_tag, doc_end_tag_len) == 0) {
                start_doc_next = 1;
            }
            // "<p>" or "<p "
            else if (start_par_next || (strncmp(tokens[i], par_tag, par_tag_len) == 0 &&
                    (tokens[i][par_tag_len] == ' ' || tokens[i][par_tag_len] == '>'))) {
                pars[par_count++] = i;
                start_par_next = 0;
            }
            // "</p>"
            else if (strncmp(tokens[i], par_end_tag, par_end_tag_len) == 0) {
                start_par_next = 1;
            }
        }

        if (doc_count == 1 && !feof(Input)) {
            // full buffer contains only one document
            fprintf(stderr, "Too long document at byte %li.\n", total_processed_bytes);
            return 1;
        }

        if (feof(Input)) {
            // create sentinels
            docs[doc_count++] = par_count;
            if (strlen(tokens[token_count-1]) == 0) {
                // files ending with a newline have a natural sentinel
                // (the last zero-length token)
                pars[par_count++] = token_count - 1;
            }
            else {
                // for other files, we need to make up the last token
                pars[par_count++] = token_count;
                tokens[token_count++] = buffer + buffer_size;
            }
        }

        // for all documents
        int doc_i;
        for (doc_i=0; doc_i<doc_count-1; doc_i++) {
            buzhash_clear_buffer(&bh_buffer);
            J1FA(judy_rc, ljudy);
            // for all paragraphs in the document
            int par_i;
            for (par_i=docs[doc_i]; par_i<docs[doc_i+1]; par_i++) {
                int total_tokens = 0;
                int bad_tokens = 0;
                /* prev_bad_tokens is the number of tokens in the current
                 * n-gram which are contained in one of the previous bad
                 * n-grams.
                 *
                 * At the beginning of a new paragraph we need to pretend that
                 * there are Ngram_size prev_bad_tokens so that the leading
                 * bad n-grams do not generate too many bod tokens. */
                int prev_bad_tokens = Ngram_size;
                // for all tokens in the paragraph
                int tok_i;
                for (tok_i=pars[par_i]; tok_i<pars[par_i+1]; tok_i++) {
                    char *token = tokens[tok_i];
                    if (token[0] == '<')
                        continue;
                    total_tokens++;
                    prev_bad_tokens--;
                    if (prev_bad_tokens < 0)
                        prev_bad_tokens = 0;
                    hash_t hash = buzhash(token, &bh_buffer);
                    hash_t masked_hash = hash & hash_bitmask;
                    if (!buzhash_is_full_buffer(&bh_buffer))
                        continue;
                    J1T(judy_rc, ljudy, hash);
                    if (!judy_rc) {
                        if (have_dupl_ngrams) {
                            // test with the last bit set to 1
                            // (check against already seen duplicate ngrams)
                            J1T(judy_rc, judy, masked_hash | 1);
                        }
                        else {
                            J1T(judy_rc, judy, masked_hash);
                        }
                    }
                    if (judy_rc) {
                        bad_tokens+= Ngram_size - prev_bad_tokens;
                        prev_bad_tokens = Ngram_size;
                    } 
                    J1S(judy_rc, ljudy, hash);
                }

                // remember the length of the paragraph
                par_len[par_i] = total_tokens;

                // mark bad paragraphs
                bad_par[par_i] = (total_tokens > 0 &&
                        1.0 * bad_tokens / total_tokens > Dupl_thres);
            }

            // smoothing
            if (!No_smoothing) {
                int last_bad_par = docs[doc_i] - 1;
                int stub_length = 0;
                for (par_i=docs[doc_i]; par_i<=docs[doc_i+1]; par_i++) {
                    if (par_i==docs[doc_i+1] || bad_par[par_i]) {
                        if (stub_length <= Max_stub_length) {
                            // remove stub
                            int par_j;
                            for (par_j=last_bad_par+1; par_j<par_i; par_j++)
                                bad_par[par_j] = 1;
                        }
                        last_bad_par = par_i;
                        stub_length = 0;
                    }
                    else {
                        stub_length+= par_len[par_i];
                    }
                }
            }

            // is there at least one good paragraph?
            int all_bad = 1;
            for (par_i=docs[doc_i]; par_i<docs[doc_i+1]; par_i++) {
                if (!bad_par[par_i] && par_len[par_i] > 0) {
                    all_bad = 0;
                    break;
                }
            }

            buzhash_clear_buffer(&bh_buffer);
            // for all paragraphs in the document (again)
            for (par_i=docs[doc_i]; par_i<docs[doc_i+1]; par_i++) {
                int first_token = pars[docs[doc_i]];
                int last_token = pars[docs[doc_i+1]] - 1;
                // for all tokens in the paragraph
                int tok_i;
                for (tok_i=pars[par_i]; tok_i<pars[par_i+1]; tok_i++) {
                    char* token = tokens[tok_i];
                    int bad_token = bad_par[par_i];
                    // tags at document boundaries are preserved if there is
                    // at least one good paragraph, removed otherwise
                    if (tok_i == first_token && 
                            strncmp(token, doc_tag, doc_tag_len) == 0)
                        bad_token = all_bad;
                    if (tok_i == last_token &&
                            strcmp(token, doc_end_tag) == 0)
                        bad_token = all_bad;
                    // print output
                    if (Strip_dupl) {
                        if (!bad_token)
                            printf("%s\n", token);
                    }
                    else {
                        printf("%i\t%s\n", bad_token, token);
                    }
                    if (token[0] == '<')
                        continue;
                    // store hashes of n-grams
                    hash_t hash = buzhash(token, &bh_buffer);
                    hash_t masked_hash = hash & hash_bitmask;
                    if (!buzhash_is_full_buffer(&bh_buffer))
                        continue;
                    if (!bad_par[par_i]) {
                        if (have_dupl_ngrams) {
                            // If we have the list of hashes of all duplicate
                            // n-grams, we set the least significant bit of the
                            // stored hash to 1 if we have seen the matching
                            // duplicate n-gram to indicate it has been seen.
                            // Unique n-grams are ignored.
                            J1U(judy_rc, judy, masked_hash & BITMASK_HIGH63);
                            if (judy_rc)
                                J1S(judy_rc, judy, masked_hash | 1); 
                        }
                        else {
                            // otherwise we have to store hashes of all n-grams
                            J1S(judy_rc, judy, masked_hash); 
                        }
                    }
                }
            }
        }

        // copy the unprocessed data to the beginning of the buffer
        if (!feof(Input)) {
            char* last_doc_fst_tok = tokens[pars[docs[doc_count-1]]];
            int processed_bytes = last_doc_fst_tok - buffer;
            total_processed_bytes+= processed_bytes;
            int remaining_bytes = buffer_size - processed_bytes;
            char* buffer_end = last_doc_fst_tok + remaining_bytes;
            char* pos;
            // replace \0s with EOLs (revert the buffer contents to original)
            for (pos=last_doc_fst_tok; pos<buffer_end; pos++)
                if (pos[0] == '\0')
                    pos[0] = '\n';
            // print progress information
            if (!Quiet) {
                float percent_done = -1;
                if (Input_size > 0)
                    percent_done = 100.0 * total_processed_bytes / Input_size;
                print_progress("removing duplicates", total_processed_bytes, percent_done);
            }
            memmove(buffer, last_doc_fst_tok, remaining_bytes);
            buffer_content = remaining_bytes;
        }
    } 

    // print progress information
    total_processed_bytes+= buffer_size;
    if (!Quiet)
        print_progress("removing duplicates", total_processed_bytes, 100);

    if (Input != stdin)
        fclose(Input);

    return 0;
}
