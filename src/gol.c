#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include "gol.h"
#include "pattern.h"

int gol_row(int index, int cols) {
    int row = 0;
    row = (int) index / cols;
    return row;
}

int gol_col(int index, int row, int cols) {
    int col = 0;
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
int gol_update_cell(char *data, int index, int cols, int rows) {

    char cell = data[index];
    int health = 0;
    int neighbours[GOL_CLUSTER_SIZE] = {-1};

    col_get_cluster(neighbours, index, cols, rows);

    for (int n = 0; n < GOL_CLUSTER_SIZE; n++) {
        int nindex = neighbours[n];

        if(nindex > -1 && n != 4) {
            health += (data[nindex] == GOL_ALIVE) ? 1 : 0;
        }
    }

    if (cell == GOL_ALIVE) {
        return ((health == 2 || health == 3) ? GOL_ALIVE : GOL_DEAD);
    } else {
        return ((health == 3) ? GOL_ALIVE : GOL_DEAD);
    }

    return cell;
}

/**
 *  A day in world history. Updates world data matrix according to gol rules.
 */
void gol_update(Pattern *world, void (*render_cell)(int cell, int index, int cols, int rows)) {
    int cols = world->cols;
    int rows = world->rows;
    char *data = world->data;

    int size = cols * rows;
    char out[size];
    for (int i = 0; i < size; i++) {
        // update
        int cell = gol_update_cell(data, i, cols, rows);
        // render
        render_cell(cell, i, cols, rows);
        // cache
        out[i] = cell;
        /*
        if((i + 1) % cols == 0) {
            // end of row
        }
        */
    }
    memcpy(data, out, strlen(data));
}

/**
 * Populates world data matrix with a random pattern
 */
void gol_random(Pattern *world) {
    srand(time(NULL));
    int size = world->cols * world->rows;
    char *data = world->data;
    for (int i = 0; i < size; i++) {
       data[i] = (rand() % 2) ? GOL_ALIVE : GOL_DEAD;
    }
}

/**
 * create and allocate a data matrix
 * @param cols width of data matrix
 * @param rows height of data matrix
 */
char *gol_allocate_data(int cols, int rows) {
    // prepare data
    size_t size = (rows * cols) + 1;
    char *data = malloc(size * sizeof(char));

    if (data == NULL) {
        perror("Could not allocate enough memory for data.\n");
        return data;
    }
    // fill data with default cell value (dead)
    memset(data, GOL_DEAD, size - 1);
    data[size] = '\0';
    return data;
}

/**
 * clears a data matrix
 * @param data reference to data matrix pointer (1D char array)
 */
void gol_clear_data(char *data) {
    // prepare data
    memset(data, GOL_DEAD, strlen(data) * sizeof(char));
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
 * @param *src pointer to source Pattern
 * @param *targ pointer to target Pattern
 * @param offsetcols target x-offset, number of columns
 * @param offsetrows target y-offset, number of rows
 */
void gol_merge_data(Pattern *src, Pattern *targ, int offset_cols, int offset_rows) {
    int index = 0;

    int s_rows = src->rows;
    int s_cols = src->cols;
    char *s_data = src->data;

    int t_rows = targ->rows;
    int t_cols = targ->cols;
    char *t_data = targ->data;

    for (int r = 0; r < s_rows; r++) {
        // boundary
        if (offset_rows + r > t_rows) {
            break;
        }
        index = (r + offset_rows) * t_cols + offset_cols;

        for (int c = 0; c < s_cols; c++) {
            // boundary
            if (offset_cols + c > t_cols) {
                break;
            }
            int offset = (r * s_cols) + c;
            t_data[index] = s_data[offset];

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
