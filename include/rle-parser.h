/**
 * definitions for RLE file parser
 * @see http://www.conwaylife.com/wiki/RLE
 */

int rle_patternlist_from_dir(char *path, char *ext, Pattern ***patterns);
void rle_free_patternlist(Pattern **patterns, int len);

int rle_load_meta(char *file_name, Pattern *pattern);
int rle_load_data(Pattern *pattern, char *data);

