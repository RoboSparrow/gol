#include <stdio.h>
#include <malloc.h>
#include <dirent.h>
#include <string.h>

#include "pattern.h"
#include "rle-parser.h"

/**
 * creates pattern list - read a directory of files, filter out pattern files by extension and parse meta data into a list
 * @param dirname  name of tartget dir within the directory of the executable
 * @param ext file extension to filter directory for
 * @param patterns reference to an initialized array of Pattern structs, this may be empoty. The required memory will be expanded by this function
 * @return -1 on error, else number of patterns parsed, 0 if empty
 */
int pattern_read_dir(char *dirname, char *ext, Pattern ***patterns) {
    DIR *dir;
    int len = 0;
    struct dirent *e = NULL;

    Path path;
    path_build(dirname, path);

    dir = opendir(path);
    if(dir == NULL) {
        fprintf(stderr, "rle-parser: Unable able to open dir %s.\n", path);
        return -1;
    }

    //// first count
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

    //// allocate patterns

    *patterns = (Pattern **) realloc(**patterns, len * sizeof(Pattern *));
    if(*patterns == NULL) {
        fprintf(stderr, "rle-parser: unable to re-allocate memory for patterns.\n");
        return -1;
    }

    int i = 0;
    e = readdir(dir);
    while(NULL != e){
        char *fext = str_getfileext(e->d_name);
        if(strcmp(fext, ext) == 0) {
                Pattern *pattern = calloc(1, sizeof(Pattern));

                strcpy(pattern->file, path);
                strcat(pattern->file, "/");
                strcat(pattern->file, e->d_name);

                int loaded  = 0;
                if (strcmp(ext, "rle") == 0) {
                    loaded = rle_load_meta(pattern->file, pattern);
                }

                if(loaded < 0) {
                    fprintf(stderr, "error loading pattern file %s\n", pattern->file);
                } else {
                    (*patterns)[i] = pattern;
                    i++;
                }
        }

        e = readdir(dir);
    }

    closedir(dir);
    return len;
}

PatternList *pattern_load_patternlist(char *path, char *ext) {

    PatternList li = {
        .len = 0,
        .patterns = NULL
    };
    PatternList *list = &li;

    Pattern **patterns = (Pattern **) malloc(sizeof(Pattern *));
    if(patterns == NULL) {
        fprintf(stderr, "Unable to allocate memory for patterns.\n");
        return NULL;
    }

    int len =  pattern_read_dir(path, ext, &patterns);
    if (len < 0) {
        fprintf(stderr, " pattern_read_dir: Unable able to read dir (%s).\n", path);
    }

    for(int i = 0; i < len; i++) {
        printf("- pattern %d\n", i);
        pattern_print_pattern(patterns[i]);
    }

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
            if(patterns[i] != NULL) {
                free(patterns[i]);
            }
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

    printf("    |_ title: %s\n", pattern->title);
    printf("    |_ description: %s\n", pattern->description);
    printf("    |_ file: %s\n", pattern->file);
    printf("    |_ cols: %d\n", pattern->cols);
    printf("    |_ rows: %d\n", pattern->rows);
}
