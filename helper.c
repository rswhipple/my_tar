#include "header.h"

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
    int len = my_strlen(src);

    for (i = 0; i < len; i++) {
        dst[i] = src[i]; 
    }

    dst[i] = '\0';

    return dst;
}


char* my_strncpy(char* dst, char* src, int index)
{
    int i;
    int length = my_strlen(src);

    for (i = 0; index < length; i++) {
        dst[i] = src[index];
        index += 1; 
    }

    dst[i] = '\0';

    return dst;
}


int my_strcmp(char* str_1, char* str_2) 
{
    int i;
    int str_to_int_1 = 0;
    int str_to_int_2 = 0;

    for (i = 0; str_1[i] != '\0'; i++) {
        str_to_int_1 += str_1[i];
    } 

    for (i = 0; str_2[i] != '\0'; i++) {
        str_to_int_2 += str_2[i];
    } 
    
    if (str_to_int_1 == str_to_int_2) {
        return 0;
    } else if (str_to_int_1 > str_to_int_2) {
        return -1;
    } else {
        return 1;
    }
}


int my_strlen(char* str) 
{
    int i = 0;

    do {
        i += 1;
    } while (str[i] != '\0');

    return i;
}


long my_strtol(char *str, char **endptr, int base) 
{
    // Skip leading whitespace 
    while (my_is_space(*str)) {
        str += 1;
    }

    // Conversion
    long result = 0;
    while (*str != '\0') {
        int digit;
        if (my_is_digit(*str)) {
            digit = *str - '0';
        } else if (my_is_alpha(*str)) {
            digit = my_to_downcase(*str) - 'a' + 10;
        } else {
            break;  // Invalid character, stop conversion
        }

        if (digit >= base) {
            break;  // Invalid digit for base, stop conversion
        }

        result = result * base + digit;
        str += 1;
    }

    // Update endptr if provided
    if (endptr != NULL) {
        *endptr = (char *)str;
    }

    return result;
}


char* my_strcat(char *dst, char *src) { 
    // Save original destination pointer
    char *original_dst = dst; 
    
    // Move destination pointer to the end of the first string
    while (*dst != '\0') {
        dst += 1;
    }

    // Copy characters from src to dst
    while (*src != '\0') {
        *dst = *src;
        dst += 1;
        src += 1;
    }

    // Null-terminate 
    *dst = '\0';

    return original_dst;
}



int my_is_space(int c)
{
    if (c == ' ' || c == '\t' || c == '\n') {
        return 1; 
    } else {
        return 0;
    }
}


int my_is_digit(int c)
{
    if (c >= '0' && c <= '9') {
        return 1; 
    } else {
        return 0;
    }
}


int my_is_alpha(int c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        return 1; 
    } else {
        return 0;
    }
}


int my_to_downcase(int c)
{
    int output = c;
    if (c >= 'A' && c <= 'Z') {
        output += 32;
    } 

    return output;
}

