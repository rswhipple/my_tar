#include "header.h"

int create_archive(char *name) 
{
    // Create archive
    int fd = open(name, O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        exit(EXIT_FAILURE);
    }
    
    // Return file descriptor
    return fd;
}

void add_file(int fd, char *file_name, int type) 
{
    // Header
    write_header(fd, file_name, type);

    // File content
    // If file is a directory or symlink, program will not write file content
    if (type != 5 && type != 1 && type != 2) { 
        write_file(fd, file_name);
    } 
}

void add_directory(int fd, char *file_name)
{
    // Write directory header
    char* dir_name = format_directory_name(file_name);
    write_header(fd, dir_name, 5);

    int type;

    // Open directory
    DIR *dir;
    struct dirent *entry;
    dir = opendir(dir_name);

    // Write each file in directory
    while ((entry = readdir(dir)) != NULL) {
        // Remove '.' files
        if (entry->d_name[0] != '.') 
        {
            // Check type + format name
            char* file_name = format_file_name_in_directory(entry->d_name, dir_name);
            type = check_file_type(file_name);

            // Write tar entry
            add_file(fd, file_name, type);
            
            free(file_name);
        }
    }

    closedir(dir);
    free(dir_name);
}

void write_header(int fd, char *file_name, int type) 
{
    // Header
    header_t* header = malloc(sizeof(header_t) * 1);
    create_header(header, file_name, type);
    ssize_t bytes_written = write(fd, header, sizeof(header_t));

    // ERROR + EXIT
    if (bytes_written != 500) {  
        exit(EXIT_FAILURE);
    }

    // 12 byte offset
    char offset[12] = {0};
    bytes_written = write(fd, offset, 12);

    // ERROR + EXIT
    if (bytes_written != 12) {  
        exit(EXIT_FAILURE);
    }

    free(header);
}


void write_file(int fd, char *file_name) 
{
    // Open file
    int file_fd = open(file_name, O_RDONLY);
    if (file_fd == -1) {
        printf("Error opening file\n");
        return;
    }

    // Initiate remaining_bytes equal to the file's total size
    int remaining_bytes = check_file_size(file_name);

    char buffer[BLOCK_SIZE];
    ssize_t bytes_read;
    ssize_t bytes_written;

    // Read bytes into buffer, write into tar and keep track of remaining_bytes
    while (remaining_bytes >= BLOCK_SIZE) {
        bytes_read = read(file_fd, buffer, BLOCK_SIZE);
        bytes_written = write(fd, buffer, BLOCK_SIZE);
        remaining_bytes -= bytes_read;
    }

    // When remaining_bytes are less than BLOCK_SIZE, pad buffer with NULL
    if (remaining_bytes < BLOCK_SIZE) {
        bytes_read = read(file_fd, buffer, remaining_bytes);
        int null_padding = BLOCK_SIZE - bytes_read - 1;
        memset(buffer + remaining_bytes, 0, null_padding);

        bytes_written = write(fd, buffer, BLOCK_SIZE);
        if (bytes_written == -1) {
            printf("Error writing to tar file\n");
            close(file_fd);
            return;
        }
    }

    close(file_fd);
}


void write_end_of_file_entry(int fd)
{
    char zero_block[BLOCK_SIZE] = {0};
    write(fd, zero_block, BLOCK_SIZE);
    write(fd, zero_block, BLOCK_SIZE);
}

int check_file_size(char* arg) 
{
    // Create struct to store file info
    struct stat sb;

    if (lstat(arg, &sb) == -1) {
        printf("my_tar: cannot access '%s': No such file or directory\n", arg);
        return -1;
    }

    int size = sb.st_size;

    return size;
}

char* format_directory_name(char *file_name) 
{
    int str_len = strlen(file_name);
    char * buf = NULL;

    // If char* does not end with '/', realloc and add '/'
    if (file_name[str_len - 1] != '/') {
        buf = malloc(sizeof(char) * str_len + 2);
        buf = my_strcpy(buf, file_name);
        buf[str_len] = '/';
        buf[str_len + 1] = '\0';
    } else {
        buf = malloc(sizeof(char) * str_len + 1);
        buf = my_strcpy(buf, file_name);
    }
    return buf;
}

char* format_file_name_in_directory(char* name, char* path)
{
    // Find length of file name and path
    int len = strlen(name);
    int len_path = strlen(path);

    // Create new variable, copy and cat 
    char* file_name = malloc(sizeof(char) * (len + len_path + 1));
    my_strcpy(file_name, path);
    my_strcat(file_name, name);

    return file_name;
}
