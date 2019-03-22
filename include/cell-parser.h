/**
 * definitions for RLE file parser
 * @see http://www.conwaylife.com/wiki/RLE
 */

#ifndef __CELL_PARSER_H__
#define __CELL_PARSER_H__

#include "pattern.h"

pattern_state cell_load_pattern(char *file, Pattern *pattern, pattern_state targ_state);
int cell_save_pattern(Path file, Pattern *pattern);
#endif
