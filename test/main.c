// adapted to c: http://headerphile.com/sdl2/sdl2-part-4-making-things-happen/
#include <stdio.h>

#include "utils.h"
#include "gol.h"
#include "paint.h"
#include "state.h"
#include "rle-parser.h"
#include "pattern.h"
#include "init.h"

#define __DOING_TESTS__

Path FS_ROOT_DIR;

int main(int argc, char* argv[]) {

    // root dir, TODO this needs to be made compatible for OS and runtime modes
    init_set_rootdir(argc, argv, FS_ROOT_DIR);
    printf(" => %s\n", FS_ROOT_DIR);
    return 0;
}
