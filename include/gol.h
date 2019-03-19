/**
 * definitions for Game of Live data
 */

#ifndef __GOL_H__
#define __GOL_H__

#include "pattern.h"

#define GOL_DEAD '.' /** defines value for a dead cell */
#define GOL_ALIVE 'O' /** defines value for a living cell */

char *gol_allocate_data(int cols, int rows);
void gol_free_data(char *data);
void gol_clear_data(char *data);
void gol_print_data(char *data, int cols, int rows);

void gol_merge_data(Pattern *src, Pattern *targ, int offset_cols, int offset_rows);

// 0   1   2
// 3  [4]  5
// 6   7   8
#define GOL_CLUSTER_SIZE 9 /** defines size of neighbor cluster array (8) of a cell, including that cell */

void gol_random(Pattern *world);
void gol_update(Pattern *world);

int gol_col(int index, int row, int cols);
int gol_row(int index, int cols);

#endif
