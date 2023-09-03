#include "header.h"

int main(int argc, char *argv[]) 
{
    // Get options
    flag_t *flag = (flag_t *)malloc(sizeof(flag_t) + 1);
    if (my_getopt(argc, argv, flag)) {
        free(flag);
        return EXIT_FAILURE;
    } else if (getopt_errors(flag)) {
        free(flag);
        return EXIT_FAILURE;
    }

    // Parse arguments
    arg_t *arg = (arg_t *)malloc(sizeof(arg_t) + 1);
    parse_args(argc, argv, flag, arg);

    // Extract archive contents
    if (flag->OPT_x == 1) {
        // If destination for extraction was provided
        if (arg->size > 0) {
            extract_archive(arg->name, arg->files[0]);
        } // If no destination, extract to current folder
        else {
            extract_archive(arg->name, "./");
        }  
    } 

    // Create or open archive
    int fd = 0;
    tar_node_t* head = NULL; // Initiate linklist head
    if (flag->OPT_u == 1) {
        head = create_tar_node_list(arg->name);
    }
    if (flag->OPT_c == 1 || flag->OPT_r == 1 || flag->OPT_u == 1) {
        int status = initiate_archive_fd(arg, flag, fd);
        if (status == EXIT_FAILURE) return EXIT_FAILURE;
    }

    // Write or add to archive
    if (flag->OPT_r == 1 || flag->OPT_c == 1 || flag->OPT_u == 1) {
        int status = write_new_or_add_to_archive(arg, flag, head, fd);
        if (status == EXIT_FAILURE) return EXIT_FAILURE;
    }

    // Read file names to stdout
    if (flag->OPT_t == 1) {
        read_file_names(arg->name);
    } 

    // Deallocate memory
    if (head != NULL) {
        free_tar_linklist(head);
    }
    custom_exit(flag, arg);

    return EXIT_SUCCESS;
}

int initiate_archive_fd(arg_t *arg, flag_t *flag, int fd) 
{
    if (flag->OPT_c == 1) {
        fd = create_archive(arg->name);
        if (fd == -1) return EXIT_FAILURE;
    }
    if (flag->OPT_r == 1 || flag->OPT_u == 1) {
        fd = open_append_archive(arg->name);
        if (fd == -1) return EXIT_FAILURE;
    } 
   
    return EXIT_SUCCESS;
}

int write_new_or_add_to_archive(arg_t *arg, flag_t *flag, tar_node_t* head, int fd) 
{
    // Write or add to archive
    if (flag->OPT_r == 1 || flag->OPT_c == 1 || flag->OPT_u == 1) {
        int i = 0;
        int type = 0;

        // Check file type and write
        while (i < arg->size) {
            if (flag->OPT_u == 1) // Skip file if an earlier version already exists in archive (-u)
            {
                if (compare_file_names(head, arg->files[i])) // Check file type and add file (-u)
                {
                    type = check_file_type(arg->files[i]);
                    if (type == -1) {
                        custom_exit(flag, arg);
                        return EXIT_FAILURE;
                    }
                    if (type == 5) add_directory(fd, arg->files[i]);
                    else add_file(fd, arg->files[i], type);
                }
            }
            else  // Check file type then add file (-cr)
            {
                type = check_file_type(arg->files[i]);
                if (type == -1) {
                    custom_exit(flag, arg);
                    return EXIT_FAILURE;
                }
                if (type == 5) add_directory(fd, arg->files[i]);
                else add_file(fd, arg->files[i], type);
            }
            i += 1;
        }

        // Writes 2 zero byte 512 size blocks
        write_end_of_file_entry(fd);

        // Close tar file descriptor
        close(fd);
    }

    return EXIT_SUCCESS;
}
