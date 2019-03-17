/**
 * definitions for Pattern structs (GOL meta data)
 */
#include <limits.h>
#include "utils.h"

#define LINE_BOUNDS 255 /** max line length, for header and comment lines only */

#ifndef PATTERN_GUARD
#define PATTERN_GUARD
typedef enum {
    PATTERN_NONE,
    PATTERN_META,
    PATTERN_FULL,
} pattern_state;

typedef struct pattern {
    char title[LINE_BOUNDS];
    char description[LINE_BOUNDS];
    Path file;
    int cols;
    int rows;
    pattern_state state;
    char *data;
} Pattern;

typedef struct patternList {
    int len;
    Pattern **patterns;
} PatternList;
#endif

PatternList *pattern_load_patternlist(char *dirname, char *ext);
void pattern_free_patternlist(PatternList *list);

Pattern *pattern_allocate_pattern();
void pattern_free_pattern(Pattern *pattern);
void pattern_print_pattern(Pattern *pattern);

int pattern_merge_from_file(char *patternfile, char *ext, char *world, int cols, int rows, int colOffset, int rowOffset);
