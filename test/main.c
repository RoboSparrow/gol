// adapted to c: http://headerphile.com/sdl2/sdl2-part-4-making-things-happen/
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "utils.h"
#include "gol.h"
#include "paint.h"
#include "state.h"
#include "rle-parser.h"
#include "pattern.h"
#include "init.h"

#define __DOING_TESTS__

#define GROUP(message) fprintf(stderr, "\n\033[01;33mGROUP: %s\033[0m\n", message)
#define DESCRIBE(message) fprintf(stderr, "\033[1;34m=> \033[0m %s() %s\n", __func__, message)
#define DONE() fprintf(stderr, "    |_ \033[1;32mPASS\033[0m: %s\n\n", __func__)

Path FS_ROOT_DIR;

struct ptrn {
    char title[256];
    char description[256];
    int cols;
    int rows;
    pattern_state state;
    char data[256];
};

struct ptrn rle_shuttle = {
    "Queen bee shuttle",
    "A period 30 oscillator.",
    22,
    7,
    PATTERN_FULL,
    ".........O............"
    ".......O.O............"
    "......O.O............."
    "OO...O..O...........OO"
    "OO....O.O...........OO"
    ".......O.O............"
    ".........O............"
};

struct ptrn cell_shuttle = {
    "Queen bee shuttle",
    "Author: Bill Gosper",
    22,
    7,
    PATTERN_FULL,
    ".........O............"
    ".......O.O............"
    "......O.O............."
    "OO...O..O...........OO"
    "OO....O.O...........OO"
    ".......O.O............"
    ".........O............"
};

void test_init_pattern(Pattern *pattern);

//// RLE parser
void test_rle_loadfile_meta(Pattern *pattern);
void test_rle_loadfile_full(Pattern *pattern);
void test_rle_savefile(Pattern *pattern);

//// CELL parser
void test_cell_loadfile_meta(Pattern *pattern);
void test_cell_loadfile_full(Pattern *pattern);
void test_cell_savefile(Pattern *pattern);

int main(int argc, char* argv[]) {
    init_set_rootdir(argc, argv, FS_ROOT_DIR);
    Pattern *pattern;

    //// RLE parser
    GROUP("RLE parser");
    pattern = pattern_allocate_pattern();
    test_init_pattern(pattern);
    test_rle_loadfile_meta(pattern);
    pattern_free_pattern(pattern);

    pattern = pattern_allocate_pattern();
    test_init_pattern(pattern);
    test_rle_loadfile_full(pattern);
    test_rle_savefile(pattern);
    pattern_free_pattern(pattern);

    //// CELL parser
    GROUP("CELL parser");
    pattern = pattern_allocate_pattern();
    test_init_pattern(pattern);
    test_cell_loadfile_meta(pattern);
    pattern_free_pattern(pattern);

    pattern = pattern_allocate_pattern();
    test_init_pattern(pattern);
    test_cell_loadfile_full(pattern);
    test_cell_savefile(pattern);
    pattern_free_pattern(pattern);

    return 0;
}

void test_init_pattern(Pattern *pattern) {
    DESCRIBE("inititializes empty pattern");
    assert(pattern != NULL);
    assert(strlen(pattern->title) == 0);
    assert(strlen(pattern->description) == 0);
    assert(strlen(pattern->file) == 0);
    assert(pattern->cols == 0);
    assert(pattern->rows == 0);
    assert(pattern->state == PATTERN_NONE);
    assert(pattern->data == NULL);
    DONE();
}

//// Tests for RLE parser

void test_rle_loadfile_meta(Pattern *pattern) {
    DESCRIBE("load pattern full");
    extern struct ptrn rle_shuttle;

    int loaded = pattern_load_file("./queenbeeshuttle.rle", pattern, PATTERN_META);
    assert(loaded >= 0);
    assert(strcmp(pattern->title, rle_shuttle.title) == 0);
    assert(strcmp( pattern->description, rle_shuttle.description) == 0);
    assert(pattern->cols == rle_shuttle.cols);
    assert(pattern->rows == rle_shuttle.rows);
    assert(pattern->state == PATTERN_META);
    assert(pattern->data == NULL);
    DONE();
}

void test_rle_loadfile_full(Pattern *pattern) {
    DESCRIBE("loads pattern fully and compares values");
    extern struct ptrn rle_shuttle;

    int loaded = pattern_load_file("./queenbeeshuttle.rle", pattern, PATTERN_FULL);
    assert(loaded >= 0);
    assert(strcmp(pattern->title, rle_shuttle.title) == 0);
    assert(strcmp( pattern->description, rle_shuttle.description) == 0);
    assert(pattern->cols == rle_shuttle.cols);
    assert(pattern->rows == rle_shuttle.rows);
    assert(pattern->state == PATTERN_FULL);

    printf("%s\n%s\n",pattern->data, rle_shuttle.data);
    assert(strcmp(pattern->data, rle_shuttle.data) == 0);

    DONE();
}

void test_rle_savefile(Pattern *pattern) {
    DESCRIBE("saves pattern and then loads saved pattern, compares values");
    char *outfile = "out/save.queenbeeshuttle.rle";

    int saved = pattern_save_file(outfile, pattern);
    assert(saved >= 0);

    Pattern *check = pattern_allocate_pattern();
    int loaded = pattern_load_file(outfile, check, PATTERN_FULL);
    assert(loaded >= 0);

    assert(strcmp(pattern->title, check->title) == 0);
    assert(strcmp( pattern->description, check->description) == 0);
    assert(pattern->cols == check->cols);
    assert(pattern->rows == check->rows);
    assert(pattern->state == check->state);

    printf("%s\n%s\n", pattern->data, check->data);
    assert(strcmp(pattern->data, check->data) == 0);

    pattern_free_pattern(check);
    DONE();
}

//// Tests for CELL parser

void test_cell_loadfile_meta(Pattern *pattern) {
    DESCRIBE("load pattern full");
    extern struct ptrn cell_shuttle;

    int loaded = pattern_load_file("./queenbeeshuttle.cells", pattern, PATTERN_META);
    pattern_print_pattern(pattern);
    assert(loaded >= 0);
    assert(strcmp(pattern->title, cell_shuttle.title) == 0);
    assert(strcmp( pattern->description, cell_shuttle.description) == 0);
    assert(pattern->cols == cell_shuttle.cols);
    assert(pattern->rows == cell_shuttle.rows);
    assert(pattern->state == PATTERN_META);
    assert(pattern->data == NULL);
    DONE();
}

void test_cell_loadfile_full(Pattern *pattern) {
    DESCRIBE("loads pattern fully and compares values");
    extern struct ptrn cell_shuttle;

    int loaded = pattern_load_file("./queenbeeshuttle.cells", pattern, PATTERN_FULL);
    assert(loaded >= 0);
    assert(strcmp(pattern->title, cell_shuttle.title) == 0);
    assert(strcmp( pattern->description, cell_shuttle.description) == 0);
    assert(pattern->cols == cell_shuttle.cols);
    assert(pattern->rows == cell_shuttle.rows);
    assert(pattern->state == PATTERN_FULL);

    printf("%s\n%s\n",pattern->data, cell_shuttle.data);
    assert(strcmp(pattern->data, cell_shuttle.data) == 0);

    DONE();
}

void test_cell_savefile(Pattern *pattern) {
    DESCRIBE("saves pattern and then loads saved pattern, compares values");
    char *outfile = "out/save.queenbeeshuttle.cells";

    int saved = pattern_save_file(outfile, pattern);
    assert(saved >= 0);

    Pattern *check = pattern_allocate_pattern();
    int loaded = pattern_load_file(outfile, check, PATTERN_FULL);
    assert(loaded >= 0);

    assert(strcmp(pattern->title, check->title) == 0);
    assert(strcmp( pattern->description, check->description) == 0);
    assert(pattern->cols == check->cols);
    assert(pattern->rows == check->rows);
    assert(pattern->state == check->state);

    printf("%s\n%s\n", pattern->data, check->data);
    assert(strcmp(pattern->data, check->data) == 0);

    pattern_free_pattern(check);
    DONE();
}
