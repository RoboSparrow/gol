#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "test.h"

TestPattern cell_shuttle = {
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

TestPattern cell_48p31 = {
    "48P31",
    "Author: Matthias Merzenich",
    24,
    13,
    PATTERN_FULL,
    ".......OO.O..O.OO......."
    "OO....O..O....O..O....OO"
    "OO.....O.O....O.O.....OO"
    "........O......O........"
    "........................"
    "........................"
    "........................"
    "........................"
    "........................"
    "........O......O........"
    "OO.....O.O....O.O.....OO"
    "OO....O..O....O..O....OO"
    ".......OO.O..O.OO......."
};

//// Tests for CELL parser

void test_cell_loadfile_meta(Pattern *pattern) {
    DESCRIBE("load pattern full");
    extern TestPattern cell_shuttle;

    int loaded = pattern_load_file("./queenbeeshuttle.cells", pattern, PATTERN_META);
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
    extern TestPattern cell_shuttle;

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

// blank lines

void test_cell_loadsave_blanklines(Pattern *pattern) {

    // load

    DESCRIBE("loads pattern fully and compares values");
    extern TestPattern cell_48p31;

    int loaded1 = pattern_load_file("./48p31.cells", pattern, PATTERN_FULL);
    assert(loaded1 >= 0);
    assert(strcmp(pattern->title, cell_48p31.title) == 0);
    assert(strcmp(pattern->description, cell_48p31.description) == 0);
    assert(pattern->cols == cell_48p31.cols);
    assert(pattern->rows == cell_48p31.rows);
    assert(pattern->state == PATTERN_FULL);

    printf("%s\n%s\n",pattern->data, cell_48p31.data);
    assert(strcmp(pattern->data, cell_48p31.data) == 0);

    // save

    DESCRIBE("saves pattern and then loads saved pattern, compares values");
    char *outfile = "out/save.48p31.cells";

    int saved = pattern_save_file(outfile, pattern);
    assert(saved >= 0);

    Pattern *check = pattern_allocate_pattern();
    int loaded2 = pattern_load_file(outfile, check, PATTERN_FULL);
    assert(loaded2 >= 0);

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
