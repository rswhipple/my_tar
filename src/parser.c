#include <stdio.h>
#include "../include/parser.h"
#include "../include/helper.h"


int parse_args(int argc, char *argv[], flag_t *flags, arg_t *arg)
{
    // initiate char *name and copy archive name into variable
    int index = flags->INDEX_f;
    arg->name = (char *)malloc(sizeof(char) * (my_strlen(argv[index]) + 1));
    arg->files = (char **)malloc(sizeof(char*) * (argc - index + 1));
    my_strcpy(arg->name, argv[index]);

    index += 1;
    int i = 0;

    // initiate char **files and copy filenames into variable
    while (index < argc) {
        if (argv[index][0] != '-') {
            /* make sure file exists */
            int status = check_file_type(argv[index]);
            if (status == -1) {
                error_message(argv[index]);
                return EXIT_FAILURE;
            }
            
            arg->files[i] = (char *)malloc(sizeof(char) * (my_strlen(argv[index]) + 1));
            my_strcpy(arg->files[i], argv[index]);

            i += 1;
        }
        index += 1;
    }

    arg->size = i;

    return EXIT_SUCCESS;
}

