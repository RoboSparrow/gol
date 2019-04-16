#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "pattern.h"
#include "gol.h"
#include "rle-parser.h"
#include "cell-parser.h"
#include "utils.h"

////
// pattern list sorting
////

static int patternlist_compareby_title(const void *a, const void *b) {
    Pattern *pa = *(Pattern **) a;
    Pattern *pb = *(Pattern **) b;
    return strcmp(pa->title, pb->title);
}

static int patternlist_compareby_file(const void *a, const void *b) {
    Pattern *pa = *(Pattern **) a;
    Pattern *pb = *(Pattern **) b;
    return strcmp(pa->file, pb->file);
}

static int patternlist_compareby_size(const void *a, const void *b) {
    Pattern *pa = *(Pattern **) a;
    Pattern *pb = *(Pattern **) b;
    return (pa->rows * pa->cols) - (pb->rows * pb->cols);
}

static int patternlist_compareby_mtime(const void *a, const void *b) {
    Pattern *pa = *(Pattern **) a;
    Pattern *pb = *(Pattern **) b;
    return pa->mtime - pb->mtime;
}


////
// Api
////

/**
 * find pattern type based on file extension
 */
PatternType pattern_get_type(char *file) {
    int len = strlen(file);
    char *ext;
    if (len < 5) { // 1 char + '.rle'
        return PATTERN_TYPE_NONE;
    }

    ext = &file[len - 4];
    if(strcmp(ext, ".rle") == 0) {
        return PATTERN_TYPE_RLE;
    }
    ext = &file[len - 6];
    if(strcmp(ext, ".cells") == 0) {
        return PATTERN_TYPE_CELLS;
    }
    return PATTERN_TYPE_NONE;
}

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
int pattern_load_patternlist(char *dirname, GenList *list, PatternSort order) {

    // free and set len to zero
    genlist_flush(list);

    // open dir
    DIR *dir;
    struct dirent *e = NULL;

    Path path = "";
    path_build(dirname, path);

    dir = opendir(path);
    if(dir == NULL) {
        LOG_ERROR_F("Unable able to open dir %s.", path);
        return -1;
    }

    // read files and parse meta data
    e = readdir(dir);
    while(NULL != e){
        Path file;
        snprintf(file, MAX_PATH_LENGTH, "%s/%s", path, e->d_name);

        if(pattern_get_type(file) != PATTERN_TYPE_NONE) {
            Pattern *pattern = pattern_allocate_pattern();
            int loaded = pattern_load_file(file, pattern, PATTERN_META);
            if(loaded < 0) {
                LOG_ERROR_F("error loading pattern file %s", file);
            } else {
                genlist_push(list, pattern);
            }
        }
        e = readdir(dir);
    }

    closedir(dir);

    switch (order) {
        case PATTERN_SORT_TITLE:
            qsort(list->items, list->length, sizeof(Pattern*), patternlist_compareby_title);
        break;
        case PATTERN_SORT_FILE:
            qsort(list->items, list->length, sizeof(Pattern*), patternlist_compareby_file);
        break;
        case PATTERN_SORT_SIZE:
            qsort(list->items, list->length, sizeof(Pattern*), patternlist_compareby_size);
        break;
        case PATTERN_SORT_MTIME:
            qsort(list->items, list->length, sizeof(Pattern*), patternlist_compareby_mtime);
        break;
        case PATTERN_SORT_NONE:
        break;
    }

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
    printf("    |_ modified: %s\n", ctime((&pattern->mtime)));
    if(pattern->data == NULL) {
        printf("    |_ data: NULL\n");
    } else {
        printf("    |_ data:\n");
        gol_print_data(pattern->data, pattern->cols, pattern->rows);
    }
}

/**
 * Load pattern from file
 * @param file file to load
 * @param pattern allocated Pattern to load into
 * @param targ_state flag, which level data should be loaded
 * @return -1 on error
 */
int pattern_load_file(char *file, Pattern *pattern, pattern_state targ_state) {
    PatternType type = pattern_get_type(file);
    if (type >= PATTERN_TYPE_NONE) {
        LOG_ERROR_F("pattern: No parser type found for file (load)%s.", pattern->file);
        return -1;
    }

    path_build(file, pattern->file);
    pattern->state = PATTERN_NONE;

    struct stat fstat;
    if (stat(pattern->file, &fstat) < 0) {
        LOG_ERROR_F("could not stat file: %s.", pattern->file);
        pattern->mtime = 0;
    } else {
        pattern->mtime = fstat.st_mtime;
    }

    if (type == PATTERN_TYPE_RLE) {
        pattern_state state = rle_load_pattern(pattern, targ_state);
        if(state != targ_state) {
            return -1;
        }
        return 0;
    }

    if (type == PATTERN_TYPE_CELLS) {
        pattern_state state = cell_load_pattern(pattern, targ_state);
        if(state != targ_state) {
            return -1;
        }
        return 0;
    }

    LOG_ERROR_F("pattern: Parser type %d. No parser logic found for this type (file: %s)", type, pattern->file);
    return -1;
}

/**
 * Create random pattern
 * @param pattern allocated Pattern to load into
 * @param targ_state flag, which level data should be loaded
 * @return -1 on error
 */
int pattern_create_random(Pattern *pattern, pattern_state targ_state) {
    if (!pattern->cols || !pattern->rows) {
        LOG_ERROR("random pattern: No cols or rows defined!");
        return -1;
    }
    if(targ_state == PATTERN_META) {
        pattern->state = PATTERN_META;
        return 0;
    }
    pattern->data = gol_allocate_data(pattern->data, pattern->cols, pattern->rows);
    gol_random(pattern);
    pattern->state = PATTERN_FULL;
    return 0;
}

/**
 * Save pattern to file
 * @param file file to load
 * @param pattern allocated Pattern to load into
 * @return -1 on error, else length of merged pattern data
 */
int pattern_save_file(char *file, Pattern *pattern) {
    PatternType type = pattern_get_type(file);

    Path path = "";
    path_build(file, path);

    if (type == PATTERN_TYPE_RLE) {
        return rle_save_pattern(path, pattern);
    }

    if (type == PATTERN_TYPE_CELLS) {
        return cell_save_pattern(path, pattern);
    }

    LOG_ERROR_F("pattern: No parser found for file (save)%s", file);
    return -1;
}
