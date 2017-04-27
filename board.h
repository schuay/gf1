/*
** $Id: board.h,v 1.9 2000/01/25 19:14:37 kurt Exp $
*/
/*
**    Copyright (C) 1998 Kurt Van den Branden
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  
*/

#ifndef _BOARD_H_
#define _BOARD_H_ 1

#include <stdio.h>
#include <malloc.h>
#include "linklist.h"
#include "position.h"
#include "rem_gipf.h"
#include "rem_row.h"
#include "gamelog.h"
#include "xmlite.h"

typedef struct {
    char pieces[8][9];
    int status;
    char nextpiece;
    char winner;
    char checkfour;
    gamelog * log;
    int movecounter;
    listheader * gipfextra;
    listheader * rowextraw;
    listheader * rowextrab;
    int white;
    int lostwhite;
    int gipfwhite;
    char typewhite;
    int black;
    int lostblack;
    int gipfblack;
    char typeblack;
#ifdef WINGIPF
    int removewait;
    int animate;
#endif
} board;
#define b_newboard()   (board *) malloc (sizeof (board))

/* struct for moves */
typedef struct {
    char from[3];
    char to[3];
    char type;
} fromto;

/* lists with all possible moves */
extern fromto allmovesn[];
extern fromto allmovesg[];

/* all neighbours of board-positions */
extern unsigned char b_buren[9][9][6][2];

/* types of games */
#define T_BASIC 0
#define T_STANDARD 1
#define T_TOURNAMENT 2
#define T_OTHER 3   /* can not be used for b_new !! */

/* possible values for status */
#define S_NORMAL 0
#define S_REMOVEROW 1
#define S_REMOVEGIPF 2
#define S_FINISHED 3

/* some macro's for easy programming */
extern int _colsize[];
#define b_colsize(x)   _colsize[x]

#define b_colnr(x)     x - 'a'

extern char _colchar[];
#define b_colchar(x)   _colchar[x]

#define b_opponent(x)    (x == 'o'? 'x' : 'o')

#define b_otherpiece(x)  (x=='o' ? 'O' : (x=='O' ? 'o' : (x=='x' ? 'X' : 'x')))

#define b_gipf_position(x)   postostr (x->pos)
#define b_gipf_owner(x)      x->owner

#define b_status(x)          x->status
/*#define b_row_extra(x)       x->rowextra*/
#define b_gipf_extra(x)      x->gipfextra
#define b_white(x)           x->white
#define b_white_gipf(x)      x->gipfwhite
#define b_white_lost(x)      x->lostwhite
#define b_black(x)           x->black
#define b_black_gipf(x)      x->gipfblack
#define b_black_lost(x)      x->lostblack
#define b_next_piece(x)      x->nextpiece
#define b_winner(x)          x->winner
#define b_move_counter(x)    x->movecounter

#define b_nolog(x)           x->log = NULL
#define b_setlog(x,y)        x->log = y
#define b_colour(x,y)        (y == 'o' ? x->white : x->black)
#define b_colour_gipf(x,y)   (y == 'o' ? x->gipfwhite : x->gipfblack)
#define b_colour_lost(x,y)   (y == 'o' ? x->lostwhite : x->lostblack)
#define b_colour_type(x,y)   (y == 'o' ? x->typewhite : x->typeblack)

/* this uses a position as second parameter instead of a string in b_piece */
#define b_ppiece(x, y)       x->pieces[y->col][y->row] 

#ifdef WINGIPF
#define setremovewait(x,y)   x->removewait = y
#define setanimate(x,y)      x->animate = y
#endif

/* real functions */
#ifdef __cplusplus
extern "C" {
#endif
board * b_new (int board_type);
board * b_copy (board * orig_board);
void b_print (board * boardp);
void b_del (board * dboard);
board * b_move (board * oboard, char * from, char * to, char npiece);
board * b_remove_row (board * oboard, int rownr);
board * b_remove_gipf (board * oboard, rem_gipf * rgipf);
board * b_checkfour (board * oboard);
rem_row * b_rowoffour (board * oboard, int rownr);
listheader * b_row_extra (board * oboard);

char b_piece (board * oboard, char * strpos);
int b_game_finished (board * oboard);
int b_compare (board * board1, board * board2);
board * b_edit_piece (board * oboard, position * pos, char piece);
board * b_edit_lostwhite (board * oboard, int newval);
board * b_edit_lostblack (board * oboard, int newval);
int b_to_file (board * oboard, FILE * fp);
board * b_from_file (FILE * fp);
xmlite_entity * b_to_xml (board * oboard);
board * b_from_xml (xmlite_entity * root);

#ifdef __cplusplus
}
#endif

#endif
