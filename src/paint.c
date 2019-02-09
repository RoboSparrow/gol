#include <stdio.h>
#include <stdlib.h>

#include "gol.h"

void paint_cell(int cell, int index, int width, int height) {
    if (cell == GOL_ALIVE) {
        printf("\033[07m  \033[m");
    } else {
        printf("  ");
    }
}

void paint_clear_canvas(int width, int height) {
    printf("\033[H\033[J"); // clears the screen in Linux type operating systems
}
