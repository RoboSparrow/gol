/**
 * definitions for Pattern structs (GOL meta data)
 */
#include <limits.h>
#include "utils.h"

#define LINE_BOUNDS 255 /** max line length, for header and comment lines only */

#ifndef PATTERN_GUARD
#define PATTERN_GUARD
typedef struct pattern {
    char title[LINE_BOUNDS];
    char description[LINE_BOUNDS];
    Path file;
    int cols;
    int rows;
} Pattern;

typedef struct patternList {
    int len;
    Pattern **patterns;
} PatternList;
#endif

PatternList *pattern_load_patternlist(char *path, char *ext);
void pattern_free_patternlist(PatternList *list);
void pattern_print_pattern(Pattern *pattern);
