/**
 * @see https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
 * Any live cell with fewer than two live neighbors dies, as if by underpopulation.
 * Any live cell with two or three live neighbors lives on to the next generation.
 * Any live cell with more than three live neighbors dies, as if by overpopulation.
 * Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
 */

#define GOL_DEAD 0
#define GOL_ALIVE 1

#define GOL_CLUSTER_SIZE 9

void gol_init(int world[], int cols, int rows);
void gol_update(int world[], int cols, int rows);
