#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <string.h>

#include "pattern.h"
#include "gol.h"
#include "rle-parser.h"

/**
 * creates pattern with zero values
 */
Pattern *pattern_allocate_pattern() {
    Pattern *pattern = calloc(1, sizeof(Pattern));
    if (pattern == NULL) {
        fprintf(stderr, "Could not allocate enough memory for pattern.\n");
        return pattern;
    }
    return pattern;
}

/**
 * frees allocated pattern
 */
void pattern_free_pattern(Pattern *pattern) {
    if (pattern == NULL) {
        return;
    }
    free(pattern);
}

/**
 * creates pattern list - read a directory of files, filter out pattern files by extension and parse meta data into a list
 * @param dirname  name of tartget dir within the directory of the executable
 * @param ext file extension to filter directory for
 */
PatternList *pattern_load_patternlist(char *dirname, char *ext) {

    PatternList li = {
        .len = 0,
        .patterns = NULL,
    };
    PatternList *list = &li;

    // open dir
    DIR *dir;
    int len = 0;
    struct dirent *e = NULL;

    Path path;
    path_build(dirname, path);

    dir = opendir(path);
    if(dir == NULL) {
        fprintf(stderr, "rle-parser: Unable able to open dir %s.\n", path);
        return NULL;
    }

    // get length
    e = readdir(dir);
    while(NULL != e){
        char *fext = str_getfileext(e->d_name);
        if(strcmp(fext, ext) == 0) {
            // printf("%s\n", e->d_name);
            len++;
        }
        e = readdir(dir);
    }
    rewinddir(dir);

    // allocate mem for patterns
    list->len = len;
    list->patterns = malloc(len * sizeof(Pattern *));
    if(list->patterns == NULL) {
        fprintf(stderr, "rle-parser: unable to re-allocate memory for patterns.\n");
        return NULL;
    }

    // read files and parse meta data
    int i = 0;
    e = readdir(dir);
    while(NULL != e){
        char *fext = str_getfileext(e->d_name);
        if(strcmp(fext, ext) == 0) {
                Pattern *pattern = pattern_allocate_pattern();

                strcpy(pattern->file, path);
                strcat(pattern->file, "/");
                strcat(pattern->file, e->d_name);

                int loaded  = 0;
                // insert parser of choice here
                if (strcmp(ext, "rle") == 0) {
                    loaded = rle_load_meta(pattern->file, pattern);
                }

                if(loaded < 0) {
                    fprintf(stderr, "error loading pattern file %s\n", pattern->file);
                } else {
                    list->patterns[i] = pattern;
                    i++;
                }
        }

        e = readdir(dir);
    }

    closedir(dir);

    return list;
}

/**
 * free a collection of patterns
 * @param patterns reference to an initialized array of Pattern structs,
 * @param len length of list
 */
void pattern_free_patterns(Pattern **patterns, int len) {
    if(patterns != NULL) {
        for(int i = 0; i < len; i++) {
            pattern_free_pattern(patterns[i]);
        }
        free(patterns);
    }
}

/**
 * free a pattern list
 * @param patterns reference to an initialized array of Pattern structs,
 * @param len length of list
 */
void pattern_free_patternlist(PatternList *list) {
    if(list != NULL) {
        pattern_free_patterns(list->patterns, list->len);
    }
}

/**
 * prints pattern info
 * @param pattern Pattern struct
 */
void pattern_print_pattern(Pattern *pattern) {
    if (pattern == NULL) {
        printf("- pattern is empty\n");
    }
    // TODO into pattern.h
    char *pattern_state_names[] = {
        "NONE", "META", "FULL"
    };
    printf("    |_ title: %s\n", pattern->title);
    printf("    |_ description: %s\n", pattern->description);
    printf("    |_ file: %s\n", pattern->file);
    printf("    |_ cols: %d\n", pattern->cols);
    printf("    |_ rows: %d\n", pattern->rows);
    printf("    |_ state: %s\n", (pattern->state < 3) ? pattern_state_names[pattern->state] : "UNKNOWN");
    printf("    |_ data: %s\n", pattern->data);
}

/**
 * Load pattern from file and merge into given world
 * @param patternfile file to load
 * @param ext file extension, flags parser to invoke
 * @param world target data matrix to merge into
 * @param cols target data matrix width
 * @param cols target data matrix height
 * @param colOffset x-offset target data matrix
 * @param rowOffset y-offset target data matrix
 * @return -1 on error, else length of merged pattern data
 */
int pattern_merge_from_file(char *patternfile, char *ext, char *world, int cols, int rows, int colOffset, int rowOffset) {
        // pattern from file
        Pattern *pattern = pattern_allocate_pattern();
        if(pattern == NULL) {
            return -1;
        }
        char *data = NULL;
        // todo switch ext
        data = rle_load_pattern(patternfile, pattern);
        int size = pattern->cols * pattern->rows;

        if(data == NULL) {
            return -1;
        }

        gol_merge_data(
            data, pattern->cols, pattern->rows,
            world, cols, rows,
            colOffset, rowOffset
        );

        gol_free_data(data);
        pattern_free_pattern(pattern);

        return size;
}
