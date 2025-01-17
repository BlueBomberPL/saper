/*  
 *  leaderboard.h
 *
 *  Reads and writes players' scores.
 *
 */

#ifndef _SAPER_LEADERBOARD_H_FILE_
#define _SAPER_LEADERBOARD_H_FILE_

#define FILE_NAME           ".saper_scores"
#define FILE_LINE_LIMIT     256   
#define FILE_RECORD_LIMIT   32  

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Represents a player.
 */
typedef struct _sap_player
{
    char *name;
    size_t score;

} player_t;


/* Adds a player to the list.
 *
 *  name        - player's name
 *  score       - the score
 * 
 *  Returns 0 if succeeded.
 */
int         lead_add(const char *name, size_t score);

/* Returns the list of top n players.
 * If the list is too short, missing
 * players will have NULL names.
 *
 *  n           - no of players
 *
 *  Returns valid pointer if succeeded.
 */
player_t    *lead_get(size_t n);

#endif /* _SAPER_LEADERBOARD_H_FILE_ */