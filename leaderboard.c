/*
 *  leaderboard.c
 *
 *  Extends 'leaderboard.h'.
 * 
 */

#include "leaderboard.h"


/* Used for qsort. 
 * DESCENDING
 */
static int _qsort_comp(const void *e1, const void *e2)
{
    const player_t *p1 = (player_t *) e1;
    const player_t *p2 = (player_t *) e2;

    return p2->score - p1->score;
}

/* Adds a player to the list.
 *
 *  name        - player's name
 *  score       - the score
 * 
 *  Returns 0 if succeeded.
 */
int lead_add(const char *name, size_t score)
{
    /* Pointer check */
    assert(name);

    /* The stream */
    FILE *file = fopen(FILE_NAME, "a");

    /* Failed ? */
    if(! file)
        return EXIT_FAILURE;

    /* Adding new player to the end */
    if(fprintf(file, "%s %zu\n", name, score) < 2)
    {
        /* Failed */
        fclose(file);
        return EXIT_FAILURE;
    }

    fclose(file);
    return EXIT_SUCCESS;
}

/* Returns the list of top n players.
 * If the list is too short, missing
 * players will have NULL names.
 *
 *  n           - no of players
 *
 *  Returns valid pointer if succeeded.
 */
player_t    *lead_get(size_t n)
{
    /* Checking */
    assert(n > 0 && n < FILE_RECORD_LIMIT);

    /* Allocating memory for the list */
    player_t *list = NULL;
    if(! (list = (player_t *) calloc(FILE_RECORD_LIMIT, sizeof(player_t))))
    {
        /* Failed */
        return NULL;
    }

    /* Opening the file */
    FILE *file = fopen(FILE_NAME, "r+");

    /* Failed ? */
    if(! file)
    {
        free(list);
        return NULL;
    }

    /* Reading the data */
    size_t i = 0;

    while(! feof(file) && i < FILE_RECORD_LIMIT)
    {
        char buffer[FILE_LINE_LIMIT];
        int c;
        size_t j = 0;

        /* Reading a line */
        while((c = fgetc(file)) != '\n' && c != EOF && j < FILE_LINE_LIMIT - 1)
            buffer[j++] = (char) c;
        buffer[j] = '\0';

        /* Obtaining the player's name */
        /* (first space after read non-blank char.) */
        char *index = buffer;
        int letter_read = false;

        while(index != &buffer[strlen(buffer) - 1])
        {
            if(isalnum(*index))
                letter_read = true;

            if(isspace(*index) && letter_read)
                break;

            ++index;
        }

        /* Placing temporary null to mark the place */
        *index = '\0';

        /* Allocating space for the player's name in struct */
        if(! (list[i].name = (char *) malloc(strlen(buffer) + 2)))
        {
            /* Oops... */
            free(list);
            return NULL;
        }

        /* Copying the name */
        strcpy(list[i].name, buffer);

        /* Removing temporary null */
        *index = ' ';

        /* Pointer to the score part */
        char *score = (index + 1);

        /* Trying to parse the score */
        list[i].score = (size_t) atoi(score);

        ++i;
    }

    fclose(file);

    /* Sorting the list */
    qsort(list, i, sizeof(player_t), _qsort_comp);

    /* Filling missing players */
    while(i < n)
    {
        list[i].name = NULL;
        list[i].score = 0;
        ++i;
    }

    return list;
}