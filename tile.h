/*  
 *  tile.h
 *
 *  Tile is a single square placed on a grid.
 *  Each tile contains two layers, the upper 
 *  one decides if the tile has been revealed, 
 *  flagged or unrevealed.
 *  The lower one contains a 1-8 digit, a mine or
 *  is empty.
 * 
 */

#ifndef _SAPER_TILE_H_FILE_
#define _SAPER_TILE_H_FILE_


/* Upper layer type */
typedef enum _sap_upp_layer_t
{
    UNREVEALED = '#',                       /* Default, as '#'          */
    REVEALED = ' ',                         /* Transparent              */
    FLAG = 'F',                             /* As 'F', cannot be unrev  */

} up_layer_t;

/* Lower layer type */
typedef enum _sap_low_layer_t
{
    D0 = ' ',                               /* Empty                    */                                 
    D1 = '1',                               /* As '1'                   */  
    D2,                                     /* As '2' ...               */  
    D3,
    D4,
    D5,
    D6,
    D7,
    D8,                                     /* As '8'                   */
    MINE = 'M'                              /* As '@'                   */

} lo_layer_t;


/* A tile */
typedef struct _sap_tile_t
{
    up_layer_t up;                          /* Upper layer of the tile  */
    lo_layer_t lo;                          /* Lower layer of the tile  */

} tile_t;


#endif /*_SAPER_TILE_H_FILE_ */