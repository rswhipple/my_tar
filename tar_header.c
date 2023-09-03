#include "header.h"

void create_header(header_t *header, char *file_name, int type) 
{
    struct stat sb;

    // Open lstat
    if (lstat(file_name, &sb) == -1) {
        printf("my_tar: cannot access '%s': No such file or directory\n", file_name);
    }
    
    // If name lenght > 100 split between name and prefix
    check_name_length(header->name, header->prefix, file_name);     /* name & prefix */  


    write_space_before(sb.st_mode, header->mode, 8);                /* mode */
    header->mode[0] = '0';  // fix mode discrepancy

    write_space_before(sb.st_uid, header->uid, 8);                  /* uid */

    write_space_before(sb.st_gid, header->gid, 8);                  /* gid */

    write_regular_number(sb.st_size, header->size, 12);             /* size */
    
    time_t mod_time = sb.st_mtimespec.tv_sec;                       /* mtime */ 
    write_regular_number(mod_time, header->mtime, 12);

    header->typeflag = set_typeflag(type);                          /* typeflag */

    if (type == 2 || type == 1) {                                   /* linkname */
        find_linkname(file_name, header->linkname);
    } else {
        memset(header->linkname, 0, 100);
    }
    
    for (int i = 0; i < 6; i++) header->magic[i] = TMAGIC[i];       /* magic */

    memset(header->version, '0', 2);                                /* version */ 

    int devmajor = major(sb.st_dev);
    // int devmajor = 0;   // work around
    write_space_before(devmajor, header->devmajor, 8);              /* devmajor */

    int devminor = minor(sb.st_dev);
    // int devminor = 0;   // work around
    write_space_before(devminor, header->devminor, 8);              /* devminor */

    struct passwd *user_info = getpwuid(sb.st_uid);
    zero_padded_char(header->uname, 32, user_info->pw_name);        /* uname */

    struct group *group_info = getgrgid(sb.st_gid);
    zero_padded_char(header->gname, 32, group_info->gr_name);       /* gname */

    memset(header->chksum, ' ', 8);                                 /* chksum */
    unsigned int chksum = calculate_checksum(header);
    write_chksum(chksum, header->chksum, 8);
}

char set_typeflag(int type)
{
    // Convert int to char
    switch(type) {
        case 0: return '0';            /* regular file */
        case 1: return '1';            /* link */
        case 2: return '2';            /* reserved */
        case 3: return '3';            /* character special */
        case 4: return '4';            /* block special */
        case 5: return '5';            /* directory */
        case 6: return '6';           /* FIFO special */
        case 7: return '7';     
        default: return '0';    
    }
}

void write_space_before(unsigned int num, char* dst, int len) 
{
    dst[len - 1] = '\0';
    dst[len - 2] = ' ';
    decimal_to_octal(num, dst, len - 2);
}

void write_regular_number(unsigned int num, char* dst, int len) 
{
    dst[len] = '\0';
    dst[len - 1] = ' ';
    decimal_to_octal(num, dst, len - 1);
}

void write_chksum(unsigned int num, char* dst, int len) 
{
    dst[len - 1] = ' ';
    dst[len - 2] = '\0';
    decimal_to_octal(num, dst, len - 2);
}


void decimal_to_octal(unsigned int num, char* dst, int len) 
{
    int i = len - 1;
    // Find octal + convert to char
    do {
        dst[i] = '0' + (num % 8);
        num /= 8;
        i -= 1;
    } while (num != 0);

    // If char octal is shorter than dst, pad with '0'
    while (i >= 0 ) {
        dst[i] = '0';
        i -= 1;
    }
}


int zero_padded_char(char* dst, int dst_len, char* src) 
{
    int length = strlen(src);
    int i = 0;

    // Copy src to dst
    while (i < length) {
        dst[i] = src[i];
        i += 1;
    }

    // If src is shorter than dst, pad with NULL
    while (i < dst_len) {
        dst[i++] = '\0';
    }

    // Return length of src
    return length;
}


void check_name_length(char* dst_1, char* dst_2, char* src) 
{
    char *buf;
    int src_len = strlen(src);

    /* If file name is longer than 100 */
    if (src_len > 100) 
    {
        char* last_slash = strrchr(src, '/'); // fix
        if (last_slash) {
            // Write name
            buf = malloc(sizeof(char) * src_len);
            my_strcpy(buf, last_slash + 1);
            zero_padded_char(dst_1, 100, buf);

            // Null-terminate + write prefix
            last_slash[1] = '\0';
            zero_padded_char(dst_2, 155, src);
            free(buf);
        } 
    }
    /* If file name is shorter than 100 */
    else 
    {
        // Set prefix to NULL
        memset(dst_2, 0, 155);
        // Write name
        zero_padded_char(dst_1, 100, src);
    }
}


void find_linkname(char* file_name, char *header) // do you really need to malloc here??
{             
    // Create buffer                
    char buf[100] = {0}; // if segv here is problem, return to malloc
    ssize_t bytes;
    ssize_t buf_size = 100;

    // Readlink() into buf
    bytes = readlink(file_name, buf, buf_size);

    // If readlink() produces error, exit program
    if (bytes == -1) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }

    // Null-terminate
    buf[bytes] = '\0';

    // If linkname takes up the full buf it may have been longer
    if (bytes == buf_size) {
        printf("(Linkname may have been truncated)\n");
    }

    // Copy buf into header->linkname with padding
    zero_padded_char(header, 100, buf); 
}


unsigned int calculate_checksum(header_t *header) // try function pointers?
{
    // Initiate sum to zero
    unsigned int sum = 0;

    // Add ascii value of each character in header to sum
    sum += calculate_string_ascii_sum(header->name, 100);
    sum += calculate_string_ascii_sum(header->mode, 8);
    sum += calculate_string_ascii_sum(header->uid, 8);
    sum += calculate_string_ascii_sum(header->gid, 8);
    sum += calculate_string_ascii_sum(header->size, 12);
    sum += calculate_string_ascii_sum(header->mtime, 12);
    sum += calculate_string_ascii_sum(header->chksum, 8);
    sum += header->typeflag;
    sum += calculate_string_ascii_sum(header->linkname, 100);
    sum += calculate_string_ascii_sum(header->magic, 6);
    sum += calculate_string_ascii_sum(header->version, 2);
    sum += calculate_string_ascii_sum(header->uname, 32);
    sum += calculate_string_ascii_sum(header->gname, 32);
    sum += calculate_string_ascii_sum(header->devmajor, 8);
    sum += calculate_string_ascii_sum(header->devminor, 8);
    sum += calculate_string_ascii_sum(header->prefix, 155);

    return sum;
}

unsigned int calculate_string_ascii_sum(char *header, int size)
{
    unsigned int sum = 0;
    int i = 0;
    while (i < size) {
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

    dst[i] = '\0';

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

    // TESTING
    // printf("header->name = |%s|\n", header->name); printf("header->prefix = |%s|\n", header->prefix);
    // printf("Mode: %jo (octal)\n", (uintmax_t) sb.st_mode);
    // printf("header->mode = |%s|\n", header->mode);
    // printf("int chksum = |%u|\n", chksum);
    // printf("devmajor = |%i|\n", devmajor);
