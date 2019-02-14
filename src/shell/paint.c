#include <stdio.h>
#include <stdlib.h>

#include "gol.h"

void paint_init() {}

void paint_loop_cell(int cell, int index, int cols, int rows) {
    if (cell == GOL_ALIVE) {
        printf("\033[07m  \033[m");
    } else {
        printf("  ");
    }
}

void paint_loop_row_end() {
    printf("\n");
}

void paint_loop_start(int cols, int rows) {
    printf("\033[H\033[J"); // clears the screen in Linux type operating systems
}

void paint_loop_end(int cols, int rows) {
    fflush(stdout);
}

void paint_exit() {}

