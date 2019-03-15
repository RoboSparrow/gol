#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
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

/**
 *  Any live cell with fewer than two live neighbors dies, as if by underpopulation.
 *  Any live cell with two or three live neighbors lives on to the next generation.
 *  Any live cell with more than three live neighbors dies, as if by overpopulation.
 *  Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
 */
int gol_update_cell(char *world, int index, int cols, int rows) {

    char cell = world[index];
    int health = 0;
    int neighbours[GOL_CLUSTER_SIZE] = {-1};

    col_get_cluster(neighbours, index, cols, rows);

    for (int n = 0; n < GOL_CLUSTER_SIZE; n++) {
        int nindex = neighbours[n];

        if(nindex > -1 && n != 4) {
            health += (world[nindex] == GOL_ALIVE) ? 1 : 0;
        }
    }

    if (cell == GOL_ALIVE) {
        return ((health == 2 || health == 3) ? GOL_ALIVE : GOL_DEAD);
    } else {
        return ((health == 3) ? GOL_ALIVE : GOL_DEAD);
    }

    return cell;
}

void gol_update(char *world, int cols, int rows) {
    int size = cols * rows;
    char out[size];

    for (int i = 0; i < size; i++) {
        int cell = gol_update_cell(world, i, cols, rows);
        paint_loop_cell(cell, i, cols, rows);

        out[i] = cell;
        if((i + 1) % cols == 0) {
            paint_loop_row_end();
        }
    }
    memcpy(world, out, strlen(world));
/*
    for (int i = 0; i < size; i++) {
       world [i] = out[i];
    }
*/
}

void gol_init(char *world, int cols, int rows) {
    srand(time(NULL));
    int size = cols * rows;
    for (int i = 0; i < size; i++) {
       world[i] = (rand() % 2) ? GOL_ALIVE : GOL_DEAD;
    }
}

/**
 * allocate memory for a data matrix
 * @param data reference to data matrix pointer (1D char array)
 * @param cols width of data matrix
 * @param rows height of data matrix
 */
void gol_allocate_data(char **data, int cols, int rows) {
    // prepare data
    size_t size = rows * cols * sizeof(char);

    *data = (char *)malloc(size);

    if (*data == NULL) {
        perror("Could not allocate enough memory for data.\n");
        exit(EXIT_FAILURE);
    }
    // fill data with default cell value (dead)
    memset(*data, GOL_DEAD, size);
}

/**
 * free memory for a data matrix
 * @param data reference to data matrix pointer (1D char array)
 */
void gol_free_data(char *data) {
    if (data == NULL) {
        return;
    }
    free(data);
}

/**
 * merge data matrix src into data matrix targ
 * should src offset + width extend the targ with src will be cut off,same applies for targ height
 * psrc can be placed into targ with an x and y offset
 * @param *src pointer to source data matrix (1D char array)
 * @param scols source data matrix width
 * @param srows source data matrix height
 * @param *targ pointer to target data matrix (1D char array)
 * @param tcols target data matrix width
 * @param trows target data matrix height
 * @param offsetcols target x-offset, number of columns
 * @param offsetrows target y-offset, number of rows
 */
void gol_merge_data(
    char *src, int scols, int srows,
    char *targ, int tcols, int trows,
    int offsetcols, int offsetrows
) {
    int index;
    for (int r = 0; r < srows; r++) {
        // boundary
        if (offsetrows + r > trows) {
            printf("row break\n");
            break;
        }
        index = (r + offsetrows) * tcols + offsetcols;

        for (int c = 0; c < scols; c++) {
            // boundary
            if (offsetcols + c > tcols) {
                break;
            }
            int offset = (r * scols) + c;
            targ[index] = src[offset];

            index++;
        }
    }
}

/**
 * prints a data matrix
 * @param data reference to data matrix pointer (1D char array)
 * @param cols width of data matrix
 * @param rows height of data matrix
 */
void gol_print_data(char *data, int cols, int rows) { //TODO (data, rows, cols)
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int offset = (r * cols) + c;
            printf("%c", data[offset]);
        }
        printf("\n");
    }
}
