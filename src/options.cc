/*

Thomas Daley
11/5/2017
options.h

*/

#include "options.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_OPTS 10
#define OPT_UNKNOWN -1

#define OPT_L 0
#define OPT_K 1
#define OPT_S 2
#define OPT_Z 3
#define OPT_C 4
#define OPT_E 5
#define OPT_H 6
#define OPT_V 7
#define OPT_W 8
#define OPT_R 9

typedef struct OPTION {
    int count = 0;
    char *arg = NULL;
} OPTION;

OPTION opts[NUM_OPTS];
int argind = 1;

int optind = 1;
char *optarg = NULL;
int i = 0;
char *pos = NULL;
int moreopts = 0;

void free_args() {
    for (int i = 0; i < NUM_OPTS; i++) {
        free(opts[i].arg);
    }
}

void free_optarg() {
    free (optarg);
}

int opt_index(char opt) {
    switch(opt) { 
        case 'l': 
            return OPT_L;
        case 'k': 
            return OPT_K;
        case 's': 
            return OPT_S;
        case 'z': 
            return OPT_Z;
        case 'c': 
            return OPT_C;
        case 'e': 
            return OPT_E;
        case 'h': 
            return OPT_H;
        case 'v': 
            return OPT_V;
        case 'w': 
            return OPT_W;
        case 'r': 
            return OPT_R;
        default:
            return OPT_UNKNOWN;
    }
}

int pre_parse_opts(int argc, char **argv) {
    /* fill the opt array with number of times each opt is found */
    atexit(free_args);
    int total_unique = 0;
    for (int i = 1; i < argc; i++) {
        char *pos = argv[i];
        if (*pos == '-') {
            argind = i+1;
            while (*++pos) {
                int index = opt_index(*pos);
                if (index < 0) continue;
                if (!opts[index].count) total_unique++;
                opts[index].count++;
                if (i+1 >= argc) continue;
                if (*argv[i+1] == '-') continue;
                int size = sizeof(char) * (strlen(argv[i+1]));
                opts[index].arg = (char *) malloc(size + 1);
                memset(opts[index].arg, '\0', size + 1);
                memcpy(opts[index].arg, argv[i+1], size);
            }
        }
    }
    return total_unique;
}

char get_opt(int argc, char **argv) { 
    /* incremental arg parsing similar to get_opt.h */
    free(optarg);
    optarg = NULL;

    if (!moreopts) {
        i++;
        if (i >= argc) {
            atexit(free_optarg);
            return NULL;
        }
        pos = argv[i];
    }

    if (*pos == '-' || moreopts) {
        if (*++pos) {
            optind = i + 1;
            if (i+1 < argc) {
                if (*argv[i+1] != '-') {
                    int size = sizeof(char) * (strlen(argv[i+1]));
                    optarg = (char *) malloc(size + 1);
                    memset(optarg, '\0', size + 1);
                    memcpy(optarg, argv[i+1], size);
                }
            }
            if (*(pos+1)) moreopts = 1;
            return *pos;
        }
    }

    moreopts = 0;
    return get_opt(argc, argv);
}

int opt_count(char opt) {
    /* return the option count */
    int index = opt_index(opt);
    return index < 0 ? 0 : opts[index].count;
}

char *opt_arg(char opt) {
    /* return the option argument (can be NULL) */
    int index = opt_index(opt);
    return index < 0 ? NULL : opts[index].arg;
}
