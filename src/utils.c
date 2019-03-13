/**
 * misc utility units
 */

#include <string.h>
#include <ctype.h>

/**
 * strips whistespaces from a string
 */
void str_nospaces(char* source) {
    char* i = source;
    char* j = source;
    while(*j != 0) {
        *i = *j++;
        if(*i != ' ') {
            i++;
        }
    }
    *i = 0;
}

/**
 * trims a stringleft and right, removes line breaks
 */
void str_trim(char *str) {
    while (isspace(*str)) {
        str++;
    }

    while(*str && *str != '\n' && *str != '\r') {
        str++;
    }

    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end = end - 1;
    }
    *str = 0;
}

/**
 * converts a string to lower case
 */
void str_tolower(char *str) {
    for(int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }
}

/**
 * extracts file extension from a string
 */
char *str_getfileext(char *filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) {
        return "";
    }
    return dot + 1;
}
