#include "header.h"


int extract_archive(char *tar_filename)
{
    // Open fd of tar file
    int fd = open(tar_filename, O_RDONLY, 0644);
    if (fd == -1) {
        char* error = "Error opening tar file\n";
        write(2, error, 30);
        return EXIT_FAILURE;
    }

    char* path = "./";
    ssize_t bytes = 1;

    while (bytes > 0) 
    {
        // Read header 
        header_t* header = malloc(sizeof(header_t));
        bytes = read(fd, header, sizeof(header_t));

        // If end of archive is reached, free header and break
        if (header->name[0] == '\0') {
            free(header);
            break;
        }

        // Convert size field to size_t
        size_t file_size = my_strtol(header->size, NULL, 8);

        // Jump to end of block
        lseek(fd, 12, SEEK_CUR);

        // Check if directory exists
        is_directory(header->name, path);

        // Create path for new file
        char* file_name = create_file_path(header->name, path);

        // Write file
        if(create_file(file_name, fd, file_size) != 0) {
            return EXIT_FAILURE;
        }
        set_permissions(file_name, header->mode);

        // Calculate remaining bytes in current block + jump to end
        size_t remaining_bytes = BLOCK_SIZE - (file_size % BLOCK_SIZE);
        lseek(fd, remaining_bytes, SEEK_CUR);

        // Free all allocated memory
        free(header);
        free(file_name);
    }

    // Close fd to tar file
    close(fd);

    return EXIT_SUCCESS;
}


int is_directory(char* name, char* path) 
{
    // if there is a directory, remove file_name
    char* directory_name = isolate_directory(name);

    // check to see if directory exists
    if (directory_name != NULL) {
        char* directory_path = create_file_path(directory_name, path);
        free(directory_name);

        // if no !directory, make directory
        if (check_file_type(directory_path) != 5) {
            int status = mkdir(directory_path, 0777);
            if (status != 0) {
                char* error = "Error creating directory\n";
                write(2, error, 30);
                return EXIT_FAILURE;
            }
        } 
        free(directory_path);
    }

    return EXIT_SUCCESS;
}


char* isolate_directory(char* name) 
{
    // Find last '/' in string
    int index = lst_slash(name); 

    // Create path for directory
    if (index > 0) {
        char* directory = malloc(sizeof(char) * index + 1);
        my_cpy_prefix(directory, name, index);
        return directory;
    }
    else {
        return NULL;
    }
}


int lst_slash(char* str_1)
{
    char char_1 = '/';
    int len = my_strlen(str_1) - 1;
    int i;

    for (i = len; i > 0; i--) {
        if (str_1[i] == char_1) {
            return i;
        }
    }

    return 0;
}


char* my_cpy_prefix(char* dst, char* src, int index)
{
    int i;

    for (i = 0; i < index; i++) {
        dst[i] = src[i];
    }

    dst[i] = '\0';

    return dst;
}


void set_permissions(char* file_name, char* header) 
{
    mode_t mode = my_strtol(header, NULL, 8);
    chmod(file_name, mode);
}


int create_file(char* name, int tar_fd, size_t file_size) 
{

    // Create new file
    int file_fd = open(name, O_CREAT | O_RDWR, 0755);

    // Create buf and read tar fd to buf
    char *buf = malloc(sizeof(char) * file_size + 1);
    if (file_fd == -1) {
        char* error = "Error creating file\n";
        write(2, error, 30);
        return EXIT_FAILURE;
    }
    read(tar_fd, buf, file_size);

    // Null-terminate
    buf[file_size] = '\0';

    // Write buf to file
    write(file_fd, buf, file_size);

    // Free allocated memory and close fd to new file
    free(buf);
    close(file_fd);

    return EXIT_SUCCESS;
}


char* create_file_path(char* header, char* path)
{
    // Find length of file name and provided path
    int len = my_strlen(header);
    int len_path = my_strlen(path);

    // Create new variable, copy and cat 
    char* file_name = malloc(sizeof(char) * (len + len_path + 1));
    my_strcpy(file_name, path);
    my_strcat(file_name, header);

    return file_name;
}
