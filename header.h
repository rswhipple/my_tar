#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>            
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

#define BLOCK_SIZE 512

#ifndef STRUCT_FLAG_S_
#define STRUCT_FLAG_S_
struct flag_s {
    int OPT_c;
    int OPT_r;
    int OPT_u;
    int OPT_t;
    int OPT_x;
    int OPT_f;
    int INDEX_f;
    int FILES;
}; typedef struct flag_s flag_t;
#endif

#ifndef STRUCT_ARG_S_
#define STRUCT_ARG_S_
struct arg_s {
    char *name;
    char **files;
    int size;
}; typedef struct arg_s arg_t;
#endif

#ifndef STRUCT_MY_TAR_HEADER
#define STRUCT_MY_TAR_HEADER
struct my_tar_header
{                              /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
                                /* 500 */

}; typedef struct my_tar_header header_t;
#endif

#ifndef STRUCT_TAR_NODE
#define STRUCT_TAR_NODE
struct s_tar_node {
    char* name;
    ssize_t mtime;
    struct s_tar_node* next;
}; typedef struct s_tar_node tar_node_t;
#endif

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2

/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */

#define XHDTYPE  'x'            /* Extended header referring to the next file in the archive */
                                   
#define XGLTYPE  'g'            /* Global extended header */

/* Bits used in the mode field, values in octal.  */
#define TSUID    04000          /* set UID on execution */
#define TSGID    02000          /* set GID on execution */
#define TSVTX    01000          /* reserved */
                                /* file permissions */
#define TUREAD   00400          /* read by owner */
#define TUWRITE  00200          /* write by owner */
#define TUEXEC   00100          /* execute/search by owner */
#define TGREAD   00040          /* read by group */
#define TGWRITE  00020          /* write by group */
#define TGEXEC   00010          /* execute/search by group */
#define TOREAD   00004          /* read by other */
#define TOWRITE  00002          /* write by other */
#define TOEXEC   00001          /* execute/search by other */

/* functions in my_tar.c */
int main(int argc, char *argv[]);
int write_new_or_add_to_archive(arg_t *arg, flag_t *flag, tar_node_t* head, int* fd);
int initiate_archive_fd(arg_t *arg, flag_t *flag, int* fd);

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
int my_strlen(char* str);

/* functions in deallocate_memory.c */
void custom_exit(flag_t *flag, arg_t *arg);
void free_array(char **array, int size);
void free_tar_linklist(tar_node_t* head);

/* functions in extract_archive.c */
int extract_archive(char* tar_filename);
int is_directory(char* name, char* path);
char* isolate_directory(char* name);
int lst_slash(char* str_1);
char* my_cpy_prefix(char* dst, char* src, int index);
char* create_file_path(char* name, char* path);
int create_file(char* name, int fd, size_t blocks);
void set_permissions(char* file_name, char* header);
char* my_strcat(char *dest, char *src);
long my_strtol(char *str, char **endptr, int base);
int my_is_space(int c);
int my_is_digit(int c);
int my_is_alpha(int c);
int my_to_downcase(int c);

/* functions in getopt.c */
int my_getopt(int argc, char *argv[], flag_t *flags);
int check_flags(int argc, char *const argv[], const char *optstring, flag_t *flags);
int getopt_errors(flag_t* flags);

/* functions in parser.c */
int parse_args(int argc, char *argv[], flag_t *flags, arg_t *arg);
int check_file_type(char* arg);
void error_message(char* name);

/* functions in read_and_update_archive.c */
int read_file_names(char *tar_filename);
int open_append_archive(char *name);
void create_tar_node_list(tar_node_t** head, char *name);
tar_node_t* create_tar_node(ssize_t* bytes, int fd);
void add_tar_node_to_head(tar_node_t** head, tar_node_t* new_node);
ssize_t check_file_mtime(char* file_to_update);
int compare_file_names(tar_node_t* head, char* file_name);
int my_strcmp(char* str_1, char* str_2);
void print_file_name(char* name);

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
unsigned int calculate_string_ascii_sum(char *header, int size);
char* my_strncpy(char* dst, char* src, int index);
char* my_strcpy(char* dst, char* src);
char set_typeflag(int type);


/*
my_tar is a command to manipulate tape archive. The first option to tar is a mode indicator from the following list:

-c Create a new archive containing the specified items.
-r Like -c, but new entries are appended to the archive. The -f option is required.
-t List archive contents to stdout.
-u Like -r, but new entries are added only if they have a modification date newer than the corresponding entry in the archive. The -f option is required.
-x Extract to disk from the archive. If a file with the same name appears more than once in the archive, each copy will be extracted, with later copies overwriting (replacing) earlier copies.
In -c, -r, or -u mode, each specified file or directory is added to the archive in the order specified on the command line. By default, the contents of each directory are also archived.

Unless specifically stated otherwise, options are applicable in all operating modes:

-f file Read the archive from or write the archive to the specified file. The filename can be standard input or standard output.
my_tar will not handle file inside subdirectory.
*/

