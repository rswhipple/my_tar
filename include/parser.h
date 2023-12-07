#include "header.h"

/* functions in parser.c */
int parse_args(int argc, char *argv[], flag_t *flags, arg_t *arg);
int check_file_type(char* arg);
void error_message(char* name);