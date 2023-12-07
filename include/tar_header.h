#include "header.h"

/* functions in tar_header.c */
void create_header(header_t *header, char *file_name, int type);
void write_space_before(unsigned int num, char* result, int len);
void write_regular_number(unsigned int num, char* dst, int len);
void write_chksum(unsigned int num, char* dst, int len);
void decimal_to_octal(unsigned int num, char* dst, int len);
int zero_padded_char(char* dst, int dst_len, char* src);
void check_name_length(char* dst_1, char* dst_2, char* src);
void find_linkname(char* file_name, char *header);
unsigned int calculate_checksum(header_t *header);
char set_typeflag(int type);