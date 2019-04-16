/**
 * definitions for RLE file parser
 * @see http://www.conwaylife.com/wiki/RLE
 */

#ifndef __RLE_PARSER_H__
#define __RLE_PARSER_H__

#include "utils.h"
#include "pattern.h"

pattern_state rle_load_pattern(Pattern *pattern, pattern_state targ_state);
int rle_save_pattern(Path file, Pattern *pattern);
#endif
