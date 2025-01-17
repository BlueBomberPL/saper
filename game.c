/*
 *  game.c
 *
 *  Extends 'game.h'.
 * 
 */

#include "game.h"

/* Calls on the exit. */
void _game_on_exit(void)
{
    col_set(COLOR_DEFAULT);
    cls();
    cur_home();
}


/* Starts the game.
 *
 *  settings - the game options
 *  filegrid - file to read the grid from (can be NULL)
 *  filemove - file to read movement from (can be NULL)
 *  seed     - seed value
 */
void game_init(int settings, const char *filegrid, const char *filemove, unsigned int seed)
{
    /* Initializing the modules */
    draw_init(settings);

    /* Setting exit function */
    atexit(_game_on_exit);

    /* Loading move file if provided */
    if(filemove && ! (g_rules.move = fopen(filemove, "r")))
    {
        /* Error */
        draw_label("Nie mozna zaladowac ruchow z pliku, konczenie...", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
        exit(EXIT_FAILURE);
    }

    /* Loading grid from file, if provided */
    if(filegrid && ! (g_rules.grid = grid_load(filegrid)))
    {
        /* Error */
        draw_label("Nie mozna zaladowac planszy z pliku, konczenie...", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
        exit(EXIT_FAILURE);
    }
    else if(filegrid)
    {
        g_rules.diff = OWN;
        g_rules.cols = g_rules.grid->cols;
        g_rules.rows = g_rules.grid->rows;
        g_rules.score = 0;
        g_rules.seed = 0;
        g_rules.mines = 0;

        for(size_t y = 0; y < g_rules.rows; ++y)
            for(size_t x = 0; x < g_rules.cols; ++x)
                if(grid_at(g_rules.grid, x, y)->lo == MINE)
                    ++g_rules.mines;
    }

    while(! filegrid)
    {
        /* Drawing the input and asking for the difficulty */
        char *in = draw_input("Podaj trudnosc (L/N/T/W): ", LOCATION_INPUT_X, LOCATION_INPUT_Y);

        /* Bad input */
        if(! in)
        {
            draw_label("Blad krytyczny, konczenie...", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
            exit(EXIT_FAILURE);
        }

        switch (toupper(in[0]))
        {
            case 'L':
                g_rules.diff = EASY;
                g_rules.rows = 9;
                g_rules.cols = 9;
                g_rules.mines = 10;
                break;
            case 'N':
                g_rules.diff = NORMAL;
                g_rules.rows = 16;
                g_rules.cols = 16;
                g_rules.mines = 40;
                break;
            case 'T':
                g_rules.diff = HARD;
                g_rules.rows = 16;
                g_rules.cols = 30;
                g_rules.mines = 99;
                break;
            case 'W':
                g_rules.diff = OWN;
                break;
            default:
                draw_label("Nieznana wartosc.", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                continue;
        }

        g_rules.state = RUNNING;
        g_rules.score = 0;
        g_rules.seed = seed;

        /* Getting info about own grid */
        if(g_rules.diff == OWN)
        {
            int a, b, c;

            while(true)
            {
                a = atoi(draw_input("Wprowadz ilosc wierszy: ", LOCATION_INPUT_X, LOCATION_INPUT_Y));
                if(a <= 0 || a > GRID_MAX_HEIGHT)
                {
                    draw_label("Nieprawidlowa wartosc.", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                    continue;
                }

                b = atoi(draw_input("Wprowadz ilosc kolumn: ", LOCATION_INPUT_X, LOCATION_INPUT_Y));
                if(b <= 0 || b > GRID_MAX_WIDTH)
                {
                    draw_label("Nieprawidlowa wartosc.", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                    continue;
                }

                c = atoi(draw_input("Wprowadz ilosc min: ", LOCATION_INPUT_X, LOCATION_INPUT_Y));
                if(c <= 0 || c >= a * b)
                {
                    draw_label("Nieprawidlowa wartosc.", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                    continue;
                }
                
                g_rules.rows = a;
                g_rules.cols = b;
                g_rules.mines = c;

                break;
            }


        }

        break;
    }

    /* Printing info */
    {
        char buffer[BUFFER_CHAR_LIMIT] = {0, };
        sprintf(buffer, "Ustawiono: R=%zu  K=%zu  M=%zu.", g_rules.rows, g_rules.cols, g_rules.mines);
        draw_label(buffer, LOCATION_LABEL_X, LOCATION_LABEL_Y, INFOR_WAIT_TIME_S);
    }

    /* Creating the grid */
    if(! filegrid && ! (g_rules.grid = new_grid(g_rules.rows, g_rules.cols, g_rules.mines, g_rules.seed)))
    {
        /* Error */
        draw_label("Blad krytyczny, konczenie...", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
        exit(EXIT_FAILURE);
    }

    g_rules.move = filemove ? g_rules.move : stdin;

    draw_attach(g_rules.grid, LOCATION_GRID_X, LOCATION_GRID_Y);

    /* Entering the loop */
    game_loop();
}

/* The game loop.
 */
void game_loop(void)
{
    /* Alias */
    grid_t *grid = g_rules.grid;

    /* Drawing the grid */
    draw_grid();

    /* THE LOOP */
    while(g_rules.state == RUNNING)
    {
        move_t *move;

        /* User move */
        while(true)
        {
            /* Writing score */
            /* Only if predefined difficulty */
            if(g_rules.diff != OWN)
            {
                char buffer[BUFFER_CHAR_LIMIT];
                sprintf(buffer, "Wynik: %zu", g_rules.score);
                draw_label(buffer, LOCATION_SCORE_X, LOCATION_SCORE_Y, 0);
            }

            /* Getting the input */
            char *in = draw_finput(g_rules.move, "Ruch: ", LOCATION_INPUT_X, LOCATION_INPUT_Y);
            move = game_input(in);

            /* Bad input */
            if(! in || ! move)
            {
                draw_label("Blad krytyczny, konczenie...", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                exit(EXIT_FAILURE);
            }

            /* Input ended (move file) */
            if(g_rules.move && strlen(in) == 0)
            {
                /* Game is still running, so GAME OVER */
                draw_label("Koniec ruchow, przegrana!", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                exit(EXIT_SUCCESS);
            }

            /* Analysing the move */

            if(strstr(in, "exit"))
            {
                /* Exit */
                exit(EXIT_SUCCESS);
            }

            /* Mode */
            if(move->reveal == (size_t) -1)
            {

                if(g_rules.move == stdin)
                    draw_label("Nieznany typ ruchu.", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                else
                {
                    draw_label("Nie mozna odczytac ruchu z pliku, konczenie... [typ]", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                    exit(EXIT_FAILURE);
                }
                continue;
            }


            /* Second coordinate (row, y) */
            {
                if(move->row > grid->rows || move->row == 0)
                {
                    if(g_rules.move == stdin)
                        draw_label("Niewlasciwy indeks wiersza.", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                    else
                    {
                        draw_label("Nie mozna odczytac ruchu z pliku, konczenie... [wiersz]", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                        exit(EXIT_FAILURE);
                    }
                    continue;
                }
            }

            /* Input checking */
            if(move->col > grid->cols || move->col == 0)
            {
                if(g_rules.move == stdin)
                    draw_label("Niewlasciwy indeks kolumny.", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                else
                {
                    draw_label("Nie mozna odczytac ruchu z pliku, konczenie... [kolumna]", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
                    exit(EXIT_FAILURE);
                }
                continue;
            }

            /**********************/

            /* Executing the move */

            /* Reveal and adding points */
            size_t revealed = 0;

            if(move->reveal)
            {
                revealed = grid_reveal(grid, move->col - 1, move->row - 1);

                /* Updating the grid */
                draw_grid();

                /* Valid points */
                if(revealed < (size_t) -1)
                    g_rules.score += revealed * (size_t) g_rules.diff;

                /* Mine - GAME OVER */
                else if(revealed == (size_t) -1)
                {
                    g_rules.state = LOSER;
                    game_end();
                    return;
                }

                /* If all non-mine tiles have been revealed */
                /* GAME WON */
                for(size_t y = 0; y < grid->rows; ++y)
                {
                    for(size_t x = 0; x < grid->cols; ++x)
                    {
                        if(grid_at(grid, x, y)->up == REVEALED ||
                        grid_at(grid, x, y)->lo == MINE)
                        {}
                        else
                            goto END;
                    }
                }  

                /* GAME WON */
                g_rules.state = WINNER;
                game_end();
                return;  

                /* Still goin' */
                END:;
            }

            /* Flag */
            else
            {
                if(grid_at(grid, move->col - 1, move->row - 1)->up == FLAG)
                    grid_at(grid, move->col - 1, move->row - 1)->up = UNREVEALED;

                else if(grid_at(grid, move->col - 1, move->row - 1)->up == UNREVEALED)
                    grid_at(grid, move->col - 1, move->row - 1)->up = FLAG;

                /* Updating the grid */
                draw_grid();
            }
        }   
    }
}

/* Ends the game.
 */
void game_end(void)
{
    /* Message */
    if(g_rules.state == WINNER)
        draw_label("Wygrana!", LOCATION_LABEL_X, LOCATION_LABEL_Y, INFOR_WAIT_TIME_S);
    else
        draw_label("Przegrana!", LOCATION_LABEL_X, LOCATION_LABEL_Y, INFOR_WAIT_TIME_S);

    /* Delete the grid */
    cls();

    /* Ask for the name only if score > 0 */
    /* Save the score */
    if(g_rules.score > 0)
    {
        /* Get the name */
        char *name = draw_input("Wprowadz swoje imie: ", LOCATION_INPUT_X, LOCATION_INPUT_Y);

        if(lead_add(name, g_rules.score))
        {
            /* Oops.. */
            draw_label("Nie mozna zapisac wyniku, konczenie...", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
            exit(EXIT_FAILURE);
        }

        /* Printing top players */
        player_t *list = lead_get(LEADERBOARD_CNT);
        if(! list)
        {
            /* Error... */
            draw_label("Nie mozna zaladowac wynikow, konczenie...", LOCATION_LABEL_X, LOCATION_LABEL_Y, ERROR_WAIT_TIME_S);
            exit(EXIT_FAILURE);
        }

        /* THE LEADERBOARD */
        cur_move(DOWN, 2);
        printf("TOP WYNIKI\n----------------\n");

        size_t pos = 1;
        for(size_t i = 0; pos <= LEADERBOARD_CNT && i < FILE_RECORD_LIMIT; ++i)
        {
            if(! list[i].name || strlen(list[i].name) == 0)
                continue;

            printf("%zu. %s %*zu\n", pos++, list[i].name, 20, list[i].score);
        }
    }

    /* Ending input */
    printf("\nNacisnij Enter aby zakonczyc...");
    fgetc(stdin);

    exit(EXIT_SUCCESS);
}

/* Translates text into move. 
 *
 *  str     - the text
 * 
 * Returns valid pointer if succeeded.
 * If invalid, field will hold (-1) value.
 */
move_t *game_input(char *str)
{
    /* Pointer check */
    assert(str);
    assert(strlen(str) > 0);

    static move_t *move = NULL;

    /* Allocation */
    if(! move && ! (move = (move_t *) malloc(sizeof(move_t))))
    {
        /* Oops */
        return NULL;
    }

    move->col = move->row = move->reveal = (size_t) -1;

    /* All letters to lower */
    for(size_t i = 0; i < strlen(str); ++i)
        str[i] = tolower(str[i]);

    /* Deleting all spaces */
    for(size_t i = 0; i < strlen(str); ++i)
    {
        if(str[i] == ' ')
        {
            for(size_t j = i; j < strlen(str) - 1; ++j)
                str[j] = str[j + 1];
        }
    }

    /* Getting data */

    /* Move type */
    move->reveal = (str[0] == 'r') ? true : (str[0] == 'f') ? false : (size_t) -1;
    str = str + 1;

    if(move->reveal == (size_t) -1)
        return move;

    /* Column (1, 2, 3...) */
    size_t i = 0;
    for(; i < strlen(str); ++i)
    {
        if(! isdigit(str[i]))
            break;
    }

    /* End? */
    if(i == strlen(str) || i == 0)
    {
        return move;
    }

    /* Temporary null */
    char tmp = str[i];
    str[i] = '\0';

    /* Converting column number */
    move->col = (size_t) atoi(str);

    /* Removing the null */
    str[i] = tmp;

    /* Row (a, b, c... z) */
    while(! isalpha(str[i]) && i < strlen(str))
        ++i;

    move->row = isalpha(str[i]) ? (size_t)(str[i] - 'a' + 1) : (size_t) -1;

    return move;
}