#include "header.h"

/* functions in getopt.c */
int my_getopt(int argc, char *argv[], flag_t *flags);
int check_flags(int argc, char *const argv[], const char *optstring, flag_t *flags);
int getopt_errors(flag_t* flags);