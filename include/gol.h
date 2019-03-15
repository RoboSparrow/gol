/**
 * definitions for Game of Live data
 */

#define GOL_DEAD '.' /** defines value for a dead cell */
#define GOL_ALIVE 'O' /** defines value for a living cell */

void gol_allocate_data(char **data,  int cols, int rows);
void gol_free_data(char *data);
void gol_print_data(char *data, int cols, int rows);

void gol_merge_data(char *src, int scols, int srows, char *targ, int tcols, int trows, int offsetcols, int offsetrows);

// 0   1   2
// 3  [4]  5
// 6   7   8
#define GOL_CLUSTER_SIZE 9 /** defines size of neighbor cluster array (8) of a cell, including that cell */

void gol_init(char *world, int cols, int rows);
void gol_update(char *world, int cols, int rows);

int gol_col(int index, int row, int cols);
int gol_row(int index, int cols);
