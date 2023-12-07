#include "header.h"

/* functions in helper.c */
unsigned int calculate_string_ascii_sum(char *header, int size);
char* my_strncpy(char* dst, char* src, int index);
char* my_strcpy(char* dst, char* src);
int my_strcmp(char* str_1, char* str_2);
int my_strlen(char* str);
char* my_strcat(char *dest, char *src);
long my_strtol(char *str, char **endptr, int base);
int my_is_space(int c);
int my_is_digit(int c);
int my_is_alpha(int c);
int my_to_downcase(int c);
int check_file_type(char* arg);
void error_message(char* name);
