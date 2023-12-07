#include "header.h"

/* functions in extract_archive.c */
int extract_archive(char* tar_filename);
int is_directory(char* name, char* path);
char* isolate_directory(char* name);
int lst_slash(char* str_1);
char* my_cpy_prefix(char* dst, char* src, int index);
char* create_file_path(char* name, char* path);
int create_file(char* name, int fd, size_t blocks);
void set_permissions(char* file_name, char* header);