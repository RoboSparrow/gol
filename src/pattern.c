#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <string.h>

#include "pattern.h"
#include "gol.h"
#include "rle-parser.h"
#include "cell-parser.h"
#include "utils.h"

/**
 * creates pattern with zero values
 */
Pattern *pattern_allocate_pattern() {
    Pattern *pattern = calloc(1, sizeof(Pattern));
    if (pattern == NULL) {
        LOG_ERROR("Could not allocate enough memory for pattern.");
        return pattern;
    }
    pattern->data = NULL;
    return pattern;
}

/**
 * frees allocated pattern
 */
void pattern_free_pattern(Pattern *pattern) {
    if (pattern == NULL) {
        return;
    }
    gol_free_data(pattern->data);
    free(pattern);
}

/**
 * creates pattern list - read a directory of files, filter out pattern files by extension and parse meta data into a list
 * @param dirname  name of tartget dir within the directory of the executable
 * @param list Pointer to Genlist
 * @return list length or -1 on error
 */
int pattern_load_patternlist(char *dirname, GenList *list) {

    // free and set len to zero
    genlist_flush(list);

    // open dir
    DIR *dir;
    int len = 0;
    struct dirent *e = NULL;

    Path path = "";
    path_build(dirname, path);

    dir = opendir(path);
    if(dir == NULL) {
        LOG_ERROR_F("Unable able to open dir %s.", path);
        return -1;
    }

    // get length
    e = readdir(dir);
    while(NULL != e){
        char *fext = str_getfileext(e->d_name);
        if(strcmp(fext, "rle") == 0 || strcmp(fext, "cells") == 0) {
            // printf("%s\n", e->d_name);
            len++;
        }
        e = readdir(dir);
    }
    rewinddir(dir);

    // read files and parse meta data
    int i = 0;
    e = readdir(dir);
    while(NULL != e){
        char *fext = str_getfileext(e->d_name);
        if(strcmp(fext, "rle") == 0 || strcmp(fext, "cells") == 0) {
                Pattern *pattern = pattern_allocate_pattern();

                strcpy(pattern->file, path);
                strcat(pattern->file, "/");
                strcat(pattern->file, e->d_name);

                pattern_state loaded;
                // insert parser of choice here
                if (strcmp(fext, "rle") == 0) {
                    loaded = rle_load_pattern(pattern->file, pattern, PATTERN_META);
                }

                if (strcmp(fext, "cells") == 0) {
                    loaded = cell_load_pattern(pattern->file, pattern, PATTERN_META);
                }

                if(loaded != PATTERN_META) {
                    LOG_ERROR_F("error loading pattern file %s", pattern->file);
                } else {
                    genlist_push(list, pattern);
                    i++;
                }
        }

        e = readdir(dir);
    }

    closedir(dir);

    return list->length;
}

/**
 * free a GenList of pattern pointers and reset list to  zero length
 * @param list Pointer to Genlist
 */
void pattern_free_patternlist(GenList *list) {
    if(list->items == NULL) {
        return;
    }

    for (int i = 0; i < list->length; i++) {
        Pattern *pattern = (Pattern *) genlist_get(list, i);
        pattern_free_pattern(pattern);
    }

    genlist_flush(list);
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
    if(pattern->data) {
        printf("    |_ data:\n");
        gol_print_data(pattern->data, pattern->cols, pattern->rows);
    }
}

/**
 * Load pattern from file
 * @param file file to load
 * @param pattern allocated Pattern to load into
 * @param targ_state flag, which level data should be loaded
 * @return -1 on error, else length of merged pattern data
 */
int pattern_load_file(char *file, Pattern *pattern, pattern_state targ_state) {

    // todo path_build?
    char *ext = str_getfileext(file);

    Path path = "";
    path_build(file, path);

    pattern->state = PATTERN_NONE;

    if (strcmp(ext, "rle") == 0) {
        pattern_state state = rle_load_pattern(path, pattern, targ_state);
        if(state != targ_state) {
            return -1;
        }
        return 0;
    }

    if (strcmp(ext, "cells") == 0) {
        pattern_state state = cell_load_pattern(path, pattern, targ_state);
        if(state != targ_state) {
            return -1;
        }
        return 0;
    }

    LOG_ERROR_F("pattern: No parser found for file (load)%s.", path);
    return -1;
}

/**
 * Save pattern to file
 * @param file file to load
 * @param pattern allocated Pattern to load into
 * @return -1 on error, else length of merged pattern data
 */
int pattern_save_file(char *file, Pattern *pattern) {
    // todo path_build?
    char *ext = str_getfileext(file);

    Path path = "";
    path_build(file, path);

    if (strcmp(ext, "rle") == 0) {
        return rle_save_pattern(path, pattern);
    }

    if (strcmp(ext, "cells") == 0) {
        return cell_save_pattern(path, pattern);
    }

    LOG_ERROR_F("pattern: No parser found for file (save)%s", file);
    return -1;
}
