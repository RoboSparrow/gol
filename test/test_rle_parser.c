#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "test.h"

TestPattern rle_shuttle = {
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

//// Tests for RLE parser

void test_rle_loadfile_meta(Pattern *pattern) {
    DESCRIBE("load pattern full");
    extern TestPattern rle_shuttle;

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
    extern TestPattern rle_shuttle;

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

