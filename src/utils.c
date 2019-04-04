/**
 * misc utility units
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h>
#include <malloc.h>

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
 * trims a string left and right
 */
char *str_trim(char *str) {
    char *end;
    // leading
    while (isspace(*str)) {
        str = str + 1;
    }
    // trailing
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end = end - 1;
    }

    *(end + 1) = '\0';
    return str;
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

////
// Dynamic Generic Array
// based on the excellent tutorial by Ed Mann: https://eddmann.com/posts/implementing-a-dynamic-vector-array-in-c/
////

/**
 * Re-allocates memory for nsize data items
 * @param list Pointer to Genlist
 * @param nsize new capacity
 */
static void genlist_resize(GenList *list, int nsize) {
    void **items = realloc(list->items, sizeof(void *) * nsize);
    if (items == NULL) {
        LOG_ERROR_F("Cannot re-allocate memory for %d items", nsize);
        return;
    }

    list->items = items;
    list->size = nsize;
}

/**
 * Initialize generic array
 * @param list Pointer to Genlist
 */
void genlist_init(GenList *list) {
    list->size = GENLIST_INIT_SIZE;
    list->length = 0;
    list->items = malloc(sizeof(void *) * list->size);

    if (list->items == NULL) {
        list->size = 0;
        LOG_ERROR_F("Cannot allocate memory for %d items", list->size);
    }
}

/**
 * Add a new data item to the end of the generic array
 * @param list Pointer to Genlist
 * @param item item data
 */
void genlist_push(GenList *list, void *item) {
    if (list->size <= list->length) {
        int nsize = list->size * 2;
        genlist_resize(list, nsize);
    }
    list->items[list->length++] = item;
}

/**
 * Insert new data to an existing data item
 * @param list Pointer to Genlist
 * @param index position in data array to be replaced
 * @param item item data
 */
void genlist_set(GenList *list, int index, void *item) {
    if (index >= 0 && index < list->length) {
        list->items[index] = item;
    }
}

/**
 * Get an existing data item for a given position.
 * The data item is a void pointer, so has to be casted to it's known type
 * @param list Pointer to Genlist
 * @param index position in data array to be fetched
 */
void *genlist_get(GenList *list, int index) {
    if (index >= 0 && index < list->length) {
        return list->items[index];
    }
    return NULL;
}

/**
 * Delete an existing data item for a given position.
 * @param list Pointer to Genlist
 * @param index position in data array to be deleted
 */
void genlist_delete(GenList *list, int index) {
    if (index < 0 || index >= list->length) {
        return;
    }

    list->items[index] = NULL;

    for (int i = index; i < list->length - 1; i++) {
        list->items[i] = list->items[i + 1];
        list->items[i + 1] = NULL;
    }

    list->length--;

    if (list->length > 0 && list->length == list->size / 4) {
        genlist_resize(list, list->size / 2);
    }
}

/**
 * Flush a dynamic array instance. Clear all data items, reset length to zero and allocate min memory.
 * @param list Pointer to Genlist
 * @param index position in data array to be deleted
 */
void genlist_flush(GenList *list) {
    if (list->length == 0) {
        return;
    }
    for (int i = 0; i < list->length - 1; i++) {
        list->items[i] = NULL;
    }
    list->length = 0;
    genlist_resize(list, GENLIST_INIT_SIZE);
}

/**
 * Fee and destroy a dynamic array instance.
 * @param list Pointer to Genlist
 */
void genlist_free(GenList *list) {
    if(list->items != NULL) {
        free(list->items);
    }
    list = NULL;
}
