/**
 *  Any live cell with fewer than two live neighbors dies, as if by underpopulation.
 *  Any live cell with two or three live neighbors lives on to the next generation.
 *  Any live cell with more than three live neighbors dies, as if by overpopulation.
 *  Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
 */
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "gol.h"

int gol_row(int index, int width) {
    int row;
    row = (int) index / width;
    return row;
}

int gol_col(int index, int row, int width) {
    int col;
    col = index - (row * width);
    return col;
}

void col_get_cluster(int ns[], int index, int width, int height) {

    int row = gol_row(index, width);
    int col = gol_col(index, row, width);

    //printf("index %d,col %d, row%d, width %d, height %d\n", index, col, row, width, height);

    int above = row - 1 > -1;
    int below = row + 1 < height;
    int left = col - 1 > -1;
    int right = col + 1 < width;

    //printf("above %d, below %d, left %d, right %d \n", above, below, left, right);

    //  0    1    2    3
    //  4    5    6    7
    //  8    9   10   11
    // 12   13   14   15

    // 0   1   2
    // 3  [4]  5
    // 6   7   8

    ns[0] = (above && left)  ? (index - width) - 1 : -1;
    ns[1] = (above)          ?  index - width      : -1;
    ns[2] = (above && right) ? (index - width) + 1 : -1;

    ns[3] = (left)           ? index - 1 : -1;
    ns[4] =                    index;
    ns[5] = (right)          ? index + 1 : -1;

    ns[6] = (below && left)  ? (index + width) - 1 : -1;
    ns[7] = (below)          ?  index + width      : -1;
    ns[8] = (below && right) ? (index + width) + 1 : -1;

}

int gol_update_cell(int world[], int index, int width, int height) {

    int state = world[index];
    int health = 0;
    int neighbours[GOL_CLUSTER_SIZE] = {-1};

    col_get_cluster(neighbours, index, width, height);

    for (int n = 0; n < GOL_CLUSTER_SIZE; n++) {
        int nindex = neighbours[n];

        if(nindex > -1 && n != 4) {
            health += world[nindex];
        }
    }

    if (state == GOL_ALIVE) {
        return health == 2 || health == 3;
    } else {
        return health == 3;
    }

    return state;

}

void gol_update(int world[], int width, int height) {

    int size = width * height;

    printf("\033[H"); // clears the screen in Linux type operating systems
    printf("%u\n", (unsigned) time(NULL));

    int out[size];

    for (int i = 0; i < size; i++) {
        int cell = gol_update_cell(world, i, width, height);

        if (cell == GOL_ALIVE) {
            printf("\033[07m  \033[m");
        } else {
            printf("  ");
        }

        out[i] = cell;
        if((i + 1) % width == 0) {
            printf("\n");
        }
    }

    for (int i = 0; i < size; i++) {
       world [i] = out[i];
    }

    fflush(stdout);
}

void gol_init(int world[], int width, int height) {
    int size = width * height;
    for (int i = 0; i < size; i++) {
       world[i] = rand() % 2;
    }
}

