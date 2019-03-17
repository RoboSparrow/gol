/**
 * definitions for misc utility units
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#define EXIT_NULL(val, message) if (val == NULL) fprintf(stderr, "fatal error: %s", message); exit(EXIT_FAILURE);

typedef char Path[PATH_MAX];

void str_nospaces(char* source);
void str_trim(char *str);
void str_tolower(char *str);
char *str_getfileext(char *filename);
int path_build(char *segment, Path path);

#endif
