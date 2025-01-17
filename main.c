/*
 *  main.c
 *
 *  Entry point for the program.
 * 
 */

#include "game.h"

#include <stdlib.h>

/* Displays help. */
void help(void)
{
    printf("\n Uzycie:\n\n\t./saper.out <opcjonalne flagi>\n\n");
    printf(" Flagi:\n\n");
    printf(" h           - wyswietla pomoc\n"
           " c           - wylacza obsluge kolorow\n"
           " f <plik>    - korzysta z planszy z pliku\n"
           " r <plik>    - korzysta z pliku ruchow\n"
           " z <wartosc> - ustawia ziarno generatora\n\n");

    exit(EXIT_SUCCESS);
}

/* Analysing options */
void analyse_cmd(int argc, char **argv)
{
    int opt;

    /* Game settings */
    int settings = DRAW_ROW_INDEXING | DRAW_COL_INDEXING;
    int seed = 0;
    char map_name[128];     map_name[0] = '\0';
    char move_name[128];    move_name[0] = '\0';

#if 1
    while((opt = getopt(argc, argv, "hcf:r:z:")) != EOF)
    {
        switch(opt)
        {
            case 'h':
                help();
                exit(EXIT_SUCCESS);

            case 'c':
                settings |= DRAW_MONO;
                break;

            case 'f':
            {
                /* Is the file name valid? */
                if(! strcpy(map_name, optarg) || strlen(map_name) < 1)
                {
                    fprintf(stderr, "-f: Brak nazwy pliku.");
                    exit(EXIT_FAILURE);
                }
                break;
            }

            case 'r':
            {
                /* Is the file name valid? */
                if(! strcpy(move_name, optarg) || strlen(move_name) < 1)
                {
                    fprintf(stderr, "-r: Brak nazwy pliku.");
                    exit(EXIT_FAILURE);
                }
                break;
            }

            case 'z':
                seed = atoi(optarg);
                break;

            case '?':
                if(optopt == 'h' || optopt == 'c')
                    exit(EXIT_FAILURE);

                fprintf(stderr, "-%c: Nieznana flaga.", opt);
                exit(EXIT_FAILURE);
        }
    }
#endif

    /* STARTING THE GAME */
    game_init(settings, 
    (strlen(map_name)) ? map_name : NULL,
    (strlen(move_name)) ? move_name : NULL,
    (unsigned int) seed);

}

int main(int argc, char **argv)
{
    analyse_cmd(argc, argv);
    return EXIT_SUCCESS;
}