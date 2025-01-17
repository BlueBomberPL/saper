/*
 *  draw.h
 *
 *  Manages drawing a grid in terminal.
 *
 */

#ifndef _SAPER_DRAW_H_FILE_
#define _SAPER_DRAW_H_FILE_

#define DRAW_MONO                 1          /* Black/white                  */
#define DRAW_ROW_INDEXING         (1 << 1)   /* Row indexing (1, 2, 3... )   */
#define DRAW_COL_INDEXING         (1 << 2)   /* Column indexing (a, b, c...) */

#define INPUT_CHAR_LIMIT          32

#include "grid.h"
#include "terminal.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>


/* Global pointer to the grid. Every
 drawing operation is going to involve
 this particular grid object. 
 It is only a shallow copy! 
 */
static const grid_t *g_grid = NULL;

/* The grid's location. 
 */
static size_t g_grid_x = 1;
static size_t g_grid_y = 1;

/* Global settings. */
static int g_settings = 0;

/* Initializes the drawing module. 
 * Must be called once, at the beginning.
 * 
 *  settings    - options (DRAW_* constants)
 */
void        draw_init(int settings);

/* Sets global grid. 
 *  
 *  grid    - the grid to be set
 *  x, y    - the location
 */
void        draw_attach(const grid_t *grid, size_t x, size_t y);

/* Draws the grid. 
 */
void        draw_grid(void);

/* Draws the input module. 
 *
 *  comm        - comment, text next to the input
 *  x, y        - location
 * 
 * Returns the input if succeeded.
 */
char        *draw_input(const char *comm, size_t x, size_t y);

/* Draws the input module with given stream. 
 *
 *  stream      - the stream
 *  comm        - comment, text next to the input
 *  x, y        - location
 * 
 * Returns the input if succeeded.
 */
char        *draw_finput(FILE *stream, const char *comm, size_t x, size_t y);

/* Draws text (label). 
 *
 * comm         - the text
 * x, y         - location
 * ms           - display time [sec] (0 if forever)
 */
void        draw_label(const char *comm, size_t x, size_t y, size_t s);


#endif /* _SAPER_DRAW_H_FILE_ */