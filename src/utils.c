/**
 * misc utility units
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h>

#include "utils.h"

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

/**
 * TODO
 * Resolves a path segment based on the current working directory
 * If no segment is passed, the function will return the absolute path of the current working directory.
 * @param segment path segement (i.e."patterns")
 * @param path Path buffer to write into
 *
 *
 */
int path_build(char *segment, Path path) {
    extern Path FS_ROOT_DIR;

    strcpy(path, FS_ROOT_DIR);
    if (segment[0] == '\0') {
        return strlen(path);
    }

    int clen = strlen(FS_ROOT_DIR) + strlen(segment);
    if (clen > PATH_MAX) {
        LOG_ERROR_F("Combined len %d is lager than MAX_PATH (%d)", clen, PATH_MAX);
        return 0;
    }

    strcat(path, "/");
    strcat(path, segment);

    // todo check with access?
    return strlen(path);
}
