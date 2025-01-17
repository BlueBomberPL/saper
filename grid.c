/*
 *  grid.c
 *
 *  Extends 'grid.h'.
 *
 */

#include "grid.h"


/* Creates new, randomly filled grid with given settings.
 *
 *  rows    - number of rows
 *  cols    - number of columns
 *  mines   - number of mines
 *  seed    - optional seed, if NULL the time() function will be used
 *
 *  Returns NULL if failed, valid pointer otherwise.
 */
grid_t *new_grid(size_t rows, size_t cols, size_t mines, unsigned int seed)
{
    /* Checking integer values */
    assert(rows > 0 && cols > 0 && mines < rows * cols);

    grid_t *g = NULL;

    /* Memory allocation + checking */
    if((g = (grid_t *) malloc(sizeof(grid_t))) == NULL)
    {
        return NULL;
    }

    g->rows = rows;
    g->cols = cols;

    /* Actual grid allocation + checking */
    if((g->tiles = (tile_t **) malloc(sizeof(tile_t *) * cols)) == NULL)
    {
        free(g);
        return NULL;
    }

    for(size_t x = 0; x < cols; ++x)
    {
        if((g->tiles[x] = (tile_t *) malloc(sizeof(tile_t) * rows)) == NULL)
        {
            free(g->tiles);
            free(g);
            return NULL;
        }

        for(size_t y = 0; y < rows; ++y)
        {
            /* Setting default value */
            grid_at(g, x, y)->up = UNREVEALED;
            grid_at(g, x, y)->lo = D0;
        }
    }

    /* Randomizing mines position */
    srand(seed == 0 ? time(NULL) : seed);

    /* Collection of positions not yet used for mines */
    size_t vpos[rows * cols];
    size_t mines_left = mines;
    {
        for(size_t i = 0; i < rows * cols; ++i)
            vpos[i] = i;
    }

    /* Selecting positions until all mines have been placed */
    while(mines_left > 0)
    {
        /*                       size of the 'vpos'                 */
        /*                       vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
        size_t index = rand() % ((rows * cols) - (mines - mines_left));

        /* Obtaining chosen tile position */
        size_t pos = vpos[index];

        /* Erasing this position from the 'vpos' */
        for(size_t i = index; i < (rows * cols) - (mines - mines_left) - 1u; ++i)
            vpos[i] = vpos[i + 1];

        /* Translating chosen position into (row x column) format */
//        printf("\n[%d/%d]: \t%d\t%d\t\t(%zu)", mines - mines_left, mines, pos % cols, pos / cols, pos);
        tile_t *temp = grid_at(g, pos % cols, pos / cols);

        if(! temp)
            return NULL;

        temp->lo = MINE;

        --mines_left;
    }

    complete_grid(g);
    return g;
}

/* Loads grid from file.
 *
 *  file    - the file name
 */
grid_t *grid_load(const char *filename)
{
    /* Pointer checking */
    assert(filename);

    grid_t *grid = NULL;
    size_t rows = 0;
    size_t cols = 0;

    size_t line_number = 0;

    FILE *file = fopen(filename, "r");
    if(! file)
    {
        /* Oops */
        return NULL;
    }

    while(! feof(file))
    {
        /* First line: rows */
        if(line_number == 0)
        {
            if(fscanf(file, "%zu", &rows) < 1 || rows > GRID_MAX_HEIGHT)
                return NULL;
        }

        /* Second line: cols */
        else if(line_number == 1)
        {
            if(fscanf(file, "%zu", &cols) < 1 || cols > GRID_MAX_WIDTH)
                return NULL;
            
            /* Memory allocation */
            if(!(grid = new_grid(rows, cols, 0, 0)))
            {
                /* Oops */
                return NULL;
            }
        }

        /* Next lines: mine points */
        else
        {
            size_t x, y;
            if(fscanf(file, "%zu %zu", &x, &y) < 2 || x >= cols || y >= rows)
            {
                /* Oops */
                free(grid);
                return NULL;
            }

            grid_at(grid, x, y)->lo = MINE;
        }

        ++line_number;
    }

    fclose(file);

    /* Recalculating */
    complete_grid(grid);

    return grid;
}

/* Completes lower layer of the grid
 * based on the mine placement.
 *
 *  grid     - the grid
 */
void complete_grid(grid_t *grid)
{
    /* Pointer check */
    assert(grid);

    /* Calculations for each tile */
    /* L - Left, R - Right, U - Up, D - Down */
    for(size_t x = 0; x < grid->cols; ++x)
    {
        for(size_t y = 0; y < grid->rows; ++y)
        {
            int tile_val = 0;

            /* Skip if mine */
            if(grid_at(grid, x, y)->lo == MINE)
                continue;

            /* LU */
            if(grid_at(grid, x - 1u, y - 1u) && grid_at(grid, x - 1u, y - 1u)->lo == MINE)
                ++tile_val;

            /* U */
            if(grid_at(grid, x, y - 1u) && grid_at(grid, x, y - 1u)->lo == MINE)
                ++tile_val;

            /* RU */
            if(grid_at(grid, x + 1u, y - 1u) && grid_at(grid, x + 1u, y - 1u)->lo == MINE)
                ++tile_val;

            /* R */
            if(grid_at(grid, x + 1u, y) && grid_at(grid, x + 1u, y)->lo == MINE)
                ++tile_val;

            /* RD */
            if(grid_at(grid, x + 1u, y + 1u) && grid_at(grid, x + 1u, y + 1u)->lo == MINE)
                ++tile_val;

            /* D */
            if(grid_at(grid, x, y + 1u) && grid_at(grid, x, y + 1u)->lo == MINE)
                ++tile_val;

            /* LD */
            if(grid_at(grid, x - 1u, y + 1u) && grid_at(grid, x - 1u, y + 1u)->lo == MINE)
                ++tile_val;

            /* L */
            if(grid_at(grid, x - 1u, y) && grid_at(grid, x - 1u, y)->lo == MINE)
                ++tile_val;

            /* Giving 0 - 8 value */

            if(tile_val == 0)
                grid_at(grid, x, y)->lo = D0;
            else
                grid_at(grid, x, y)->lo = (lo_layer_t) (tile_val + '0');
        }
    }


}

/* Reveals the tiles starting with
 * the specified one
 *
 *  grid    - the grid
 *  x       - the tile's x
 *  y       - the tile's y
 */
size_t grid_reveal(grid_t *grid, size_t x, size_t y)
{
    /* Arguments checking */
    assert(grid);
    assert(grid_at(grid, x, y));

    /* Do not reveal if flagged or already revealed */
    if(grid_at(grid, x, y)->up != UNREVEALED)
        return 0;

    /* IF IT IS 1ST MOVE AND A MINE,
     * CHANGE THE MINE'S POSITION */
    if(grid_at(grid, x, y)->lo == MINE)
    {
        for(size_t y = 0; y < grid->rows; ++y)
            for(size_t x = 0; x < grid->cols; ++x)
                if(grid_at(grid, x, y)->up == REVEALED)
                    goto END;

        /* Yes, it is 1st move */
        /* Choosing next position for the mine */
        for(size_t i = 0; i < grid->rows * grid->cols; ++i)
        {
            tile_t *temp = grid_at(grid, i % grid->cols, i / grid->cols);
            if(! temp || temp->lo == MINE)
                continue;

            temp->lo = MINE;
            break;
        }

        grid_at(grid, x, y)->lo = D0;

        /* Updating the grid */
        complete_grid(grid);

        END:;
    }

    /* Reveal all the mines if a mine was chosen */
    if(grid_at(grid, x, y)->lo == MINE)
    {
        for(size_t y = 0; y < grid->rows; ++y)
        {
            for(size_t x = 0; x < grid->cols; ++x)
            {
                if(grid_at(grid, x, y)->lo == MINE)
                    grid_at(grid, x, y)->up = REVEALED;
            }
        }

        return (size_t) -1;
    }

    /* Reveal all empties around (flood fill) */
    else if(grid_at(grid, x, y)->lo != D0)
    {
        /* Do not flood fill */
        grid_at(grid, x, y)->up = REVEALED;
        return 1;
    }

    /* Flood fill */

    return _grid_reveal_loop(grid, x, y);
}

/* Gives a pointer to a tile at position.
 *
 *  grid    - the grid
 *  x       - x position (column)
 *  y       - y position (row)
 *
 * Returns NULL if failed, valid pointer otherwise.
 */
tile_t *grid_at(const grid_t *grid, size_t x, size_t y)
{
    /* Pointer checking */
    assert(grid);

    /* Bounds checking */
    if(x >= grid->cols || y >= grid->rows)
        return NULL;

    return &(grid->tiles[x][y]);
}

/* Deletes the grid, frees up the memory.
 *
 *  grid    - object to be deleted
 *
 */
void del_grid(grid_t *grid)
{
    if(grid == NULL)
        return;

    for(size_t x = 0u; x < grid->cols; ++x)
        free(grid->tiles[x]);

    free(grid->tiles);
    free(grid);
}


/* Used for grid_reveal.
 * Should not be called explicitly.
 */
size_t _grid_reveal_loop(grid_t *grid, size_t x, size_t y)
{
    assert(grid);

    size_t count_revealed = 0;

    /* Invalid tile */
    if(! grid_at(grid, x, y))
        return 0;

    /* If this tile has been revealed already, stop. */
    /* If this tile has been flagged, stop. */
    if(grid_at(grid, x, y)->up != UNREVEALED)
        return count_revealed;

    /* Reveal this tile */
    grid_at(grid, x, y)->up = REVEALED;
    ++count_revealed;

    /* If this tile is not empty, stop. */
    if(grid_at(grid, x, y)->lo != D0)
        return count_revealed;

    /* Reveal other tiles around */

    if(x > 0)
        count_revealed += _grid_reveal_loop(grid, x - 1, y);

    if(x < grid->cols - 1)
        count_revealed += _grid_reveal_loop(grid, x + 1, y);

    if(y > 0)
        count_revealed += _grid_reveal_loop(grid, x, y - 1);

    if(y < grid->rows - 1)
        count_revealed += _grid_reveal_loop(grid, x, y + 1);

    return count_revealed;
}
