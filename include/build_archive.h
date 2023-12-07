#include "header.h"

/* functions in build_archive.c */
int create_archive(char *name);
void add_file(int fd, char *file_name, int type);
void add_directory(int fd, char *file_name);
int write_header(int fd, char *file_name, int type);
int write_file(int fd, char *file_name);
int check_file_size(char* arg);
void write_end_of_file_entry(int fd);
char* format_directory_name(char *file_name);
char* format_file_name_in_directory(char* name, char* path);