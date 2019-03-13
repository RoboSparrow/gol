/**
 * @see https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
 * Any live cell with fewer than two live neighbors dies, as if by underpopulation.
 * Any live cell with two or three live neighbors lives on to the next generation.
 * Any live cell with more than three live neighbors dies, as if by overpopulation.
 * Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
 */

#define GOL_DEAD '.' /** defines value for a dead cell */
#define GOL_ALIVE 'O' /** defines value for a living cell */

// 0   1   2
// 3  [4]  5
// 6   7   8
#define GOL_CLUSTER_SIZE 9 /** defines size of neighbor cluster array (8) of a cell, including that cell */

void gol_init(char *world, int cols, int rows);
void gol_update(char *world, int cols, int rows);

int gol_col(int index, int row, int cols);
int gol_row(int index, int cols);
