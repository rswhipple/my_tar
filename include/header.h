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
