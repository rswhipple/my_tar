#include "header.h"

int read_file_names(char *tar_filename)
{
    int fd = open(tar_filename, O_RDONLY, 0644);
    if (!fd) {
        printf("Error opening tar file\n");
        return 1;
    }

    ssize_t bytes = 1;

    while (bytes > 0) {
        header_t* header = malloc(sizeof(header_t));
        bytes = read(fd, header, sizeof(header_t));

        // End of archive
        if (header->name[0] == '\0') {
            free(header);
            break;
        }

        // Convert the size field to an integer
        size_t file_size = my_strtol(header->size, NULL, 8); 

        // Print the file name
        printf("%s\n", header->name);

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
        printf("my_tar: cannot access '%s': No such file or directory\n", name);
        return -1;
    }

    off_t offset = (off_t)sb.st_size - 1024;

    int fd = open(name, O_RDWR, 0644);
    if (!fd) {
        exit(EXIT_FAILURE);
    }

    lseek(fd, offset, SEEK_CUR);

    return fd;
}

tar_node_t* create_tar_node_list(char *name) 
{
    int fd = open(name, O_RDWR, 0644);
    if (!fd) {
        exit(EXIT_FAILURE);
    }

    tar_node_t* head = NULL;
    ssize_t bytes = 1;

    while (bytes > 0) {
        tar_node_t* new_node = malloc(sizeof(tar_node_t) + 1);
        bytes = create_tar_node(new_node, fd);
        add_tar_node_to_head(&head, new_node);
    }

    return head;
}

int create_tar_node(tar_node_t* new_node, int fd)
{
    ssize_t bytes = 1;

    // Create header_t and read data from fd
    header_t* header = malloc(sizeof(header_t));
    bytes = read(fd, header, sizeof(header_t));

    // End of archive
    if (header->name[0] == '\0') {
        free(header);
        return 0;
    }

    // Convert the size field to an integer
    size_t file_size = my_strtol(header->size, NULL, 8); 

    // Move file pointer to the beginning of next block
    size_t remaining_blocks = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    lseek(fd, remaining_blocks * BLOCK_SIZE + 12, SEEK_CUR);

    // Allocate memory for name field
    int len = strlen(header->name);
    new_node->name = malloc(sizeof(char) * len + 1);
    my_strcpy(new_node->name, header->name);

    // Convert mtime field to integer
    size_t file_mtime = my_strtol(header->mtime, NULL, 8);
    new_node->mtime = file_mtime;

    // Save tail of linklist
    new_node->next = NULL;

    free(header);

    return bytes;
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
    unsigned int new_file_mtime;

    // Iterate thru linklist
    while (current != NULL) {
        // If file_name == name in linklist, check mtime
        if (current->name == file_name) {
            new_file_mtime = check_file_mtime(file_name);
            // If the new file was modified more recently, return TRUE
            if (new_file_mtime > current->mtime) {
                return 1;
            }
        }
        current = current->next;
    }

    // Return FALSE
    return 0;
}


unsigned int check_file_mtime(char* file_to_update) 
{
    struct stat sb;

    if (lstat(file_to_update, &sb) == -1) {
        printf("my_tar: cannot access '%s': No such file or directory\n", file_to_update);
        return -1;
    }

    unsigned int mtime = sb.st_mtimespec.tv_sec;
    // size_t file_mtime = my_strtol(header->mtime, NULL, 8);

    return mtime;
}
