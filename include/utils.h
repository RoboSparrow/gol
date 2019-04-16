/**
 * definitions for misc utility units
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#define LOG_ERROR(format) do { fprintf(stderr, "[error](%s:%d) " format "\n", __FILE__, __LINE__); } while(0)
#define LOG_ERROR_F(format, ...) do { fprintf(stderr, "[error](%s:%d) " format "\n", __FILE__, __LINE__, __VA_ARGS__); } while(0)

#define EXIT_NULL(val, message) do { if (val == NULL) { fprintf(stderr, "[fatal]: %s\n", message); exit(EXIT_FAILURE); } } while(0)
#define EXIT_MINUS(val, message) do { if (val < 0) { fprintf(stderr, "[fatal]: %s\n", message); exit(EXIT_FAILURE); } } while(0)
#define EXIT_NULL_F(val, format, ...) do { if (val ==NULL) { fprintf(stderr, "[error](%s:%d) " format "\n", __FILE__, __LINE__, __VA_ARGS__); exit(EXIT_FAILURE); } } while(0)
#define EXIT_MINUS_F(val, format, ...) do { if (val < 0) { fprintf(stderr, "[error](%s:%d) " format "\n", __FILE__, __LINE__, __VA_ARGS__); exit(EXIT_FAILURE); } } while(0)

#define RETURN_VOID_IF(expr) if (expr) { return; }
#define RETURN_VAL_IF(expr, val) if (expr) { return val; }
#define RETURN_VOID_IF_ERR(expr, msg) if (expr) { LOG_ERROR(msg); return; }
#define RETURN_VAL_IF_ERR(expr, val, msg)  if (expr) { LOG_ERROR(msg); return val; }

#define MAX_PATH_LENGTH PATH_MAX
typedef char Path[MAX_PATH_LENGTH];

int str_startswith(char *search, char *str);
void str_nospaces(char* source);
char *str_trim(char *str);
void str_tolower(char *str);
char *str_getfileext(char *filename);
int path_build(char *segment, Path path);

#define GENLIST_INIT_SIZE 5

typedef struct GenList {
    void **items;
    int size;
    int length;
} GenList;

void genlist_init(GenList *list);
void genlist_push(GenList *list, void *item);

void genlist_set(GenList *list, int index, void *item);
void *genlist_get(GenList *list, int index);
void genlist_delete(GenList *list, int index);
void genlist_flush(GenList *list);
void genlist_free(GenList *list);
#endif
