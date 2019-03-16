/**
 * definitions for RLE file parser
 * @see http://www.conwaylife.com/wiki/RLE
 */
#include "pattern.h"

int rle_load_meta(char *file_name, Pattern *pattern);
int rle_load_data(Pattern *pattern, char *data);
char *rle_load_pattern(char *file, Pattern *pattern);
