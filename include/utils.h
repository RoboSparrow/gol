/**
 * definitions for misc utility units
 */
 #include <limits.h>

typedef char Path[PATH_MAX];

void str_nospaces(char* source);
void str_trim(char *str);
void str_tolower(char *str);
char *str_getfileext(char *filename);
int path_build(char *segment, Path path);

