#include "header.h"

int extract_archive(char *tar_filename, char *path)
{
    // Open fd to tar file
    int fd = open(tar_filename, O_RDONLY, 0644);
    if (!fd) {
        printf("Error opening tar file\n");
        return 1;
    }

    ssize_t bytes = 1;

    while (bytes > 0) 
    {
        // Read header 
        header_t* header = malloc(sizeof(header_t));
        bytes = read(fd, header, sizeof(header_t));

        // End of archive
        if (header->name[0] == '\0') {
            free(header);
            break;
        }

        // Convert size field to size_t
        size_t file_size = my_strtol(header->size, NULL, 8);

        /* deal with linkname + prefix ? */

        // Jump to end of block
        lseek(fd, 12, SEEK_CUR);

        // Create path for new file
        char* file_name = create_file_path(header->name, path);

        // Write file
        create_file(file_name, fd, file_size);
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

    return 0;
}

void set_permissions(char* file_name, char* header) 
{
    mode_t mode = my_strtol(header, NULL, 8);
    chmod(file_name, mode);
}

long my_strtol(char *str, char **endptr, int base) // FIX illegal functions
{
    // Skip leading whitespace 
    while (isspace(*str)) {
        str++;
    }

    // Conversion
    long result = 0;
    while (*str != '\0') {
        int digit;
        if (isdigit(*str)) {
            digit = *str - '0';
        } else if (isalpha(*str)) {
            digit = tolower(*str) - 'a' + 10;
        } else {
            break;  // Invalid character, stop conversion
        }

        if (digit >= base) {
            break;  // Invalid digit for the given base, stop conversion
        }

        result = result * base + digit;
        str++;
    }

    // Update endptr if provided
    if (endptr != NULL) {
        *endptr = (char *)str;
    }

    return result;
}

char* my_strcat(char *dst, char *src) { 
    // Save original destination pointer
    char *original_dst = dst; 
    
    // Move destination pointer to the end of the first string
    while (*dst != '\0') {
        dst += 1;
    }

    // Copy characters from src to dst
    while (*src != '\0') {
        *dst = *src;
        dst += 1;
        src += 1;
    }

    // Null-terminate 
    *dst = '\0';

    return original_dst;
}

void create_file(char* name, int tar_fd, size_t file_size) 
{
    // Create new file
    int file_fd = open(name, O_CREAT | O_WRONLY, 0644);

    // Create buf and read tar fd to buf
    char *buf = malloc(sizeof(char) * file_size + 1);
    if (!file_fd) {
        printf("Error creating file\n");
    }
    read(tar_fd, buf, file_size);

    // Null-terminate
    buf[file_size] = '\0';

    // Write buf to file
    write(file_fd, buf, file_size);

    // Free allocated memory and close fd to new file
    free(buf);
    close(file_fd);

}

char* create_file_path(char* header, char* path)
{
    // Remove original prefix
    char* name = remove_prefix(header);

    // Find length of file name and provided path
    int len = strlen(name);
    int len_path = strlen(path);

    // Create new variable, copy and cat 
    char* file_name = malloc(sizeof(char) * (len + len_path + 1));
    my_strcpy(file_name, path);
    my_strcat(file_name, name);

    return file_name;
}

char* remove_prefix(char* name) 
{
    // Find last '/' in string
    char* last_slash = strrchr(name, '/'); // replace strrchr

    // Return pointer to position following last slash
    if (last_slash) {
        return last_slash + 1;
    }
    else {
        return name;
    }
}

