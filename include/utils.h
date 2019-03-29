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

#define EXIT_NULL(val, message) do { if (val == NULL) { fprintf(stderr, "[fatal]: %s", message); exit(EXIT_FAILURE); } } while(0)
#define EXIT_MINUS(val, message) do { if (val < 0) { fprintf(stderr, "[fatal]: %s", message); exit(EXIT_FAILURE); } } while(0)
#define EXIT_NULL_F(val, format, ...) do { if (val ==NULL) { fprintf(stderr, "[error](%s:%d) " format "\n", __FILE__, __LINE__, __VA_ARGS__); exit(EXIT_FAILURE); } } while(0)
#define EXIT_MINUS_F(val, format, ...) do { if (val < 0) { fprintf(stderr, "[error](%s:%d) " format "\n", __FILE__, __LINE__, __VA_ARGS__); exit(EXIT_FAILURE); } } while(0)

typedef char Path[PATH_MAX];

void str_nospaces(char* source);
char *str_trim(char *str);
void str_tolower(char *str);
char *str_getfileext(char *filename);
int path_build(char *segment, Path path);

#endif
