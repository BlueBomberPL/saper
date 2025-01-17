/*
 *  terminal.h
 *
 *  Based on terminal escape codes.
 *  Gives opportunity to move the cursor.
 *  Contains support for basic colors. 
 *  Also manages displaying text (UI).
 * 
 */

#ifndef _SAPER_TERMINAL_H_FILE_
#define _SAPER_TERMINAL_H_FILE_

#ifdef __linux__
    #define CMD_CLEAR       "clear"
#elif _WIN32
    #define CMD_CLEAR       "cls"
#else
    #error "Unknown platform."
#endif

/* 
 *  SG - SinGle line
 */ 

#ifdef _WIN32
    #define CHAR_SG_HORIZONT        196      
    #define CHAR_SG_VERTICAL        179       
    #define CHAR_SG_CROSS           197  
    #define CHAR_SG_CROSS_L         195      
    #define CHAR_SG_CROSS_R         180
    #define CHAR_SG_CROSS_U         194
    #define CHAR_SG_CROSS_D         193 
    #define CHAR_SG_CORNER_LU       218
    #define CHAR_SG_CORNER_LD       192
    #define CHAR_SG_CORNER_RU       191
    #define CHAR_SG_CORNER_RD       217  
#elif __linux__
    #define CHAR_SG_HORIZONT        '-'      
    #define CHAR_SG_VERTICAL        '|'       
    #define CHAR_SG_CROSS           '+'  
    #define CHAR_SG_CROSS_L         '+'      
    #define CHAR_SG_CROSS_R         '+'
    #define CHAR_SG_CROSS_U         '+'
    #define CHAR_SG_CROSS_D         '+' 
    #define CHAR_SG_CORNER_LU       '+'
    #define CHAR_SG_CORNER_LD       '+'
    #define CHAR_SG_CORNER_RU       '+'
    #define CHAR_SG_CORNER_RD       '+'  
#endif

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Basic directions. */
typedef enum _sap_dir_t
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    DIR_NONE

} dir_t;

/* Terminal colors */
typedef enum _sap_color_t
{
    COLOR_DEFAULT = 39,

    BLACK = 30,
    RED = 31,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE

} color_t;


/* Moves the cursor in given direction.
 *
 *  dir         - the direction
 *  x           - no of characters (distance) 
 */
void     cur_move(dir_t dir, size_t x);

/* Moves cursor to given location.
 *
 *  x, y        - the location
 */
void    cur_to(size_t x, size_t y);

/* Saves current cursor position. 
 */
void    cur_save(void);

/* Reverts saved cursor position.
 */
void    cur_load(void);

/* Moves cursor to home (0, 0) position
 */
void    cur_home(void);

/* Clears current line.
 */
void    clr_line(void);

/* Clears the screen. 
 */
void    cls(void);

/* Sets text color. 
 *
 *  color       - the color
 */
void    col_set(color_t color);

/* Writes text in given color.
 * Returns back to the default one.

 *  text        - the text
 *  color       - the color   
 */
void    col_write(const char *text, color_t color);


#endif /* _SAPER_TERMINAL_H_FILE_ */
