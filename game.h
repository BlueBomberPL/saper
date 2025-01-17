/*
 *  game.h
 *
 *  Manages game life cycle and its status.
 *
 */

#ifndef _SAPER_GAME_H_FILE_
#define _SAPER_GAME_H_FILE_

#define ERROR_WAIT_TIME_S           3
#define INFOR_WAIT_TIME_S           4
#define BUFFER_CHAR_LIMIT           128
#define LEADERBOARD_CNT             5

#define LOCATION_GRID_X             4
#define LOCATION_GRID_Y             5
#define LOCATION_INPUT_X            2
#define LOCATION_INPUT_Y            0
#define LOCATION_SCORE_X            2
#define LOCATION_SCORE_Y            2
#define LOCATION_LABEL_X            2
#define LOCATION_LABEL_Y            3


#include "draw.h"
#include "grid.h"
#include "leaderboard.h"
#include "terminal.h"

#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>


/* Game difficulty.
 * With score multiplier.
 */
typedef enum _sap_difficulty_t
{
    OWN = 0,
    EASY = 1,
    NORMAL = 2,
    HARD = 3

} difficulty_t;

/* Game state. */
typedef enum _sap_gamestate_t
{
    RUNNING,            
    WINNER,
    LOSER

} gamestate_t;

/* Represents the game rules & state. 
 */
typedef struct _sap_rule_t
{
    difficulty_t    diff;
    size_t          rows;
    size_t          cols;
    size_t          mines;

    unsigned long   score;
    unsigned int    seed;
    gamestate_t     state;
    FILE            *move;

    grid_t          *grid;

} gamerule_t;

/* Represents a move. 
 */
typedef struct _sap_move_t
{
    size_t          row;
    size_t          col;
    int             reveal;     /* 0 - flag, 1 - reveal */

} move_t;


/* Global game rules. */
static gamerule_t g_rules;


/* Starts the game.
 *
 *  settings    - the game options
 *  filegrid    - file to read the grid from (can be NULL)
 *  filemove    - file to read movement from (can be NULL)
 *  seed        - seed value
 */
void        game_init(int settings, const char *filegrid, const char *filemove, unsigned int seed);

/* The game loop.
 */
void        game_loop(void);

/* Ends the game.
 */
void        game_end(void);

/* Translates text into move. 
 *
 *  str     - the text
 * 
 * Returns valid pointer if succeeded.
 * If invalid, field will hold (-1) value.
 */
move_t      *game_input(char *str);


#endif /* _SAPER_GAME_H_FILE_ */