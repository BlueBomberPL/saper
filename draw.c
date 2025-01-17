/*
 *  draw.c
 *
 *  Extends 'draw.h'.
 *
 */

#include "draw.h"

/* Initializes the drawing module.
 * Must be called once, at the beginning.
 */
void draw_init(int settings)
{
    cls();
    cur_home();

    /* Copying the settings. */
    g_settings = settings;

}

/* Sets global grid.
 *
 *  grid    - the grid to be set
 */
void draw_attach(const grid_t *grid, size_t x, size_t y)
{
    /* Pointer checking */
    assert(grid);
    g_grid = grid;

    cur_home();

    g_grid_x = x;
    g_grid_y = y;

    /* "Reserving" terminal area. */
    for(int i = 0; i < (g_grid_y * 4); ++i)
        printf("\n");

    cur_home();
}

/* Draws the grid.
 */
void draw_grid(void)
{
    const size_t tile_width = 4;

    /* Pointer checking */
    assert(g_grid);

    /* Tile width checking */
    assert(tile_width % 2 == 0);

    /* Moving the cursor. */
    cur_to(g_grid_x, g_grid_y);

    /* First stage - column indexes */
    if(g_settings & DRAW_COL_INDEXING)
    {
        printf("\t");
        cur_move(RIGHT, 1);

        /* Less columns than 10: */
        if(g_grid->cols < 10)
        {
            for(size_t x = 0; x < g_grid->cols; ++x)
            {
                printf("%zu", (x + 1));
                for(size_t i = 0; i < tile_width - 1; ++i)
                    printf(" ");
            }

            printf("\n\t");
            cur_move(LEFT, 1);
        }

        /* More columns: */
        else
        {
            /* First row: */
            for(size_t x = 1; x <= g_grid->cols; ++x)
            {
                printf("%c", x >= 10 ? (char)(x / 10) + '0' : ' ');
                for(size_t i = 0; i < tile_width - 1; ++i)
                    printf(" ");
            }

            printf("\n\t");

            /* Left margin */
            for(size_t i = 0; i < tile_width / 2 - 1; ++i)
                printf(" ");

            /* Second row: */
            for(size_t x = 1; x <= g_grid->cols; ++x)
            {
                printf("%zu", x % 10);

                for(size_t i = 0; i < tile_width - 1; ++i)
                    printf(" ");
            }

            printf("\n\t");
            cur_move(LEFT, 1);
        }
    }
    else
    {
        /* No indexing, just a margin */
        printf("\t");
        cur_move(LEFT, 1);
    }


    /* Second stage - upper border */

    const size_t width_char = (g_grid->cols) * tile_width;

    for(size_t i = 0; i <= width_char; ++i)
    {
        if(i == 0)
            printf("%c", CHAR_SG_CORNER_LU);

        else if(i == width_char)
            printf("%c", CHAR_SG_CORNER_RU);

        else if(i > 1 && i < width_char && i % tile_width == 0)
            printf("%c", CHAR_SG_CROSS_U);

        else
           printf("%c", CHAR_SG_HORIZONT);
    }

    printf("\n\t");
    cur_move(LEFT, 1);

    /* Third stage - each row with indexing* */
    for(size_t y = 0; y < g_grid->rows; ++y)
    {
        /* TILE ROW */

        size_t gx = 0;

        for(size_t x = 0; x <= width_char; ++x)
        {
            if(x == 0 || x == width_char || x % tile_width == 0)
                printf("%c", CHAR_SG_VERTICAL);

            /* Drawing the tile */
            else if(x % tile_width == (tile_width / 2))
            {
                char c;

                /* Revealed */
                if(grid_at(g_grid, gx, y)->up == REVEALED)
                {
                    c = (char) grid_at(g_grid, gx, y)->lo;
                    ++gx;
                }

                /* Unrevealed / flagged */
                else
                {
                    c = (char) grid_at(g_grid, gx, y)->up;
                    ++gx;
                }

                /* Color */
                color_t col = COLOR_DEFAULT;

                if(! (g_settings & DRAW_MONO))
                {
                    switch ((lo_layer_t) c)
                    {
                    case D1:
                    case D7:
                        col = CYAN;
                        break;

                    case D2:
                    case D4:
                    case D6:
                        col = GREEN;
                        break;

                    case D3:
                    case D5:
                    case D8:
                        col = RED;
                        break;

                    case FLAG:
                        col = MAGENTA;
                        break;

                    case MINE:
                        col = YELLOW;
                        break;

                    default:
                        break;
                    }
                }

                col_set(col);
                printf("%c", c);
                col_set(COLOR_DEFAULT);
            }

            else
                printf(" ");
        }

        /* Row indexing (if specified) */
        /* Does not work if many letters required */
        if(g_settings & DRAW_ROW_INDEXING && g_grid->rows <= ('z' - 'a'))
        {
            cur_move(RIGHT, 1);

            printf("%c", (char)('a' + y));
        }

        printf("\n\t");
        cur_move(LEFT, 1);

        /* BORDER ROW */

        if(y == g_grid->rows - 1)
            break;

        for(size_t x = 0; x <= width_char; ++x)
        {
            if(x == 0)
                printf("%c", CHAR_SG_CROSS_L);

            else if(x == width_char)
                printf("%c", CHAR_SG_CROSS_R);

            else if(x % tile_width == 0)
                printf("%c", CHAR_SG_CROSS);

            else
                printf("%c", CHAR_SG_HORIZONT);
        }

        printf("\n\t");
        cur_move(LEFT, 1);
    }

    /* Fourth stage - bottom border */

    for(size_t i = 0; i <= width_char; ++i)
    {
        if(i == 0)
            printf("%c", CHAR_SG_CORNER_LD);

        else if(i == width_char)
            printf("%c", CHAR_SG_CORNER_RD);

        else if(i > 1 && i < width_char && i % tile_width == 0)
            printf("%c", CHAR_SG_CROSS_D);

        else
           printf("%c", CHAR_SG_HORIZONT);
    }
}

/* Draws the input module.
 *
 *  comm        - comment, text next to the input
 *  x, y        - location
 *
 * Returns the input if succeeded.
 */
char *draw_input(const char *comm, size_t x, size_t y)
{
    return draw_finput(stdin, comm, x, y);
}

/* Draws the input module with given stream. 
 *
 *  stream      - the stream
 *  comm        - comment, text next to the input
 *  x, y        - location
 * 
 * Returns the input if succeeded.
 */
char *draw_finput(FILE *stream, const char *comm, size_t x, size_t y)
{
    /* Pointer check */
    assert(comm && stream);

    cur_save();

    /* Clearing area */
    cur_to(x, y);
    clr_line();

    static char *input = NULL;

    /* First allocation */
    if(! input && ! (input = (char *) calloc(INPUT_CHAR_LIMIT + 1, sizeof(char))))
    {
        /* Failed */
        return NULL;
    }

    /* EOF? */
    if(stream != stdin && feof(stream))
        return "";

    /* Home position */
    cur_home();
    clr_line();
    printf("%s ", comm);

    /* Input */
    fgets(input, INPUT_CHAR_LIMIT, stream);

    cur_load();

    /* Deleting LF */
    for(size_t i = 0; i < strlen(input); ++i)
        if(input[i] == '\n')
            input[i] = '\0';

    return input;
}

/* Draws text (label).
 *
 * comm         - the text
 * x, y         - location
 * ms           - display time [ms] (0 if forever)
 */
void draw_label(const char *comm, size_t x, size_t y, size_t s)
{
    /* Pointer check */
    assert(comm);

    //cur_save();

    /* Clearing area */
    cur_to(x, y);
    clr_line();

    /* Displaying */
    printf("%s", comm);
    fflush(stdout);
    /* Sleep */
    if(s > 0)
    {
        sleep(s);
        clr_line();
    }

    //cur_load();
}

