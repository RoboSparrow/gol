/**
 * definitions tests
 */

#ifndef __TEST_H__
#define __TEST_H__

#include "utils.h"
#include "gol.h"
#include "state.h"
#include "rle-parser.h"
#include "pattern.h"
#include "init.h"

#define __DOING_TESTS__

#define GROUP(message) fprintf(stderr, "\n\033[01;33mGROUP: %s\033[0m\n", message)
#define DESCRIBE(message) fprintf(stderr, "\033[1;34m=> \033[0m %s() %s\n", __func__, message)
#define DONE() fprintf(stderr, "    |_ \033[1;32mPASS\033[0m: %s\n\n", __func__)


typedef struct testpattern {
    char title[256];
    char description[256];
    int cols;
    int rows;
    pattern_state state;
    char data[512];
} TestPattern;

void test_init_pattern(Pattern *pattern);

//// RLE parser
void test_rle_loadfile_meta(Pattern *pattern);
void test_rle_loadfile_full(Pattern *pattern);
void test_rle_savefile(Pattern *pattern);

//// CELL parser
void test_cell_loadfile_meta(Pattern *pattern);
void test_cell_loadfile_full(Pattern *pattern);
void test_cell_savefile(Pattern *pattern);

void test_cell_loadsave_blanklines(Pattern *pattern);

void test_genlist_init();
void test_genlist_push();
void test_genlist_push_realloc();
void test_genlist_set();
void test_genlist_get();
void test_genlist_delete();
void test_genlist_flush();
void test_genlist_free();
#endif
