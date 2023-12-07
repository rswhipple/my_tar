#include "header.h"

/* functions in read_and_update_archive.c */
int read_file_names(char *tar_filename);
int open_append_archive(char *name);
void create_tar_node_list(tar_node_t** head, char *name);
tar_node_t* create_tar_node(ssize_t* bytes, int fd);
void add_tar_node_to_head(tar_node_t** head, tar_node_t* new_node);
ssize_t check_file_mtime(char* file_to_update);
int compare_file_names(tar_node_t* head, char* file_name);
void print_file_name(char* name);