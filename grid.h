/*
 *  grid.h
 *
 *  Everything related with the grid,
 *  which is a 2D collection of tiles.
 *  First index indicates a row, the 2nd one - 
 *  a column.
 * 
 */

#ifndef _SAPER_GRID_H_FILE_
#define _SAPER_GRID_H_FILE_

#ifndef NDEBUG
    #define GRID_MAX_WIDTH          500
    #define GRID_MAX_HEIGHT         500
#else
    #define GRID_MAX_WIDTH          30
    #define GRID_MAX_HEIGHT         20 
#endif


#include "terminal.h"
#include "tile.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* A grid */
typedef struct _sap_grid_t
{
    tile_t **tiles;                         /* Actual grid.             */
    size_t rows;                            /* No. of the grid's rows   */
    size_t cols;                            /* No. of the grid's columns*/

} grid_t;


/* Creates new, randomly filled grid with given settings. 
 *
 *  rows    - number of rows
 *  cols    - number of columns
 *  mines   - number of mines
 *  seed    - optional seed, if 0 the time() function will be used 
 * 
 *  Returns NULL if failed, valid pointer otherwise.
 */
grid_t      *new_grid(size_t rows, size_t cols, size_t mines, unsigned int seed);

/* Loads grid from file.
 *
 *  filename - the file name
 */
grid_t      *grid_load(const char *filename);

/* Completes lower layer of the grid
 * based on the mine placement.
 *
 *  grid     - the grid
 */
void        complete_grid(grid_t *grid);

/* Reveals the tiles starting with 
 * the specified one 
 *
 *  grid    - the grid
 *  x       - the tile's x
 *  y       - the tile's y
 * 
 * Returns number of revealed tiles.
 */
size_t      grid_reveal(grid_t *grid, size_t x, size_t y);

/* Gives a pointer to a tile at position. 
 *
 *  grid    - the grid
 *  x       - x position (column)  
 *  y       - y position (row)
 * 
 * Returns NULL if failed, valid pointer otherwise.
 */
tile_t      *grid_at(const grid_t *grid, size_t x, size_t y);

/* Deletes the grid, frees up the memory.
 *
 *  grid    - object to be deleted
 */
void        del_grid(grid_t *grid);


/* Used for grid_reveal. 
 * Should not be called explicitly.
 * Returns no of revealed tiles.
 */
size_t      _grid_reveal_loop(grid_t *grid, size_t x, size_t y);


#endif /*_SAPER_GRID_H_FILE_ */