// adapted to c: http://headerphile.com/sdl2/sdl2-part-4-making-things-happen/
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "test.h"

Path FS_ROOT_DIR;

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

    GROUP("CELL parser blank lines");
    pattern = pattern_allocate_pattern();
    test_cell_loadsave_blanklines(pattern);
    pattern_free_pattern(pattern);

    return 0;
}
