/**
 * definitions for Pattern structs (GOL meta data)
 */

#ifndef __PATTERN_H__
#define __PATTERN_H__

#include <limits.h>

#include "utils.h"

#define LINE_BOUNDS 255 /** max line length, for header and comment lines only */

typedef enum {
    PATTERN_NONE,
    PATTERN_META,
    PATTERN_FULL,
} pattern_state;

typedef enum {
    PATTERN_CENTER,
    PATTERN_TOPLEFT
} PatternOrigin;

typedef struct pattern {
    char title[LINE_BOUNDS];
    char description[LINE_BOUNDS];
    Path file;
    int cols;
    int rows;
    pattern_state state;
    char *data;
} Pattern;

int pattern_load_patternlist(char *dirname, GenList *list);
void pattern_free_patternlist(GenList *list);

Pattern *pattern_allocate_pattern();
void pattern_free_pattern(Pattern *pattern);

void pattern_print_pattern(Pattern *pattern);

int pattern_load_file(char *file, Pattern *pattern, pattern_state targ_state);
int pattern_save_file(char *file, Pattern *pattern);

int pattern_load_file_and_merge(char *file, Pattern *world, int offsetX, int offsetY, PatternOrigin origin);
#endif
