/**
 * Interface for rendering methods
 */

#ifndef __PAINT_H__
#define __PAINT_H__

/**
 * initialize renderer
 */
void paint_init(int cols, int rows);

/**
 * clear renderer
 */
void paint_clear();

/**
 * render start within an update loop
 */
void paint_loop_start(int cols, int rows);

/**
 * render a cell within an update loop
 */
void paint_loop_cell(int cell, int index, int cols, int rows);

/**
 * render a cell within an update loop
 */
void paint_loop_row_end();

/**
 * render end within an update loop
 */
void paint_loop_end(int cols, int rows);

/**
 * exit renderer
 */
void paint_exit(int cols, int rows);

#endif
