#include "header.h"


int read_file_names(char *tar_filename)
{
    // Open tar file
    int fd = open(tar_filename, O_RDONLY, 0644);
    if (fd == -1) {
        char* error = "Error opening tar file\n";
        write(2, error, 30);
        return 1;
    }

    ssize_t bytes = 1;

    while (bytes > 0) {
         // Create header_t and read data to header from fd
        header_t* header = malloc(sizeof(header_t));
        bytes = read(fd, header, sizeof(header_t));

        // If end of archive, free header struct
        if (header->name[0] == '\0') {
            free(header);
            break;
        }

        // Convert the size field to an integer
        size_t file_size = my_strtol(header->size, NULL, 8); 

        // Print the file name
        print_file_name(header->name);

        // Move file pointer to the beginning of next block
        size_t remaining_blocks = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        lseek(fd, remaining_blocks * BLOCK_SIZE + 12, SEEK_CUR);
        free(header);
    }

    close(fd);

    return 0;
}


int open_append_archive(char *name)
{
    struct stat sb;

    if (lstat(name, &sb) == -1) {
        error_message(name);
        return -1;
    }

    // Find size of tar file minus ending blocks
    off_t offset = (off_t)sb.st_size - 1024;

    int fd = open(name, O_RDWR, 0644);
    if (fd == -1) {
        exit(EXIT_FAILURE);
    }

    // Jump to point in tar fd before the 2 ending blocks
    lseek(fd, offset, SEEK_CUR);

    return fd;
}


void create_tar_node_list(tar_node_t** head, char *name)
{
    tar_node_t* new_node;

    int fd = open(name, O_RDWR, 0644);
    if (fd == -1) {
        exit(EXIT_FAILURE);
    }

    ssize_t bytes = 1;
    ssize_t* byte_ptr = &bytes;

    while (bytes > 0) {
        new_node = create_tar_node(byte_ptr, fd);
        if (bytes == 0) {
            break;
        }
        if (new_node != NULL) {
            add_tar_node_to_head(head, new_node);
        }
    }

    close(fd);
}


tar_node_t* create_tar_node(ssize_t* bytes, int fd)
{
    // Create header_t and read data to header from fd
    header_t* header = malloc(sizeof(header_t));
    *bytes = read(fd, header, sizeof(header_t));

    // If end of archive, free header struct
    if (header->name[0] == '\0') {
        free(header);
        *bytes = 0;
        return NULL;
    }

    // Convert the size field to an integer
    size_t file_size = my_strtol(header->size, NULL, 8); 

    // Move file pointer to the beginning of next block
    size_t remaining_blocks = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    lseek(fd, remaining_blocks * BLOCK_SIZE + 12, SEEK_CUR);

    // Allocate memory for new node
    tar_node_t* new_node = malloc(sizeof(tar_node_t));
    int len = my_strlen(header->name);
    new_node->name = malloc(sizeof(char) * len + 1);
    my_strcpy(new_node->name, header->name);

    // Convert mtime field to integer
    ssize_t file_mtime = my_strtol(header->mtime, NULL, 8);
    new_node->mtime = file_mtime;

    // Save tail of linklist
    new_node->next = NULL;

    free(header);

    return new_node;
}


void add_tar_node_to_head(tar_node_t** head, tar_node_t* new_node) // double check that if is needed
{
    if (head == NULL) {
        *head = new_node;
    } else {
        new_node->next = *head;
        *head = new_node;
    }
}


int compare_file_names(tar_node_t* head, char* file_name) 
{
    tar_node_t* current = head;
    ssize_t new_file_mtime;

    // Iterate thru linklist
    while (current != NULL) {
        // If file_name == name in linklist, check mtime
        int not_equal = my_strcmp(current->name, file_name);

        if (not_equal == 0) {
            new_file_mtime = check_file_mtime(file_name);
            // If the new file was modified more recently, return TRUE
            if (new_file_mtime > current->mtime) {
                return 1;
            } else {
                // If the new file was modified less recently or is the same, return FALSE
                return 0;
            }
        }
        current = current->next;
    }

    // Return TRUE if file_name was not found in linklist
    return 1;
}


ssize_t check_file_mtime(char* file_to_update) 
{
    struct stat sb;

    if (lstat(file_to_update, &sb) == -1) {
        error_message(file_to_update);
        return -1;
    }

    ssize_t mtime = sb.st_mtimespec.tv_sec;

    return mtime;
}


int my_strcmp(char* str_1, char* str_2) 
{
    int i;
    int str_to_int_1 = 0;
    int str_to_int_2 = 0;

    for (i = 0; str_1[i] != '\0'; i++) {
        str_to_int_1 += str_1[i];
    } 

    for (i = 0; str_2[i] != '\0'; i++) {
        str_to_int_2 += str_2[i];
    } 
    
    if (str_to_int_1 == str_to_int_2) {
        return 0;
    } else if (str_to_int_1 > str_to_int_2) {
        return -1;
    } else {
        return 1;
    }
}


void print_file_name(char* name) 
{
    int len = my_strlen(name);
    char* name_to_output = malloc(sizeof(char) * len + 1);
    my_strcpy(name_to_output, name);
    name_to_output[len] = '\n';
    name_to_output[len + 1] = '\0';
    write(1, name_to_output, len + 1);
    free(name_to_output);
}
