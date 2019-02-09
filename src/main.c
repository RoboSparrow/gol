#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "gol.h"
#include "paint.h"

int main(int argc, char *argv[])
{
    int cols = 0;
    int rows = 0;

    if (argc > 1) {
        cols = atoi(argv[1]);
    }

    if (argc > 2) {
        rows = atoi(argv[2]);
    }

    if (cols <= 0) {
        cols = 30;
    }

    if (rows <= 0) {
        rows = cols;
    }

    int size = cols * rows;
    int world[size];

    srand(time(NULL));
    gol_init(world, cols, rows);
    paint_init(cols, rows);
    usleep(100000);

    while(1) {
        gol_update(world, cols, rows);
        usleep(100000);
    }

    return 0;
}
