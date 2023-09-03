#include "header.h"


void parse_args(int argc, char *argv[], flag_t *flags, arg_t *arg)
{
    // initiate char *name and copy archive name into variable
    int index = flags->INDEX_f;
    arg->name = (char *)malloc(sizeof(char) * (strlen(argv[index]) + 1));
    arg->files = (char **)malloc(sizeof(char*) * (argc - index + 1));
    strcpy(arg->name, argv[index]);

    index += 1;
    int i = 0;

    // initiate char **files and copy filenames into variable
    while (index < argc) {
        if (argv[index][0] != '-') {
            arg->files[i] = (char *)malloc(sizeof(char) * (strlen(argv[index]) + 1));
            strcpy(arg->files[i], argv[index]);

            i += 1;
        }
        index += 1;
    }

    arg->size = i;
}

int check_file_type(char* arg) 
{
    struct stat sb;

    if (lstat(arg, &sb) == -1) {
        printf("my_tar: cannot access '%s': No such file or directory\n", arg);
        return -1;
    }

    switch (sb.st_mode & S_IFMT) {
        case S_IFREG: break;        
        case S_IFLNK: return 1;
        case S_IFCHR: return 3;
        case S_IFBLK: return 4;
        case S_IFDIR: return 5;
        case S_IFIFO: return 6;
        case S_IFSOCK: break;
        default: break;
    }

    return 0;
}
