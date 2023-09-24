#include "header.h"

/* run_getopt() initiates struct flag_t varaibles and then runs switch to check for flag options */
int my_getopt(int argc, char *argv[], flag_t* flags) {
    flags->OPT_c = 0;
    flags->OPT_r = 0;
    flags->OPT_u = 0;
    flags->OPT_t = 0;
    flags->OPT_x = 0;
    flags->OPT_f = 0;
    flags->INDEX_f = 0;
    flags->FILES = 0;
    int c;

    while ((c = check_flags(argc, argv, "crutxf", flags)) != -1) {
        switch (c) {
            case 'c':
                flags->OPT_c = 1;
                break;
            case 'r':
                flags->OPT_r = 1;
                break;
            case 'u':
                flags->OPT_u = 1;
                break;
            case 't':
                flags->OPT_t = 1;
                break;
            case 'x':
                flags->OPT_x = 1;
                break;
            case 'f':
                flags->OPT_f = 1;
                break;
            case '?':
                return EXIT_FAILURE;
            case '\0':
                break; // Move to next argument, no options
        }
    }

    return EXIT_SUCCESS;
}


/* getopt() checks for flag options as well as required arguments to complete command */
int check_flags(int argc, char *const argv[], const char *optstring, flag_t* flags) {
    static int optind = 1;  // Index of the next argument to process
    static int optpos = 1;  // Position within the current argument

    if (optind >= argc) {
        return -1;  // No more options
    }

    if (argv[optind][0] != '-' || argv[optind][1] == '\0') {
        optind += 1;
        return '\0';  // Non-option argument
    }

    // Get the current option character
    char current_opt = argv[optind][optpos++];

    // Find if the current option character is in the optstring
    const char *optchar = strchr(optstring, current_opt);
    if (optchar == NULL) {
        printf("%s: invalid option -- '%c'\n", argv[0], current_opt);
        optind++;
        optpos = 1;
        return '?';
    }

    // my_tar specific function to record argument after -f flag
    if (current_opt == 'f' && optind + 1 < argc) {
        flags->INDEX_f = optind + 1;
    } else if (current_opt == 'f') {
        flags->INDEX_f = 0;
    }
    if (current_opt == 'f' && optind + 2 < argc && argv[optind + 2][0] != '-') {
        flags-> FILES = 1;
    }

    if (argv[optind][optpos] == '\0') {
        optind++;
        optpos = 1;
    }


    return current_opt;
}


/* return option error if it exists */
int getopt_errors(flag_t* flags) 
{
    if (flags->OPT_c && flags->OPT_u) {
        char* error = "my_tar: Can't specify both -u and -c\n";
        write(2, error, 50);
        return EXIT_FAILURE;
    }
    if (flags->OPT_c && flags->OPT_r) {
        char* error = "my_tar: Can't specify both -r and -c\n";
        write(2, error, 50);
        return EXIT_FAILURE;
    }
    if (flags->OPT_r && flags->OPT_u) {
        char* error = "my_tar: Can't specify both -u and -r\n";
        write(2, error, 50);
        return EXIT_FAILURE;
    }
    if (flags->OPT_c && flags->OPT_x) {
        char* error = "my_tar: Can't specify both -x and -c\n";
        write(2, error, 50);
        return EXIT_FAILURE;
    }
    if (flags->OPT_r && !flags->OPT_f) {
        char* error = "my_tar: Cannot append to stdout.\n";
        write(2, error, 50);
        return EXIT_FAILURE;
    }
    if (flags->OPT_u && !flags->OPT_f) {
        char* error = "my_tar: Cannot append to stdout.\n";
        write(2, error, 50);
        return EXIT_FAILURE;
    }
    if ((flags->OPT_t && !flags->OPT_f) || (flags->OPT_x && !flags->OPT_f) || flags->INDEX_f == 0) {
        char* error = "Usage:\n  List:     my_tar -tf <archive-filename>\n  Extract:  my_tar -xf <archive-filename>\n  Create:   my_tar -cf <archive-filename> [filenames...]\n";
        write(2, error, 150);
        return EXIT_FAILURE;
    } 
    if (flags->OPT_f && !flags->OPT_c && !flags->OPT_r && !flags->OPT_t && !flags->OPT_u && !flags->OPT_x) {
        char* error = "my_tar: Must specify one of -c, -r, -t, -u, -x\n";
        write(2, error, 50);
        return EXIT_FAILURE;
    }
    if (!flags->OPT_f && !flags->OPT_c && !flags->OPT_r && !flags->OPT_t && !flags->OPT_u && !flags->OPT_x) {
        char* error = "my_tar: Must specify one of -c, -r, -t, -u, -x\n";
        write(2, error, 50);
        return EXIT_FAILURE;
    }
    if ((flags->OPT_c || flags->OPT_r || flags->OPT_u) && flags->FILES == 0){
        char* error = "my_tar: no files or directories specified\n";
        write(2, error, 50);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

