#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>


#ifndef STRUCT_MY_TAR_HEADER
#define STRUCT_MY_TAR_HEADER
struct my_tar_header
{                              /* byte offset */
  char name[100];               /*   0 DONE */   
  char mode[8];                 /* 100 DONE */ 
  char uid[8];                  /* 108 DONE */
  char gid[8];                  /* 116 DONE */
  char size[12];                /* 124 DONE */
  char mtime[12];               /* 136 DONE ??? */
  char chksum[8];               /* 148 DONE */
  char typeflag;                /* 156 DONE */
  char linkname[100];           /* 157 DONE */
  char magic[6];                /* 257 DONE */
  char version[2];              /* 263 DONE ??? */
  char uname[32];               /* 265 DONE */
  char gname[32];               /* 297 DONE */
  char devmajor[8];             /* 329 DONE */
  char devminor[8];             /* 337 DONE */
  char prefix[155];             /* 345 DONE */
                                /* 500 */
}; typedef struct my_tar_header header_t;

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

#define XHDTYPE  'x'            /* Extended header referring to the
                                   next file in the archive */
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

#define _BSD_SOURCE             /* See feature_test_macros(7) */
#include <sys/types.h>

#define BLOCK_SIZE 512

/* function prototypes */
void decimal_to_octal(unsigned int num, char* result, int len);
int zero_padded_char(char* dst, int dst_len, char* src);
void check_name_length(char* dst_1, char* dst_2, char* src);
void find_linkname(char* file_name, header_t *header);
unsigned int calculate_checksum(header_t *header);
unsigned int calculate_string_ascii_sum(char *header);
char* my_strncpy(char* dst, char* src, int index);
char* my_strcpy(char* dst, char* src);
// int check_file_type(char* arg);

void create_header(header_t *header, char *file_name, int type) 
{
    struct stat sb;

    if (lstat(file_name, &sb) == -1) {
        printf("my_tar: cannot access '%s': No such file or directory\n", file_name);
    }
    
    check_name_length(header->name, header->prefix, file_name); /* name & prefix */  
    printf("header->name = |%s|\n", header->name); printf("header->prefix = |%s|\n", header->prefix);
   

    decimal_to_octal(sb.st_mode, header->mode, 8);              /* mode */

    decimal_to_octal(sb.st_uid, header->uid, 8);                /* uid */

    decimal_to_octal(sb.st_gid, header->gid, 8);                /* gid */

    decimal_to_octal(sb.st_size, header->size, 12);             /* size */
    
    time_t mod_time = sb.st_mtimespec.tv_sec;                   /* mtime */ 
    decimal_to_octal(mod_time, header->mtime, 12);
    printf("mod_time (octal) = |%s|\n", header->mtime);


    header->typeflag = type;                                    /* typeflag */

    if (type == 2 || type == 1) {                               /* linkname */
        find_linkname(file_name, header);
    } else {
        memset(header->linkname, 0, 100);
    }
    
    for (int i = 0; i < 6; i++) header->magic[i] = TMAGIC[i];   /* magic */

    memset(header->version, 0, 2);                              /* version */  // need to talk to someone about this one

    int devmajor = major(sb.st_dev);
    decimal_to_octal(devmajor, header->devmajor, 8);            /* devmajor */

    int devminor = minor(sb.st_dev);
    decimal_to_octal(devminor, header->devminor, 8);            /* devminor */

    struct passwd *user_info = getpwuid(sb.st_uid);
    zero_padded_char(header->uname, 32, user_info->pw_name);    /* uname */

    struct group *group_info = getgrgid(sb.st_gid);
    zero_padded_char(header->gname, 32, group_info->gr_name);   /* gname */

    memset(header->chksum, ' ', 8);                             /* chksum */
    calculate_checksum(header);

}

void decimal_to_octal(unsigned int num, char* dst, int len) 
{
    int i = 0;
    do {
        dst[i] = '0' + (num % 8);
        num /= 8;
        i += 1;
    } while (num != 0);

    while (i < len) {
        dst[i] = '\0';
        i += 1;
    }
}

int zero_padded_char(char* dst, int dst_len, char* src) 
{
    int length = strlen(src);
    int i = 0;

    while (i < length) {
        dst[i] = src[i];
        i += 1;
    }

    while (i < dst_len) {
        dst[i++] = '\0';
    }

    return length;
}


void check_name_length(char* dst_1, char* dst_2, char* src) {
    char *buf;
    int src_len = strlen(src);
    if (src_len > 100) {
        char *lastSlash = strrchr(src, '/');
        if (lastSlash) {
            buf = malloc(sizeof(char) * src_len);
            my_strcpy(buf, lastSlash + 1);
            printf ("buf = |%s|\n", buf);   // testing
            zero_padded_char(dst_1, 100, buf);
            lastSlash[1] = '\0';    // end first string
            zero_padded_char(dst_2, 155, src);
        } 
        /* this part needs reworking */
        // else {
    //         int index = zero_padded_char(dst_2, 155, src);
    //         if (index > 0) {
    //             zero_padded_char(dst_1, 100, src);
    //         } else {
    //             memset(dst_1, 0, 100);
    //         }
    //     }
    } 
    else {
        printf("writing file_name\n");  // testing
        memset(dst_2, 0, 155);
        zero_padded_char(dst_1, 100, src);
    }
}


void find_linkname(char* file_name, header_t *header) {                             
    char *buffer = NULL;
    ssize_t bytes;
    ssize_t buf_size = 100;
    buffer = (char*)malloc(sizeof(char) * 100);

    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    bytes = readlink(file_name, buffer, buf_size);

    if (bytes == -1) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }

    buffer[bytes] = '\0';

    if (bytes == buf_size) {
        printf("(Linkname may have been truncated)\n");
    }

    zero_padded_char(header->linkname, 100, buffer); 
}


unsigned int calculate_checksum(header_t *header) 
{
    unsigned int sum = 0;
    sum += calculate_string_ascii_sum(header->name);
    sum += calculate_string_ascii_sum(header->mode);
    sum += calculate_string_ascii_sum(header->uid);
    sum += calculate_string_ascii_sum(header->gid);
    sum += calculate_string_ascii_sum(header->size);
    sum += calculate_string_ascii_sum(header->mtime);
    sum += calculate_string_ascii_sum(header->chksum);
    sum += header->typeflag;
    sum += calculate_string_ascii_sum(header->linkname);
    sum += calculate_string_ascii_sum(header->magic);
    sum += calculate_string_ascii_sum(header->version);
    sum += calculate_string_ascii_sum(header->uname);
    sum += calculate_string_ascii_sum(header->gname);
    sum += calculate_string_ascii_sum(header->devmajor);
    sum += calculate_string_ascii_sum(header->devminor);
    sum += calculate_string_ascii_sum(header->prefix);

    printf("chksum = %i\n", sum);

    return sum;
}

unsigned int calculate_string_ascii_sum(char *header)
{
    unsigned int sum = 0;
    int length = strlen(header);
    int i = 0;
    while (i < length) {
        sum += (unsigned char)header[i];
        i += 1;
    }
    
    return sum;
}

char* my_strcpy(char* dst, char* src)
{
    int i;
    int len = strlen(src);

    for (i = 0; i < len; i++) {
        dst[i] = src[i]; 
    }

    return dst;
}

char* my_strncpy(char* dst, char* src, int index)
{
    int i;
    int length = strlen(src);

    for (i = 0; index < length; i++) {
        dst[i] = src[index];
        index += 1; 
    }

    dst[i] = '\0';

    return dst;
}

int main() 
{
    header_t test[BLOCK_SIZE];
    // char *file = "symlink_file1.txt";
    char *file = "file1.txt";
    int type = 0;

    // type = check_file_type(file);

    printf("type = %i\n", type);

    create_header(test, file, type);

    return 0;
}

// void create_header(header_t *header, char *file_name, int type) 
// {
//     struct stat sb;

//     if (lstat(file_name, &sb) == -1) {
//         printf("my_tar: cannot access '%s': No such file or directory\n", file_name);
//     }

    
//     check_name_length(header->name, header->prefix, file_name); /* name & prefix */     
//     printf("header->name = |%s|\n", header->name); printf("header->prefix = |%s|\n", header->prefix);

//     decimal_to_octal(sb.st_mode, header->mode, 8);              /* mode */

//     decimal_to_octal(sb.st_uid, header->uid, 8);                /* uid */

//     decimal_to_octal(sb.st_gid, header->gid, 8);                /* gid */

//     decimal_to_octal(sb.st_size, header->size, 12);             /* size */
    
//     time_t mod_time = sb.st_mtimespec.tv_sec;                   /* mtime */  // not sure if this is working yet
//     decimal_to_octal(mod_time, header->mtime, 12);
//     // printf("mod_time = %li\n", mod_time);
//     printf("mod_time (octal) = |%s|\n", header->mtime);

//     header->typeflag = type;                                    /* typeflag */

//     if (type == 2 || type == 1) {                               /* linkname */
//         find_linkname(file_name, header);
//     } else {
//         memset(header->linkname, 0, 100);
//     }
//     // printf("header->linkname = %s\n", header->linkname);
    
//     for (int i = 0; i < 6; i++) header->magic[i] = TMAGIC[i];   /* magic */

//     memset(header->version, 0, 2);                              /* version */  // need to talk to someone about this one

//     int devmajor = major(sb.st_dev);
//     decimal_to_octal(devmajor, header->devmajor, 8);            /* devmajor */

//     int devminor = minor(sb.st_dev);
//     decimal_to_octal(devminor, header->devminor, 8);            /* devminor */

//     struct passwd *user_info = getpwuid(sb.st_uid);
//     zero_padded_char(header->uname, 32, user_info->pw_name, 0);    /* uname */

//     struct group *group_info = getgrgid(sb.st_gid);
//     zero_padded_char(header->gname, 32, group_info->gr_name, 0);   /* gname */

//     memset(header->chksum, ' ', 8);                             /* chksum */
//     calculate_checksum(header);

//     // The chksum field represents the simple sum of all bytes in the header block. 
//     // Each 8-bit byte in the header is added to an unsigned integer, initialized to zero, 
//     // the precision of which shall be no less than seventeen bits. 
//     // When calculating the checksum, the chksum field is treated as if it were all blanks.
// }

// void decimal_to_octal(unsigned int num, char* dst, int len) 
// {
//     int length = len - 1;
//     dst[len] = '\0';
//     do {
//         dst[length--] = '0' + (num % 8);
//         num /= 8;
//     } while (num != 0);

//     while (length >= 0) {
//         dst[length--] = '\0';
//     }
// }

// int zero_padded_char(char* dst, int len, char* src, int index) 
// {
//     dst[len] = '\0';
//     int length = strlen(src) - 1;
//     int i = len - 1;

//     while (length >= index) {
//         dst[i--] = src[length--];
//     }

//     while (i >= 0) {
//         dst[i--] = '\0';
//     }

//     return length;
// }

// void check_name_length(char* dst_1, char* dst_2, char* src) {
//     if (strlen(src) > sizeof(dst_1)) {
//         char *lastSlash = strrchr(src, '/');
//         if (lastSlash) {
//             int prefix_length = lastSlash - src + 1;
//             int index = zero_padded_char(dst_1, 100, src, prefix_length + 1);
//             zero_padded_char(dst_2, 155, src, index);
//         } else {
//             int index = zero_padded_char(dst_2, 155, src, 0);
//             if (index > 0) {
//                 zero_padded_char(dst_1, 100, src, index);
//             } else {
//                 memset(dst_1, 0, 100);
//             }
            
//         }
//     } else {
//         memset(dst_2, 0, 155);
//         zero_padded_char(dst_1, 100, src, 0);
//     }
// }

// void find_linkname(char* file_name, header_t *header) {                             
//     char *buffer = NULL;
//     ssize_t bytes;
//     ssize_t buf_size = 100;
//     buffer = (char*)malloc(sizeof(char) * 100);

//     if (buffer == NULL) {
//         perror("malloc");
//         exit(EXIT_FAILURE);
//     }

//     bytes = readlink(file_name, buffer, buf_size);

//     if (bytes == -1) {
//         perror("readlink");
//         exit(EXIT_FAILURE);
//     }

//     buffer[bytes] = '\0';

//     if (bytes == buf_size) {
//         printf("(Linkname may have been truncated)\n");
//     }

//     zero_padded_char(header->linkname, 100, buffer, 0); 
// }

// unsigned int calculate_checksum(header_t *header) 
// {
//     unsigned int sum = 0;
//     sum += calculate_string_ascii_sum(header->name);
//     sum += calculate_string_ascii_sum(header->mode);
//     sum += calculate_string_ascii_sum(header->uid);
//     sum += calculate_string_ascii_sum(header->gid);
//     sum += calculate_string_ascii_sum(header->size);
//     sum += calculate_string_ascii_sum(header->mtime);
//     sum += calculate_string_ascii_sum(header->chksum);
//     sum += header->typeflag;
//     sum += calculate_string_ascii_sum(header->linkname);
//     sum += calculate_string_ascii_sum(header->magic);
//     sum += calculate_string_ascii_sum(header->version);
//     sum += calculate_string_ascii_sum(header->uname);
//     sum += calculate_string_ascii_sum(header->gname);
//     sum += calculate_string_ascii_sum(header->devmajor);
//     sum += calculate_string_ascii_sum(header->devminor);
//     sum += calculate_string_ascii_sum(header->prefix);

//     printf("chksum = %i\n", sum);

//     return sum;
// }

// unsigned int calculate_string_ascii_sum(char *header)
// {
//     unsigned int sum = 0;
//     int length = strlen(header);
//     int i = 0;
//     while (i < length) {
//         sum += (unsigned char)header[i];
//         i += 1;
//     }
    
//     return sum;
// }

// int check_file_type(char* arg) 
// {
//     struct stat sb;

//     if (lstat(arg, &sb) == -1) {
//         printf("my_tar: cannot access '%s': No such file or directory\n", arg);
//         return -1;
//     }

//     switch (sb.st_mode & S_IFMT) {
//         case S_IFREG: break;        
//         case S_IFLNK: return 1;
//         case S_IFCHR: return 3;
//         case S_IFBLK: return 4;
//         case S_IFDIR: return 5;
//         case S_IFIFO: return 6;
//         case S_IFSOCK: break;
//         default: break;
//     }

//     return 0;
// }


