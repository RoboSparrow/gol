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
#include "paint.h"

int gol_row(int index, int cols) {
    int row;
    row = (int) index / cols;
    return row;
}

int gol_col(int index, int row, int cols) {
    int col;
    col = index - (row * cols);
    return col;
}

void col_get_cluster(int ns[], int index, int cols, int rows) {

    int row = gol_row(index, cols);
    int col = gol_col(index, row, cols);

    //printf("index %d,col %d, row%d, cols %d, rows %d\n", index, col, row, cols, rows);

    int above = row - 1 > -1;
    int below = row + 1 < rows;
    int left = col - 1 > -1;
    int right = col + 1 < cols;

    //printf("above %d, below %d, left %d, right %d \n", above, below, left, right);

    //  0    1    2    3
    //  4    5    6    7
    //  8    9   10   11
    // 12   13   14   15

    // 0   1   2
    // 3  [4]  5
    // 6   7   8

    ns[0] = (above && left)  ? (index - cols) - 1 : -1;
    ns[1] = (above)          ?  index - cols      : -1;
    ns[2] = (above && right) ? (index - cols) + 1 : -1;

    ns[3] = (left)           ? index - 1 : -1;
    ns[4] =                    index;
    ns[5] = (right)          ? index + 1 : -1;

    ns[6] = (below && left)  ? (index + cols) - 1 : -1;
    ns[7] = (below)          ?  index + cols      : -1;
    ns[8] = (below && right) ? (index + cols) + 1 : -1;

}

int gol_update_cell(int world[], int index, int cols, int rows) {

    int state = world[index];
    int health = 0;
    int neighbours[GOL_CLUSTER_SIZE] = {-1};

    col_get_cluster(neighbours, index, cols, rows);

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

void gol_update(int world[], int cols, int rows) {

    int size = cols * rows;

    paint_clear_canvas(cols, rows);

    int out[size];

    for (int i = 0; i < size; i++) {
        int cell = gol_update_cell(world, i, cols, rows);

        paint_cell(cell, i, cols, rows);

        out[i] = cell;
        if((i + 1) % cols == 0) {
            printf("\n");
        }
    }

    for (int i = 0; i < size; i++) {
       world [i] = out[i];
    }

    fflush(stdout);
}

void gol_init(int world[], int cols, int rows) {
    int size = cols * rows;
    for (int i = 0; i < size; i++) {
       world[i] = rand() % 2;
    }
}

