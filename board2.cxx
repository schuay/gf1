/*
** board.c
** $Id: board2.cxx,v 1.2 2000/01/25 19:14:46 kurt Exp $
**
** logging will only occur if the log-flag for a board is different from NULL
**
** the original of this file was board.c
** but because I had to add c++ code I had to rename it
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
#include <stdlib.h>
#include <string.h>
#include "board.h"

#undef WINGIPF
#ifdef WINGIPF
extern void waitremrow (board * oldb, rem_row * row, board * newb);
extern void waitremgipf (board * oldb, position * gipf, board * newb);

extern void animatemove (board * oldb, listheader * pieces, board * newb);

typedef struct {
    char piece;
    position from;
    position to;
} piecemove;
#endif

int _colsize[] = {0, 5, 6, 7, 8, 7, 6, 5, 0};

char _colchar[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'};

/* 42 different possible moves, without gipf-pieces */
fromto allmovesn[] = {
	      {"b1","b2",'n'},	      {"a1","b2",'n'},	      {"a2","b2",'n'},
	      {"a2","b3",'n'},	      {"a3","b3",'n'},
	      {"a3","b4",'n'},	      {"a4","b4",'n'},
	      {"a4","b5",'n'},	      {"a5","b5",'n'},	      {"b6","b5",'n'},
	      {"b6","c6",'n'},	      {"c7","c6",'n'},
	      {"c7","d7",'n'},	      {"d8","d7",'n'},
	      {"d8","e8",'n'},	      {"e9","e8",'n'},	      {"f8","e8",'n'},
	      {"f8","f7",'n'},	      {"g7","f7",'n'},
	      {"g7","g6",'n'},	      {"h6","g6",'n'},
	      {"h6","h5",'n'},	      {"i5","h5",'n'},	      {"i4","h5",'n'},
	      {"i4","h4",'n'},	      {"i3","h4",'n'},
	      {"i3","h3",'n'},	      {"i2","h3",'n'},
	      {"i2","h2",'n'},	      {"i1","h2",'n'},	      {"h1","h2",'n'},
	      {"h1","g2",'n'},	      {"g1","g2",'n'},
	      {"g1","f2",'n'},	      {"f1","f2",'n'},
	      {"f1","e2",'n'},	      {"e1","e2",'n'},	      {"d1","e2",'n'},
	      {"d1","d2",'n'},	      {"c1","d2",'n'},
	      {"c1","c2",'n'},	      {"b1","c2",'n'}
};

/* 84 different possible moves, with gipf-pieces */
fromto allmovesg[] = {
	      {"b1","b2",'g'},        {"a1","b2",'g'},	      {"a2","b2",'g'},
	      {"a2","b3",'g'},	      {"a3","b3",'g'},
	      {"a3","b4",'g'},	      {"a4","b4",'g'},
	      {"a4","b5",'g'},	      {"a5","b5",'g'},	      {"b6","b5",'g'},
	      {"b6","c6",'g'},	      {"c7","c6",'g'},
	      {"c7","d7",'g'},	      {"d8","d7",'g'},
	      {"d8","e8",'g'},	      {"e9","e8",'g'},	      {"f8","e8",'g'},
	      {"f8","f7",'g'},	      {"g7","f7",'g'},
	      {"g7","g6",'g'},	      {"h6","g6",'g'},
	      {"h6","h5",'g'},	      {"i5","h5",'g'},	      {"i4","h5",'g'},
	      {"i4","h4",'g'},	      {"i3","h4",'g'},
	      {"i3","h3",'g'},	      {"i2","h3",'g'},
	      {"i2","h2",'g'},	      {"i1","h2",'g'},	      {"h1","h2",'g'},
	      {"h1","g2",'g'},	      {"g1","g2",'g'},
	      {"g1","f2",'g'},	      {"f1","f2",'g'},
	      {"f1","e2",'g'},	      {"e1","e2",'g'},	      {"d1","e2",'g'},
	      {"d1","d2",'g'},	      {"c1","d2",'g'},
	      {"c1","c2",'g'},	      {"b1","c2",'g'},
	      {"b1","b2",'n'},	      {"a1","b2",'n'},	      {"a2","b2",'n'},
	      {"a2","b3",'n'},	      {"a3","b3",'n'},
	      {"a3","b4",'n'},	      {"a4","b4",'n'},
	      {"a4","b5",'n'},	      {"a5","b5",'n'},	      {"b6","b5",'n'},
	      {"b6","c6",'n'},	      {"c7","c6",'n'},
	      {"c7","d7",'n'},	      {"d8","d7",'n'},
	      {"d8","e8",'n'},	      {"e9","e8",'n'},	      {"f8","e8",'n'},
	      {"f8","f7",'n'},	      {"g7","f7",'n'},
	      {"g7","g6",'n'},	      {"h6","g6",'n'},
	      {"h6","h5",'n'},	      {"i5","h5",'n'},	      {"i4","h5",'n'},
	      {"i4","h4",'n'},	      {"i3","h4",'n'},
	      {"i3","h3",'n'},	      {"i2","h3",'n'},
	      {"i2","h2",'n'},	      {"i1","h2",'n'},	      {"h1","h2",'n'},
	      {"h1","g2",'n'},	      {"g1","g2",'n'},
	      {"g1","f2",'n'},	      {"f1","f2",'n'},
	      {"f1","e2",'n'},	      {"e1","e2",'n'},	      {"d1","e2",'n'},
	      {"d1","d2",'n'},	      {"c1","d2",'n'},
	      {"c1","c2",'n'},	      {"b1","c2",'n'}
};


/*
** list of all the neighbours of a board-position
**
**                         3
**                    1         5
**                        pos
**                    0         4
**                         2
*/
unsigned char b_buren[9][9][6][2] = {
  {{{0}}/* row a */}, 
  {/* row b */
    {{0}},
    {{0}},
    {{0, 0}, {0, 0}, {0, 0}, {1, 3}, {2, 2}, {2, 3}}, /* b2 */
    {{0, 0}, {0, 0}, {1, 2}, {1, 4}, {2, 3}, {2, 4}}, /* b3 */
    {{0, 0}, {0, 0}, {1, 3}, {1, 5}, {2, 4}, {2, 5}}, /* b4 */
    {{0, 0}, {0, 0}, {1, 4}, {0, 0}, {2, 5}, {2, 6}}  /* b5 */
  },
  {/* row c */
    {{0}},
    {{0}},
    {{0, 0}, {1, 2}, {0, 0}, {2, 3}, {3, 2}, {3, 3}}, /* c2 */
    {{1, 2}, {1, 3}, {2, 2}, {2, 4}, {3, 3}, {3, 4}}, /* c3 */
    {{1, 3}, {1, 4}, {2, 3}, {2, 5}, {3, 4}, {3, 5}}, /* c4 */
    {{1, 4}, {1, 5}, {2, 4}, {2, 6}, {3, 5}, {3, 6}}, /* c5 */
    {{1, 5}, {0, 0}, {2, 5}, {0, 0}, {3, 6}, {3, 7}}  /* c6 */
  },
  {/* row d */
    {{0}},
    {{0}},
    {{0, 0}, {2, 2}, {0, 0}, {3, 3}, {4, 2}, {4, 3}}, /* d2 */
    {{2, 2}, {2, 3}, {3, 2}, {3, 4}, {4, 3}, {4, 4}}, /* d3 */
    {{2, 3}, {2, 4}, {3, 3}, {3, 5}, {4, 4}, {4, 5}}, /* d4 */
    {{2, 4}, {2, 5}, {3, 4}, {3, 6}, {4, 5}, {4, 6}}, /* d5 */
    {{2, 5}, {2, 6}, {3, 5}, {3, 7}, {4, 6}, {4, 7}}, /* d6 */
    {{2, 6}, {0, 0}, {3, 6}, {0, 0}, {4, 7}, {4, 8}}  /* d7 */
  },
  {/* row e */
    {{0}},
    {{0}},
    {{0, 0}, {3, 2}, {0, 0}, {4, 3}, {0, 0}, {5, 2}}, /* e2 */
    {{3, 2}, {3, 3}, {4, 2}, {4, 4}, {5, 2}, {5, 3}}, /* e3 */
    {{3, 3}, {3, 4}, {4, 3}, {4, 5}, {5, 3}, {5, 4}}, /* e4 */
    {{3, 4}, {3, 5}, {4, 4}, {4, 6}, {5, 4}, {5, 5}}, /* e5 */
    {{3, 5}, {3, 6}, {4, 5}, {4, 7}, {5, 5}, {5, 6}}, /* e6 */
    {{3, 6}, {3, 7}, {4, 6}, {4, 8}, {5, 6}, {5, 7}}, /* e7 */
    {{3, 7}, {0, 0}, {4, 7}, {0, 0}, {5, 7}, {0, 0}}  /* e8 */
  },
  {/* row f */
    {{0}},
    {{0}},
    {{4, 2}, {4, 3}, {0, 0}, {5, 3}, {0, 0}, {6, 2}}, /* f2 */
    {{4, 3}, {4, 4}, {5, 2}, {5, 4}, {6, 2}, {6, 3}}, /* f3 */
    {{4, 4}, {4, 5}, {5, 3}, {5, 5}, {6, 3}, {6, 4}}, /* f4 */
    {{4, 5}, {4, 6}, {5, 4}, {5, 6}, {6, 4}, {6, 5}}, /* f5 */
    {{4, 6}, {4, 7}, {5, 5}, {5, 7}, {6, 5}, {6, 6}}, /* f6 */
    {{4, 7}, {4, 8}, {5, 6}, {0, 0}, {6, 6}, {0, 0}}  /* f7 */
  },
  {/* row g */
    {{0}},
    {{0}},
    {{5, 2}, {5, 3}, {0, 0}, {6, 3}, {0, 0}, {7, 2}}, /* g2 */
    {{5, 3}, {5, 4}, {6, 2}, {6, 4}, {7, 2}, {7, 3}}, /* g3 */
    {{5, 4}, {5, 5}, {6, 3}, {6, 5}, {7, 3}, {7, 4}}, /* g4 */
    {{5, 5}, {5, 6}, {6, 4}, {6, 6}, {7, 4}, {7, 5}}, /* g5 */
    {{5, 6}, {5, 7}, {6, 5}, {0, 0}, {7, 5}, {0, 0}}  /* g6 */
  },
  {/* row h */
    {{0}},
    {{0}},
    {{6, 2}, {6, 3}, {0, 0}, {7, 3}, {0, 0}, {0, 0}}, /* h2 */
    {{6, 3}, {6, 4}, {7, 2}, {7, 4}, {0, 0}, {0, 0}}, /* h3 */
    {{6, 4}, {6, 5}, {7, 3}, {7, 5}, {0, 0}, {0, 0}}, /* h4 */
    {{6, 5}, {6, 6}, {7, 4}, {0, 0}, {0, 0}, {0, 0}}  /* h5 */
  },
  {{{0}}/* row i */} 
};


/*
** each item of the list contains:
**   - middle point for the row
**   - first direction for this row (second direction = 5 - first direction)
**   - second direction
**   - minimum nr of pieces necessary in the first direction
*/
int rowlist[][5] = {
	    {2, 2, 1, 4, 2},
	    {3, 3, 1, 4, 2},
	    {4, 4, 1, 4, 2},
	    {4, 5, 1, 4, 1},
	    {5, 5, 1, 4, 2},
	    {5, 6, 1, 4, 2},
	    {5, 7, 1, 4, 2},
	    {1, 3, 2, 3, 2},
	    {2, 4, 2, 3, 2},
	    {3, 5, 2, 3, 2},
	    {4, 5, 2, 3, 1},
	    {5, 5, 2, 3, 2},
	    {6, 4, 2, 3, 2},
	    {7, 3, 2, 3, 2},
	    {5, 2, 0, 5, 2},
	    {5, 3, 0, 5, 2},
	    {5, 4, 0, 5, 2},
	    {4, 5, 0, 5, 1},
	    {4, 6, 0, 5, 2},
	    {3, 6, 0, 5, 2},
	    {2, 6, 0, 5, 2}
	    };



board * b_new (int board_type)
{
    board * board_ref;
    int i, j;

    board_ref = b_newboard();
    for (i = 0; i < 8; i++)
	for (j = 0; j < 9; j++)
	    board_ref->pieces[i][j] = '.';

    board_ref->status = S_NORMAL;
    board_ref->nextpiece = 'o';
    board_ref->gipfextra = NULL;
    board_ref->rowextraw = NULL;
    board_ref->rowextrab = NULL;
    board_ref->white = 18;
    board_ref->lostwhite = 0;
    board_ref->gipfwhite = 0;
    board_ref->typewhite = 'n';
    board_ref->black = 18;
    board_ref->lostblack = 0;
    board_ref->gipfblack = 0;
    board_ref->typeblack = 'n';
    board_ref->winner = '.';
    board_ref->checkfour = 'n';
    board_ref->log = NULL;
    board_ref->movecounter = 0;
#ifdef WINGIPF
    board_ref->removewait = 0;
    board_ref->animate = 0;
#endif

    if (board_type == T_BASIC)
    {
	board_ref->white = 12;
	board_ref->black = 12;
	board_ref->pieces[1][5] = 'o';
	board_ref->pieces[7][5] = 'o';
	board_ref->pieces[4][2] = 'o';
	board_ref->pieces[1][2] = 'x';
	board_ref->pieces[4][8] = 'x';
	board_ref->pieces[7][2] = 'x';
	board_ref->gipfwhite = -1;
	board_ref->gipfblack = -1;
    }
    else if (board_type == T_STANDARD)
    {
	board_ref->white = 12;
	board_ref->black = 12;
	board_ref->pieces[1][5] = 'O';
	board_ref->pieces[7][5] = 'O';
	board_ref->pieces[4][2] = 'O';
	board_ref->pieces[1][2] = 'X';
	board_ref->pieces[4][8] = 'X';
	board_ref->pieces[7][2] = 'X';
	board_ref->gipfwhite = 3;
	board_ref->gipfblack = 3;
    }
    else
    {   /* tournament */
	board_ref->typewhite = 'g';
	board_ref->typeblack = 'g';
    }

    return (board_ref);
}

board * b_copy (board * orig_board)
{
    board * new_board;
    int i, j;

    if (orig_board == NULL)
	return (NULL);
    
    new_board = b_newboard();

#define MEMCPY 1
#ifndef MEMCPY
    for (i = 1; i < 8; i++)
	for (j = 2; j <= b_colsize (i); j++)
	    new_board->pieces[i][j] = orig_board->pieces[i][j];

    new_board->status = orig_board->status;
    new_board->nextpiece = orig_board->nextpiece;
    new_board->winner = orig_board->winner;
    new_board->checkfour = orig_board->checkfour;
    new_board->log = orig_board->log;
    new_board->movecounter = orig_board->movecounter;

    new_board->white = orig_board->white;
    new_board->typewhite = orig_board->typewhite;
    new_board->lostwhite = orig_board->lostwhite;
    new_board->gipfwhite = orig_board->gipfwhite;
    new_board->black = orig_board->black;
    new_board->typeblack = orig_board->typeblack;
    new_board->lostblack = orig_board->lostblack;
    new_board->gipfblack = orig_board->gipfblack;
#else
    memcpy (new_board, orig_board, sizeof (board));
#endif

    if (orig_board->gipfextra != NULL)
    {
	new_board->gipfextra = copy_rem_gipf_row (orig_board->gipfextra);
    }
    if (orig_board->rowextraw != NULL)
    {
	new_board->rowextraw = copy_rem_row_row (orig_board->rowextraw);
    }
    if (orig_board->rowextrab != NULL)
    {
	new_board->rowextrab = copy_rem_row_row (orig_board->rowextrab);
    }
    return (new_board);
}


void b_print (board * boardp)
{
#ifndef WINGIPF
    if (boardp == NULL)
    {
	printf ("\nERROR: the boardp-pointer is NULL (b_print)\n\n");
	return;
    }

    printf ("A   B   C   D   E   F   G   H   I\n");
    printf ("5   6   7   8   9   8   7   6   5\n\n");
    printf ("                *\n");
    printf ("            *       *\n");
    printf ("        *       %c       *\n", boardp->pieces[4][8]);
    printf ("    *       %c       %c       *\n", 
	     boardp->pieces[3][7], boardp->pieces[5][7]);
    printf ("*       %c       %c       %c       *\n", 
	     boardp->pieces[2][6], boardp->pieces[4][7], boardp->pieces[6][6]);
    printf ("    %c       %c       %c       %c\n", 
	     boardp->pieces[1][5], boardp->pieces[3][6], 
	     boardp->pieces[5][6], boardp->pieces[7][5]);
    printf ("*       %c       %c       %c       *\n", 
	     boardp->pieces[2][5], boardp->pieces[4][6], boardp->pieces[6][5]);
    printf ("    %c       %c       %c       %c\n", 
	     boardp->pieces[1][4], boardp->pieces[3][5], 
	     boardp->pieces[5][5], boardp->pieces[7][4]);
    printf ("*       %c       %c       %c       *\n", 
	    boardp->pieces[2][4], boardp->pieces[4][5], boardp->pieces[6][4]);
    printf ("    %c       %c       %c       %c\n", 
	     boardp->pieces[1][3], boardp->pieces[3][4], 
	     boardp->pieces[5][4], boardp->pieces[7][3]);
    printf ("*       %c       %c       %c       *\n", 
	     boardp->pieces[2][3], boardp->pieces[4][4], boardp->pieces[6][3]);
    printf ("    %c       %c       %c       %c\n", 
	     boardp->pieces[1][2], boardp->pieces[3][3], 
	     boardp->pieces[5][3], boardp->pieces[7][2]);
    printf ("*       %c       %c       %c       *\n", 
	     boardp->pieces[2][2], boardp->pieces[4][3], boardp->pieces[6][2]);
    printf ("    *       %c       %c       *\n", 
	     boardp->pieces[3][2], boardp->pieces[5][2]);
    printf ("        *       %c       *\n", boardp->pieces[4][2]);
    printf ("            *       *\n");
    printf ("                *                        white: %d\n", 
	    boardp->white);
    printf ("                                     lostwhite: %d\n", 
	    boardp->lostwhite);
    printf ("1   1   1   1   1   1   1   1   1        black: %d\n", 
	    boardp->black);
    printf ("A   B   C   D   E   F   G   H   I    lostblack: %d\n", 
	    boardp->lostblack);
#endif
    return;
}


void b_del (board * dboard)
{
    if (dboard != NULL)
    {
	if (dboard->gipfextra != NULL)
	{
	    emptyll (dboard->gipfextra, del_rem_gipf);
	    free (dboard->gipfextra);
	}
	if (dboard->rowextraw != NULL)
	{
	    emptyll (dboard->rowextraw, del_rem_row);
	    free (dboard->rowextraw);
	}
	if (dboard->rowextrab != NULL)
	{
	    emptyll (dboard->rowextrab, del_rem_row);
	    free (dboard->rowextrab);
	}
	free (dboard);
    }

    return;
}


/*
** b_move : move a piece on a gipf-board
**
** parameters:
**      oboard : pointer to original board
**      from : string with from-position
**      to   : string with to-position
**      npiece: piece to add to the board (o, O, x, X)
**
** returns:
**      pointer to a new board
**      NULL
*/
board * b_move (board * oboard, char * from, char * to, char npiece)
{
    position * from_p,
	* to_p,
	* next_p;
    int row_dif,
	col_dif;
    board * nboard,
	* new_board;
    char save_piece,
	piece,
	tempstr[80],
	* lastmoved = NULL;
#ifdef WINGIPF
    listheader * pmoves;
    piecemove * piecem;
#endif
    
    /* check if oboard is in the correct state */
    if (oboard->status == S_REMOVEROW)
    {   /* wrong status */
	return (NULL);
    }
    if ((npiece != oboard->nextpiece) && 
	(b_otherpiece (npiece) != oboard->nextpiece))
    {   /* wrong player */
	return (NULL);
    }

    /* check if the player has pieces left */
    if ((npiece == 'o') || (npiece == 'O'))
    {
	if (oboard->white <= 0)
	{
	    return (NULL);
	}
    }
    else if (oboard->black <= 0)
    {
	return (NULL);
    }

    /* if the player wants to place a gipf, check if this is allowed */
    if ((npiece == 'O') && (oboard->typewhite != 'g'))
    {
	return (NULL);
    }
    else if ((npiece == 'X') && (oboard->typeblack != 'g'))
    {
	return (NULL);
    }

    /* move the piece */
    from_p = strtopos (from);
    to_p = strtopos (to);
    if ((from_p == NULL) || (to_p == NULL))
    {   /* invalid from or to position */
	del_position (from_p);
	del_position (to_p);
	return (NULL);
    }
    next_p = (position *) copy_position ((void *) to_p);
    row_dif = to_p->row - from_p->row;
    col_dif = to_p->col - from_p->col;

    /* valid to-position ? */
    if (((to_p->col < 1) || (to_p->col > 7)) ||
	((to_p->col != 1) && (to_p->col != 7) &&
	 (to_p->row != 2) && (to_p->row != b_colsize (to_p->col))) ||
	(((to_p->col == 1) | (to_p->col == 7)) &&
	 ((to_p->row <2) || (to_p->row >5))))
    {
#ifndef WINGIPF
	printf ("invalid move (to): (%s, %s)\n", from, to);
#endif
	del_position (from_p);
	del_position (to_p);
	del_position (next_p);
	return (NULL);
    }

    /* valid from-position ? */
    if ((abs (row_dif) > 1) ||
	(abs (col_dif) > 1) ||
	(((from_p->col == 0) || (from_p->col == 8)) && (row_dif < 0)) ||
	(( from_p->row == 1) && (((from_p->col < 5) && (col_dif < 0)) ||
				 ((from_p->col > 3) && (col_dif > 0)))) ||
	((from_p->col > 0) && (from_p->col < 8) &&
	 (from_p->row != 1) && (from_p->row != b_colsize (from_p->col) + 1)))
    {
#ifndef WINGIPF
	printf ("invalid move (from): (%s, %s)\n", from, to);
#endif
	del_position (from_p);
	del_position (to_p);
	del_position (next_p);
	return (NULL);
    }

    nboard = b_copy (oboard);
    if (nboard->gipfextra != NULL)
    {
	emptyll (nboard->gipfextra, del_rem_gipf);
	free (nboard->gipfextra);
	nboard->gipfextra = NULL;
    }

    nboard->status = S_NORMAL;
    if (oboard->nextpiece == 'o')
    {
	nboard->movecounter++;
    }

#ifdef WINGIPF
    pmoves = (listheader *) malloc (sizeof (listheader));
    newlist (pmoves);

    piecem = (piecemove *) malloc (sizeof (piecemove));
    piecem->piece = npiece;
    piecem->from.col = from_p->col;
    piecem->from.row = from_p->row;
    piecem->to.col = to_p->col;
    piecem->to.row = to_p->row;
    pushll (pmoves, (void *) piecem);
#endif
    
    save_piece = nboard->pieces[to_p->col][to_p->row];
    nboard->pieces[to_p->col][to_p->row] = npiece;
    while (save_piece != '.')
    {
#ifdef WINGIPF
	piecem = (piecemove *) malloc (sizeof (piecemove));
	piecem->piece = save_piece;
	piecem->from.col = next_p->col;
	piecem->from.row = next_p->row;
#endif
	if (col_dif != 0)
	{   /* going diagonally */
	    if (next_p->col == 4)
	    {   /* going over the middle line */
		row_dif--;
	    }
	    next_p->col += col_dif;
	}
	next_p->row += row_dif;
	if ((next_p->col == 0) ||
	    (next_p->col == 8) ||
	    (next_p->row < 2) ||
	    (next_p->row > b_colsize (next_p->col)))
	{   /* the row is full, no pieces can be added */
	    del_position (from_p);
	    del_position (next_p);
	    del_position (to_p);
	    b_del (nboard);
#ifdef WINGIPF
	    free (piecem);
	    while ((piecem = (piecemove *) llrembynr (pmoves, 1)) != NULL)
	    {
		free (piecem);
	    }
	    free (pmoves);
#endif
	    return (NULL);
	}
#ifdef WINGIPF
	piecem->to.col = next_p->col;
	piecem->to.row = next_p->row;
	pushll (pmoves, (void *) piecem);
#endif
	piece = save_piece;
	save_piece = nboard->pieces[next_p->col][next_p->row];
	nboard->pieces[next_p->col][next_p->row] = piece;
    }
    lastmoved = postostr (next_p); /* just for logging-purposes */
    
    if (npiece == 'o')
    {
	nboard->white -= 1;
	nboard->typewhite = 'n';
    }
    else if (npiece == 'O')
    {
	nboard->white -= 2;
	nboard->gipfwhite++;
    }
    else if (npiece == 'x')
    {
	nboard->black -= 1;
	nboard->typeblack = 'n';
    }
    else
    {
	nboard->black -= 2;
	nboard->gipfblack++;
    }
    nboard->nextpiece = b_opponent (nboard->nextpiece);

    if (nboard->log)
    {
	sprintf (tempstr, "%c:%2s:%2s", npiece, from, lastmoved);
	addtolog (nboard->log, LOGMOVE, tempstr);
    }
    free (lastmoved);

#ifdef WINGIPF
    /* animate move */
    if (nboard->animate)
    {
	animatemove (oboard, pmoves, nboard);
    }
    
    while ((piecem = (piecemove *) llrembynr (pmoves, 1)) != NULL)
    {
	free (piecem);
    }
    free (pmoves);
#endif

    /* check for 4 in a row */
    nboard->checkfour = 'y';
    new_board = b_checkfour (nboard);
 
    /* cleanup memory */
    del_position (from_p);
    del_position (next_p);
    del_position (to_p);
    b_del (nboard);

    return (new_board);
}


/*
** b_remove_row: remove a row from the playing field, adapt the different 
**               counters
**
** parameters:
**    oboard : pointer to gipf-board
**    rownr : nr of the row to be removed from gipfextra
**
** returns:
**    pointer to a new gipf-board
**    NULL
*/
board * b_remove_row (board * oboard, int rownr)
{
    rem_row * row_ptr,
	* n_row;
    board * nboard,
	* new_board;
    listheader * glist,
	* plist,
	* rowextra_o,
	* rowextra_n;
    int nrg = 0,
	counter,
	str_offset;
    position * coor;
    char winpiece,
	piece,
	colour,
	tempstr[80],
	* strpos,
	* start,
	* end;
    rem_gipf * gipf_ptr;

    if ((oboard->rowextraw != NULL) && 
	((oboard->nextpiece == 'x') || (oboard->rowextrab == NULL)))
    {
	colour = 'o';
	rowextra_o = oboard->rowextraw;
    } 
    else if (oboard->rowextrab != NULL)
    {
	colour = 'x';
	rowextra_o = oboard->rowextrab;
    }
    else
    {
	return (NULL);
    }

    row_ptr = (rem_row *) llitembynr (rowextra_o, rownr);
    /* this shouldn't happen */
    if (row_ptr == NULL)
    {
	return (NULL);
    }

    glist = (listheader *) malloc (sizeof (listheader));
    newlist (glist);
    nboard = b_copy (oboard);
    if (nboard->gipfextra != NULL)
    {
	emptyll (nboard->gipfextra, del_rem_gipf);
	free (nboard->gipfextra);
	nboard->gipfextra = NULL;
    }

    nboard->status = S_NORMAL;
    plist = row_ptr->piecelist;
    winpiece = row_owner (row_ptr);

    if (nboard->log)
    {
	start = postostr (row_start (row_ptr));
	end = postostr (row_end (row_ptr));
	sprintf (tempstr, "%c:%s:%s:", winpiece, start, end);
	free (start);
	free (end);
	str_offset = 8;
    }

    counter = 1;
    while ((coor = (position *) llitembynr (plist, counter)) != NULL)
    {
	counter++;

	piece = nboard->pieces[coor->col][coor->row];
	if ((piece == 'O') || (piece == 'X'))
	{
	    nrg++;
	    gipf_ptr = new_rem_gipf ();
 	    gipf_ptr->pos = (position *) copy_position ((void *) coor);
	    gipf_ptr->owner = winpiece;
	    pushll (glist, (void *) gipf_ptr);
	    continue;
	}
	
	if (nboard->log)
	{
	    strpos = postostr (coor);
	    sprintf (tempstr + str_offset, "%2s%c:", strpos, piece);
	    free (strpos);
	    str_offset += 4;
	}

	if (piece == 'o')
	{
	    if (piece == winpiece)
	    {
		nboard->white++;
	    }
	    else
	    {
		nboard->lostwhite++;
	    }
	}
	else
	{
	    if (piece == winpiece)
	    {
		nboard->black++;
	    }
	    else
	    {
		nboard->lostblack++;
	    }
	}

	nboard->pieces[coor->col][coor->row] = '.';
    }

    if (nboard->log)
    {
	addtolog (nboard->log, LOGREMROW, tempstr);
    }

#ifdef WINGIPF
    if (nboard->removewait)
    {
	waitremrow (oboard, row_ptr, nboard);
    }
#endif
    
    /*
    ** remove the row that was just removed from rowextra
    ** and check if the other rows are still complete
    */
    /* check white rowlist */
    if (nboard->rowextraw != NULL)
    {
	/* check if all rows are still valid */
	emptyll (nboard->rowextraw, del_rem_row);/* I now have an empty list */

	counter = 1;
	while ((row_ptr = (rem_row *) llitembynr (oboard->rowextraw, counter)) 
	       != NULL)
	{
	    counter++;
	    if ((colour == 'o') && ((counter - 1) == rownr))
	    {
		continue;
	    }
	    if ((n_row = b_rowoffour (nboard, row_ptr->rowindex)) != NULL)
	    {
		pushll (nboard->rowextraw, n_row);
	    }
	}

	if (llitembynr (nboard->rowextraw, 1) == NULL)
	{
	    free (nboard->rowextraw);
	    nboard->rowextraw = NULL;
	}
    }

    /* check black rowlist */
    if (nboard->rowextrab != NULL)
    {
	/* check if all rows are still valid */

	emptyll (nboard->rowextrab, del_rem_row);/* I now have an empty list */

	counter = 1;
	while ((row_ptr = (rem_row *) llitembynr (oboard->rowextrab, counter)) 
	       != NULL)
	{
	    counter++;
	    if ((colour == 'x') && ((counter - 1) == rownr))
	    {
		continue;
	    }
	    if ((n_row = b_rowoffour (nboard, row_ptr->rowindex)) != NULL)
	    {
		pushll (nboard->rowextrab, n_row);
	    }
	}

	if (llitembynr (nboard->rowextrab, 1) == NULL)
	{
	    free (nboard->rowextrab);
	    nboard->rowextrab = NULL;
	}
    }

    if ((nboard->rowextraw == NULL) && (nboard->rowextrab == NULL))
    {
	nboard->checkfour = 'n';
    }

    if (nrg != 0)
    {
	nboard->gipfextra = glist;
	nboard->status = S_REMOVEGIPF;
	return (nboard);
    }
    else if (nboard->checkfour == 'y')
    {
	free (glist);
	new_board = b_checkfour (nboard);
	b_del (nboard);
	return (new_board);
    }
    else
    {
	free (glist);
	return (nboard);
    }
}


/*
** remove a gipf from the board and add the retrieved pieces to the
** correct counter
**
** parameters:
**    oboard: pointer to gipf-board
**
** returns:
**    pointer to new board
**    NULL
*/
board * b_remove_gipf (board * oboard, rem_gipf * rgipf)
{
    board * nboard;
    char piece,
	winpiece,
	tempstr[80],
	* strpos;
    rem_row * row_ptr,
	* n_row;
    int counter;

    piece = oboard->pieces[rgipf->pos->col][rgipf->pos->row];
    winpiece = b_otherpiece (rgipf->owner);

    if ((piece != 'O') && (piece != 'X'))
    {
	return (NULL);
    }

    nboard = b_copy (oboard);

    if (nboard->gipfextra != NULL)
    {
	emptyll (nboard->gipfextra, del_rem_gipf);
	free (nboard->gipfextra);
	nboard->gipfextra = NULL;
    }

    nboard->status = S_NORMAL;

    nboard->pieces[rgipf->pos->col][rgipf->pos->row] = '.';

    if (nboard->log)
    {
	strpos = postostr (rgipf->pos);
	sprintf (tempstr, "%c:%2s%c", rgipf->owner, strpos, piece);
	free (strpos);
	addtolog (nboard->log, LOGREMGIPF, tempstr);
    }

    if (piece == 'O')
    {
	nboard->gipfwhite--;
	if (piece == winpiece)
	{
	    nboard->white += 2;
	}
	else
	{
	    nboard->lostwhite += 2;
	}
    }
    else
    {
	nboard->gipfblack--;
	if (piece == winpiece)
	{
	    nboard->black += 2;
	}
	else
	{
	    nboard->lostblack += 2;
	}
    }

    /* check white rowlist */
    if (nboard->rowextraw != NULL)
    {
	/* check if all rows are still valid */
	emptyll (nboard->rowextraw, del_rem_row);/* I now have an empty list */

	counter = 1;
	while ((row_ptr = (rem_row *) llitembynr (oboard->rowextraw, counter)) 
	       != NULL)
	{
	    counter++;
	    if ((n_row = b_rowoffour (nboard, row_ptr->rowindex)) != NULL)
	    {
		pushll (nboard->rowextraw, n_row);
	    }
	}

	if (llitembynr (nboard->rowextraw, 1) == NULL)
	{
	    free (nboard->rowextraw);
	    nboard->rowextraw = NULL;
	}
    }

    /* check black rowlist */
    if (nboard->rowextrab != NULL)
    {
	/* check if all rows are still valid */

	emptyll (nboard->rowextrab, del_rem_row);/* I now have an empty list */

	counter = 1;
	while ((row_ptr = (rem_row *) llitembynr (oboard->rowextrab, counter)) 
	       != NULL)
	{
	    counter++;
	    if ((n_row = b_rowoffour (nboard, row_ptr->rowindex)) != NULL)
	    {
		pushll (nboard->rowextrab, n_row);
	    }
	}

	if (llitembynr (nboard->rowextrab, 1) == NULL)
	{
	    free (nboard->rowextrab);
	    nboard->rowextrab = NULL;
	}
    }

    if ((nboard->rowextraw == NULL) && (nboard->rowextrab == NULL))
    {
	nboard->checkfour = 'n';
    }

#ifdef WINGIPF
    if (nboard->removewait)
    {
	waitremgipf (oboard, rgipf->pos, nboard);
    }
#endif

    return (nboard);
}


/*
** b_checkfour: check for 4 in a row
**
** parameters: 
**    oboard: pointer to gipf-board
**
** returns:
**    pointer to a new gipf-board
*/
board * b_checkfour (board * oboard)
{
    board * nboard,
	* new_board;
    int inrow4_w = 0,
	inrow4_b = 0,
	rownr,
	countrow,
        save_col,
	dir;
    listheader * row4list_w = NULL,
	* row4list_b = NULL;
    position base,
	next;
    char base_piece,
	base_piece2,
	piece;
    rem_row * rowp;

    if (oboard->checkfour == 'n')
    {
	nboard = b_copy (oboard);
	nboard->status = S_NORMAL;
	return (nboard);
    }
    if ((oboard->rowextraw != NULL) && 
	((oboard->nextpiece == 'x') || (oboard->rowextrab == NULL)))
    {
	if (llitembynr (oboard->rowextraw, 2) == NULL)
	{
	    nboard = b_remove_row (oboard, 1);
	    return (nboard);
	}
	else
	{
	    nboard = b_copy (oboard);
	    nboard->status = S_REMOVEROW;
	    return (nboard);
	}
    } 
    else if (oboard->rowextrab != NULL)
    {
	if (llitembynr (oboard->rowextrab, 2) == NULL)
	{
	    nboard = b_remove_row (oboard, 1);
	    return (nboard);
	}
	else
	{
	    nboard = b_copy (oboard);
	    nboard->status = S_REMOVEROW;
	    return (nboard);
	}
    }

/*    base = new_position ();
    next = new_position ();*/

    for (rownr=0; rownr<21; rownr++)
    {
	base.col = rowlist[rownr][0];
	base.row = rowlist[rownr][1];

	if (oboard->pieces[base.col][base.row] == '.')
	{
	    continue;
	}
	
	countrow = 1;

	base_piece = oboard->pieces[base.col][base.row];
	base_piece2 = b_otherpiece (base_piece);

	/* first direction */
	dir = rowlist[rownr][2];

/*	next.col = base.col;
	next.row = base.row;

	save_col = next.col;
	next.col = b_buren[save_col][next.row][dir][0];
	next.row = b_buren[save_col][next.row][dir][1];*/
	next.col = b_buren[base.col][base.row][dir][0];
	next.row = b_buren[base.col][base.row][dir][1];

	piece = oboard->pieces[next.col][next.row];
	while ((next.col != 0) && (piece != '.'))
	{
	    if ((piece == base_piece) || (piece == base_piece2))
	    {
		countrow++;
	    }
	    else
	    {
		break;
	    }
	    save_col = next.col;
	    next.col = b_buren[save_col][next.row][dir][0];
	    next.row = b_buren[save_col][next.row][dir][1];

	    piece = oboard->pieces[next.col][next.row];
	}

	if (countrow < rowlist[rownr][4])
	{   /* not possible to get 4 in a row anymore */
	    continue;
	}

	/* second direction */
	/*dir = rowlist[rownr][3];*/
	dir = 5 - dir;

/*	next.col = base.col;
	next.row = base.row;

	save_col = next.col;
	next.col = b_buren[save_col][next.row][dir][0];
	next.row = b_buren[save_col][next.row][dir][1];*/
	next.col = b_buren[base.col][base.row][dir][0];
	next.row = b_buren[base.col][base.row][dir][1];

	piece = oboard->pieces[next.col][next.row];
	while ((next.col != 0) && (piece != '.'))
	{
	    if ((piece == base_piece) || (piece == base_piece2))
	    {
		countrow++;
	    }
	    else
	    {
		break;
	    }
	    save_col = next.col;
	    next.col = b_buren[save_col][next.row][dir][0];
	    next.row = b_buren[save_col][next.row][dir][1];

	    piece = oboard->pieces[next.col][next.row];
	}

	if (countrow > 3)
	{
	    rowp = b_rowoffour (oboard, rownr);
	    
	    if ((base_piece == 'o') || (base_piece == 'O'))
	    {
		inrow4_w++;
		if (row4list_w == NULL)
		{
		    row4list_w = (listheader *) malloc (sizeof (listheader));
		    newlist (row4list_w);
		}
		pushll (row4list_w, rowp);
	    }
	    else
	    {
		inrow4_b++;
		if (row4list_b == NULL)
		{
		    row4list_b = (listheader *) malloc (sizeof (listheader));
		    newlist (row4list_b);
		}
		pushll (row4list_b, rowp);
	    }
	}
    }

/*    del_position (base);
    del_position (next);*/

    nboard = b_copy (oboard);

    if (inrow4_w != 0)
    {
	nboard->rowextraw = row4list_w;
    }
    if (inrow4_b != 0)
    {
	nboard->rowextrab = row4list_b;
    }

    if ((inrow4_w == 0) && (inrow4_b == 0))
    {
	nboard->status = S_NORMAL;
	nboard->checkfour = 'n';
	return (nboard);
    }
    if ((nboard->rowextraw != NULL) && 
	((nboard->nextpiece == 'x') || (nboard->rowextrab == NULL)))
    {
	if (inrow4_w == 1)
	{
	    new_board = b_remove_row (nboard, 1);
	    b_del (nboard);
	    return (new_board);
	}
	else
	{
	    nboard->status = S_REMOVEROW;
	    return (nboard);
	}
    } 
    else
    {
	if (inrow4_b == 1)
	{
	    new_board = b_remove_row (nboard, 1);
	    b_del (nboard);
	    return (new_board);
	}
	else
	{
	    nboard->status = S_REMOVEROW;
	    return (nboard);
	}
    }
}


/*
** b_rowoffour: check if a certain row has four in a row
**
** parameters:
**    oboard: pointer to gipf-board
**    rownr: nr of row in rowlist
**
** returns:
**    pointer to rem_row-structure
**    NULL (if no 4 in a row found)
*/
rem_row * b_rowoffour (board * oboard, int rownr)
{
    int countrow,
	dir,
        save_col,
	count;
    listheader * temprow;
    position * start,
	* end,
	* base,
	* next;
    char base_piece,
	base_piece2,
	piece;
    rem_row * rowp;

    temprow = (listheader *) malloc (sizeof (listheader));
    newlist (temprow);
    countrow = 1;

    base = new_position();
    base->col = rowlist[rownr][0];
    base->row = rowlist[rownr][1];
    start = (position *) copy_position ((void *) base);
    end = (position *) copy_position ((void *) base);
    next = (position *) copy_position ((void *) base);

    if (oboard->pieces[base->col][base->row] == '.')
    {
	/* cleanup */
	free (temprow);
	del_position ((void *) base);
	del_position ((void *) start);
	del_position ((void *) end);
	del_position ((void *) next);
	return (NULL);
    }
	
    base_piece = oboard->pieces[base->col][base->row];
    base_piece2 = b_otherpiece (base_piece);
    pushll (temprow, copy_position ((void *) base));

    /* first direction */
    dir = rowlist[rownr][2];

    count = 1;
/*    b_moveto (next, col_dif, &row_dif);*/
    save_col = next->col;
    next->col = b_buren[save_col][next->row][dir][0];
    next->row = b_buren[save_col][next->row][dir][1];

    piece = oboard->pieces[next->col][next->row];
    while ((next->col != 0) && (piece != '.'))
    {
	pushll (temprow, copy_position ((void *) next));
	if ((count == 1) &&
	    ((piece == base_piece) || (piece == base_piece2)))
	{
	    countrow++;
	    start->col = next->col;
	    start->row = next->row;
	}
	else
	{
	    count = 0;
	}
/*	b_moveto (next, col_dif, &row_dif);*/
	save_col = next->col;
	next->col = b_buren[save_col][next->row][dir][0];
	next->row = b_buren[save_col][next->row][dir][1];

	piece = oboard->pieces[next->col][next->row];
    }

    /* second direction */
    /*dir = rowlist[rownr][3];*/
    dir = 5 - dir;

    next->col = base->col;
    next->row = base->row;

    count = 1;
/*    b_moveto (next, col_dif, &row_dif);*/
    save_col = next->col;
    next->col = b_buren[save_col][next->row][dir][0];
    next->row = b_buren[save_col][next->row][dir][1];

    piece = oboard->pieces[next->col][next->row];
    while ((next->col != 0) && (piece != '.'))
    {
	pushll (temprow, copy_position ((void *) next));
	if ((count == 1) &&
	    ((piece == base_piece) || (piece == base_piece2)))
	{
	    countrow++;
	    end->col = next->col;
	    end->row = next->row;
	}
	else
	{
	    count = 0;
	}
/*	b_moveto (next, col_dif, &row_dif);*/
	save_col = next->col;
	next->col = b_buren[save_col][next->row][dir][0];
	next->row = b_buren[save_col][next->row][dir][1];

	piece = oboard->pieces[next->col][next->row];
    }

    if (countrow > 3)
    {
	rowp = new_rem_row ();
	rowp->startpos = start;
	rowp->endpos = end;
	rowp->piecelist = temprow;
	rowp->rowindex = rownr;
	if ((base_piece == 'o') || (base_piece == 'x'))
	{
	    rowp->owner = base_piece;
	}
	else
	{
	    rowp->owner = base_piece2;
	}
    }
    else
    {
	emptyll (temprow, del_position_f);
	free (temprow);
	del_position (start);
	del_position (end);
	rowp = NULL;
    }
    del_position (base);
    del_position (next);

    return (rowp);
}

#if 0
/*
** b_moveto: give coor of new position
**
** parameters:
**     pos: startpos, endpos after run
**     col_dif: column difference
**     row_dif: row difference
*/
inline void b_moveto (position * pos, int col_dif, int * row_dif)
{
    if (col_dif != 0)
    {
	if (pos->col == 4)
	{
	    (*row_dif)--;
	}
	pos->col += col_dif;
    }

    pos->row += (*row_dif);
    if ((pos->col == 0) ||
	(pos->col == 8) ||
	(pos->row < 2) ||
	(pos->row > b_colsize(pos->col)))
    {
	pos->col = 0;
	pos->row = 0;
    }
    return;
}


/*
** b_colour: return the number of available pieces for a colour
**
** parameters:
**    oboard : pointer to gipf-board
**    colour : o, O, x, X
**
** returns:
**    nr of pieces
*/
int b_colour (board * oboard, char colour)
{
    if ((colour == 'o') || (colour == 'O'))
    {
	return (oboard->white);
    }
    else if ((colour == 'x') || (colour == 'X'))
    {
	return (oboard->black);
    }
    return (-1);
}


/*
** b_colour_gipf: return the number of gipfpieces for a colour
**
** parameters:
**    oboard : pointer to gipf-board
**    colour : o, O, x, X
**
** returns:
**    nr of pieces
*/
int b_colour_gipf (board * oboard, char colour)
{
    if ((colour == 'o') || (colour == 'O'))
    {
	return (oboard->gipfwhite);
    }
    else if ((colour == 'x') || (colour == 'X'))
    {
	return (oboard->gipfblack);
    }
    return (-1);
}


/*
** b_colour_lost: return the number of lost pieces for a colour
**
** parameters:
**    oboard : pointer to gipf-board
**    colour : o, O, x, X
**
** returns:
**    nr of pieces
*/
int b_colour_lost (board * oboard, char colour)
{
    if ((colour == 'o') || (colour == 'O'))
    {
	return (oboard->lostwhite);
    }
    else if ((colour == 'x') || (colour == 'X'))
    {
	return (oboard->lostblack);
    }
    return (-1);
}


/*
** b_colour_type: return the type of pieces this colour can place
**
** parameters:
**    oboard : pointer to gipf-board
**    colour : o, O, x, X
**
** returns:
**    n or g
*/
char b_colour_type (board * oboard, char colour)
{
    if ((colour == 'o') || (colour == 'O'))
    {
	return (oboard->typewhite);
    }
    else if ((colour == 'x') || (colour == 'X'))
    {
	return (oboard->typeblack);
    }
    return (' ');
}
#endif

/*
** b_piece: return piece at position
**
** parameters:
**    oboard: pointer to gipf-board
**    pos: position-string (ex. b4)
**
** returns:
**    piece
*/ 
char b_piece (board * oboard, char * strpos)
{
    position * pos;
    char piece;

    pos = strtopos (strpos);
    piece = oboard->pieces[pos->col][pos->row];
    del_position (pos);

    return (piece);
}


/*
** b_game_finished : check if game is finished
**
** parameters:
**    oboard: gipf-board
**
** returns:
**    0: game not finished
**    1: game finished
**
** REMARK: only do this if no more rows can be removed from the board
*/
int b_game_finished (board * oboard)
{
    /*
    ** check if a player has no gipf-pieces left
    ** but only do this at the second move
    */
    if ((oboard->movecounter > 0) &&
	(oboard->gipfwhite == 0))
    {
	oboard->status = S_FINISHED;
	oboard->winner = 'x';
	return (1);
    }

    if (((oboard->movecounter > 1) || 
	 ((oboard->nextpiece == 'o') && (oboard->movecounter == 1))) &&
	(oboard->gipfblack == 0))
    {
	oboard->status = S_FINISHED;
	oboard->winner = 'o';
	return (1);
    }

    /* check if the next player has pieces left */
    if (b_colour (oboard, oboard->nextpiece) == 0)
    {
	oboard->status = S_FINISHED;
	oboard->winner = b_opponent (oboard->nextpiece);
	return (1);
    }

    return (0);
}


/*
** b_compare: compare the contents of 2 gipf-boards
**
** parameters:
**    board1
**    board2
**
** returns:
**    0: equal
**    1: different
*/
int b_compare (board * board1, board * board2)
{
    int i,
	j;

    /* sanity check */
    if ((board1 == NULL) || (board2 == NULL))
    {
	return (1);
    }

    /* compare the different counters */
    if ((board1->nextpiece != board2->nextpiece) ||
	(board1->movecounter != board2->movecounter) ||
	(board1->white != board2->white) ||
	(board1->lostwhite != board2->lostwhite) ||
	(board1->gipfwhite != board2->gipfwhite) ||
	(board1->typewhite != board2->typewhite) ||
	(board1->black != board2->black) ||
	(board1->lostblack != board2->lostblack) ||
	(board1->gipfblack != board2->gipfblack) ||
	(board1->typeblack != board2->typeblack))
    {
	return (1);
    }

#define MEMCMP 1 
#ifndef MEMCMP
    /* compare board-contents */
    for (i = 1; i < 8; i++)
	for (j = 2; j <= b_colsize (i); j++)
	    if (board1->pieces[i][j] != board2->pieces[i][j])
		return (1);
#else
    if (memcmp (&(board1->pieces), &(board2->pieces), 72) != 0)
    {
	return (1);
    }
#endif
    
    return (0);
}


listheader * b_row_extra (board * oboard)
{
    if ((oboard->rowextraw != NULL) && 
	((oboard->nextpiece == 'x') || (oboard->rowextrab == NULL)))
    {
	return (oboard->rowextraw);
    } 
    else
    {
	return (oboard->rowextrab);
    }
}


/*
** replace the piece at position 'pos' with 'piece'
** try to adapt the counters
**
** returns:
**   NULL: error
*/
board * b_edit_piece (board * oboard, position * pos, char piece)
{
    board * nboard;
    
    nboard = b_copy (oboard);

    if (nboard->pieces[pos->col][pos->row] == piece)
    {
	return (nboard);
    }

    /* remove piece */
    switch (nboard->pieces[pos->col][pos->row])
    {
    case 'o':
	nboard->white += 1;
	break;
    case 'O':
	nboard->white += 2;
	nboard->gipfwhite -= 1;
	break;
    case 'x':
	nboard->black += 1;
	break;
    case 'X':
	nboard->black += 2;
	nboard->gipfblack -= 1;
	break;
    default:
	break;
    }

    /* add piece */
    switch (piece)
    {
    case 'o':
	if (nboard->white == 0)
	{
	    b_del (nboard);
	    return (NULL);
	}
	nboard->white -= 1;
	break;
    case 'O':
	if (nboard->white < 2)
	{
	    b_del (nboard);
	    return (NULL);
	}
	nboard->white -= 2;
	nboard->gipfwhite += 1;
	break;
    case 'x':
	if (nboard->black == 0)
	{
	    b_del (nboard);
	    return (NULL);
	}
	nboard->black -= 1;
	break;
    case 'X':
	if (nboard->black < 2)
	{
	    b_del (nboard);
	    return (NULL);
	}
	nboard->black -= 2;
	nboard->gipfblack += 1;
	break;
    default:
	break;
    }

    nboard->pieces[pos->col][pos->row] = piece;
    
    return (nboard);
}


board * b_edit_lostwhite (board * oboard, int newval)
{
    board * nboard;

    if ((oboard->white + oboard->lostwhite - newval) < 0)
    {
	return (NULL);
    }
    
    nboard = b_copy (oboard);
    nboard->white = nboard->white + nboard->lostwhite - newval;
    nboard->lostwhite = newval;

    return (nboard);
}


board * b_edit_lostblack (board * oboard, int newval)
{
    board * nboard;

    if ((oboard->black + oboard->lostblack - newval) < 0)
    {
	return (NULL);
    }
    
    nboard = b_copy (oboard);
    nboard->black = nboard->black + nboard->lostblack - newval;
    nboard->lostblack = newval;

    return (nboard);
}


board * b_from_xml (xmlite_entity * root)
{
    xmlite_entity *x1, *x2, *x3;
    char tempstr[100],
	piece;
    board * nboard;
    int counter,
	counter2;
    position * temppos;
    rem_row * row_ptr;
    rem_gipf * gipf_ptr;

    if (root->getname() != "board")
    {   // very wrong
	return (NULL);
    }

    nboard = b_new (T_TOURNAMENT);

    // white player
    nboard->white = 
	atoi (root->getcontentbyname("whitepieces")->getvalue().c_str());
    nboard->lostwhite = 
	atoi (root->getcontentbyname("whitelost")->getvalue().c_str());
    nboard->gipfwhite = 
	atoi (root->getcontentbyname("whitegipf")->getvalue().c_str());
    nboard->typewhite = 
	root->getcontentbyname("whitetype")->getvalue()[0];

    // black player
    nboard->black = 
	atoi (root->getcontentbyname("blackpieces")->getvalue().c_str());
    nboard->lostblack = 
	atoi (root->getcontentbyname("blacklost")->getvalue().c_str());
    nboard->gipfblack = 
	atoi (root->getcontentbyname("blackgipf")->getvalue().c_str());
    nboard->typeblack = 
	root->getcontentbyname("blacktype")->getvalue()[0];

    nboard->nextpiece = 
	root->getcontentbyname("nextplayer")->getvalue() == "white" ? 'o':'x';

    // pieces
    counter = 0;
    while ((x1 = root->getcontentbyname ("position", counter)) != NULL)
    {
	counter++;

	piece = x1->getattribute ("color") == "white" ? 'o' : 'x';
	if (x1->getattribute ("type") == "gipf")
	    piece = b_otherpiece (piece);

	temppos = strtopos (x1->getvalue().c_str());
	nboard->pieces[temppos->col][temppos->row] = piece;
	del_position (temppos);
    }

    // white rows
    if ((x1 = root->getcontentbyname ("rowextrawhite")) != NULL)
    {
	nboard->status = S_REMOVEROW;
	nboard->rowextraw = (listheader *) malloc (sizeof (listheader));
	newlist (nboard->rowextraw);

	counter = 0;
	while ((x2 = x1->getcontentbyname ("remrow", counter)) != NULL)
	{
	    counter++;

	    row_ptr = new_rem_row ();
	    pushll (nboard->rowextraw, row_ptr);
	    row_start (row_ptr) = 
		strtopos (x2->getattribute ("startposition").c_str());
	    row_end (row_ptr) =
		strtopos (x2->getattribute ("endposition").c_str());
	    row_owner (row_ptr) =
		x2->getattribute ("owner").c_str()[0];
	    row_ptr->rowindex = atoi (x2->getattribute ("rowindex").c_str());
	    
	    row_ptr->piecelist = (listheader *) malloc (sizeof (listheader));
	    newlist (row_ptr->piecelist);
	    counter2 = 0;
	    while ((x3 = x2->getcontentbyname ("position", counter2)) != NULL)
	    {
		counter2++;
		temppos = strtopos (x3->getvalue ().c_str());
		pushll (row_ptr->piecelist, temppos);
	    }
	}
    }

    // black rows
    if ((x1 = root->getcontentbyname ("rowextrablack")) != NULL)
    {
	nboard->status = S_REMOVEROW;
	nboard->rowextrab = (listheader *) malloc (sizeof (listheader));
	newlist (nboard->rowextrab);

	counter = 0;
	while ((x2 = x1->getcontentbyname ("remrow", counter)) != NULL)
	{
	    counter++;

	    row_ptr = new_rem_row ();
	    pushll (nboard->rowextrab, row_ptr);
	    row_start (row_ptr) = 
		strtopos (x2->getattribute ("startposition").c_str());
	    row_end (row_ptr) =
		strtopos (x2->getattribute ("endposition").c_str());
	    row_owner (row_ptr) =
		x2->getattribute ("owner").c_str()[0];
	    row_ptr->rowindex = atoi (x2->getattribute ("rowindex").c_str());
	    
	    row_ptr->piecelist = (listheader *) malloc (sizeof (listheader));
	    newlist (row_ptr->piecelist);
	    counter2 = 0;
	    while ((x3 = x2->getcontentbyname ("position", counter2)) != NULL)
	    {
		counter2++;
		temppos = strtopos (x3->getvalue ().c_str());
		pushll (row_ptr->piecelist, temppos);
	    }
	}
    }

    // gipf pieces that can be removed
    if ((x1 = root->getcontentbyname ("gipfextra")) != NULL)
    {
	nboard->status = S_REMOVEGIPF;
	nboard->gipfextra = (listheader *) malloc (sizeof (listheader));
	newlist (nboard->gipfextra);

	counter = 0;
	while ((x2 = x1->getcontentbyname ("remgipf", counter)) != NULL)
	{
	    counter++;

	    gipf_ptr = new_rem_gipf ();
	    pushll (nboard->gipfextra, gipf_ptr);

	    gipf_ptr->owner = x2->getattribute ("owner")[0];
	    gipf_ptr->pos = strtopos (x2->getvalue ().c_str());
	}
    }

    return (nboard);
}


xmlite_entity * b_to_xml (board * oboard)
{
    position * temppos;
    xmlite_entity *x1, *x2, *x3, *x4;
    char tempstr[100],
	* strptr,
	tempc;
    int counter,
	counter2,
	i, j;
	
    x1 = new xmlite_entity ("board");

    // white player
    x2 = new xmlite_entity ("whitepieces");
    sprintf (tempstr, "%d", oboard->white);
    x2->setvalue (tempstr);
    x1->addcontent (x2);
    x2 = new xmlite_entity ("whitelost");
    sprintf (tempstr, "%d", oboard->lostwhite);
    x2->setvalue (tempstr);
    x1->addcontent (x2);
    x2 = new xmlite_entity ("whitegipf");
    sprintf (tempstr, "%d", oboard->gipfwhite);
    x2->setvalue (tempstr);
    x1->addcontent (x2);
    x2 = new xmlite_entity ("whitetype");
    sprintf (tempstr, "%c", oboard->typewhite);
    x2->setvalue (tempstr);
    x1->addcontent (x2);

    // black player
    x2 = new xmlite_entity ("blackpieces");
    sprintf (tempstr, "%d", oboard->black);
    x2->setvalue (tempstr);
    x1->addcontent (x2);
    x2 = new xmlite_entity ("blacklost");
    sprintf (tempstr, "%d", oboard->lostblack);
    x2->setvalue (tempstr);
    x1->addcontent (x2);
    x2 = new xmlite_entity ("blackgipf");
    sprintf (tempstr, "%d", oboard->gipfblack);
    x2->setvalue (tempstr);
    x1->addcontent (x2);
    x2 = new xmlite_entity ("blacktype");
    sprintf (tempstr, "%c", oboard->typeblack);
    x2->setvalue (tempstr);
    x1->addcontent (x2);
    
    x2 = new xmlite_entity ("nextplayer");
    x2->setvalue (oboard->nextpiece == 'o' ? "white" : "black");
    x1->addcontent (x2);

    // pieces on the board
    temppos = (position *) malloc (sizeof (position));
    for (i = 1; i < 8; i++)
    {
	temppos->col = i;
	for (j = 2; j <= b_colsize (i); j++)
	{
	    temppos->row = j;
	    tempc = b_ppiece (oboard, temppos);
	    if (tempc != '.')
	    {
		x2 = new xmlite_entity ("position");
		if ((tempc == 'o') || (tempc == 'x'))
		    x2->addattribute ("type", "piece");
		else
		    x2->addattribute ("type", "gipf");
		if ((tempc == 'o') || (tempc == 'O'))
		    x2->addattribute ("color", "white");
		else
		    x2->addattribute ("color", "black");

		strptr = postostr (temppos);
		x2->setvalue (strptr);
		delete (strptr);
		x1->addcontent (x2);
	    }
	}
    }
    free (temppos);

    // gipfextra
    if (oboard->gipfextra != NULL)
    {
	rem_gipf * gipf_ptr;
	
	x2 = new xmlite_entity ("gipfextra");
	x1->addcontent (x2);

	counter = 1;
	while ((gipf_ptr = (rem_gipf *)
		llitembynr (oboard->gipfextra, counter)) != NULL)
	{
	    counter++;

	    x3 = new xmlite_entity ("remgipf");
	    x2->addcontent (x3);
	    sprintf (tempstr, "%c", gipf_ptr->owner);
	    x3->addattribute ("owner", tempstr);
	    strptr = postostr (gipf_ptr->pos);
	    x3->setvalue (strptr);
	    delete (strptr);
	}
    }
    
    // rowextrawhite
    if (oboard->rowextraw != NULL)
    {
	rem_row * row_ptr;
	position * pos_ptr;
	
	x2 = new xmlite_entity ("rowextrawhite");
	x1->addcontent (x2);

	counter = 1;
	while ((row_ptr = (rem_row *)
		llitembynr (oboard->rowextraw, counter)) != NULL)
	{
	    counter++;

	    x3 = new xmlite_entity ("remrow");
	    x2->addcontent (x3);
	    strptr = postostr (row_start(row_ptr));
	    x3->addattribute ("startposition", strptr);
	    delete (strptr);
	    strptr = postostr (row_end(row_ptr));
	    x3->addattribute ("endposition", strptr);
	    delete (strptr);
	    sprintf (tempstr, "%c", row_owner (row_ptr));
	    x3->addattribute ("owner", tempstr);
	    sprintf (tempstr, "%d", row_ptr->rowindex);
	    x3->addattribute ("rowindex", tempstr);

	    counter2 = 1;
	    while ((pos_ptr = (position *)
		    llitembynr (row_ptr->piecelist, counter2)) != NULL)
	    {
		counter2++;

		x4 = new xmlite_entity ("position");
		x3->addcontent (x4);
		strptr = postostr (pos_ptr);
		x4->setvalue (strptr);
		delete (strptr);
	    }
	}
    }
    
    // rowextrablack
    if (oboard->rowextrab != NULL)
    {
	rem_row * row_ptr;
	position * pos_ptr;
	
	x2 = new xmlite_entity ("rowextrablack");
	x1->addcontent (x2);

	counter = 1;
	while ((row_ptr = (rem_row *)
		llitembynr (oboard->rowextrab, counter)) != NULL)
	{
	    counter++;

	    x3 = new xmlite_entity ("remrow");
	    x2->addcontent (x3);
	    strptr = postostr (row_start(row_ptr));
	    x3->addattribute ("startposition", strptr);
	    delete (strptr);
	    strptr = postostr (row_end(row_ptr));
	    x3->addattribute ("endposition", strptr);
	    delete (strptr);
	    sprintf (tempstr, "%c", row_owner (row_ptr));
	    x3->addattribute ("owner", tempstr);
	    sprintf (tempstr, "%d", row_ptr->rowindex);
	    x3->addattribute ("rowindex", tempstr);

	    counter2 = 1;
	    while ((pos_ptr = (position *)
		    llitembynr (row_ptr->piecelist, counter2)) != NULL)
	    {
		counter2++;

		x4 = new xmlite_entity ("position");
		x3->addcontent (x4);
		strptr = postostr (pos_ptr);
		x4->setvalue (strptr);
		delete (strptr);
	    }
	}
    }

    return (x1);
}

/*
** REMARK:
**     Don't use this anymore
**     use the new b_to_xml
*/
/*
** write a boardsituation to a file
** use a format that can be read by b_from_file
** (should also be readable by gipfdraw, so you can easily
**  make gif's from a board-position)
**
** returns -1 on failure, otherwise 0
**
** REMARK: the board must be in a stable state (S_NORMAL or S_FINISHED)
**         otherwise the save will fail
*/
int b_to_file (board * oboard, FILE * fp)
{
    int i, j;
    position * temppos;
    char * tempstr;

    if (oboard == NULL)
	return (-1);
    
    if ((oboard->status != S_NORMAL) && (oboard->status != S_FINISHED))
    {
	return (-1);
    }

    fprintf (fp, "whitepieces:%d\n", oboard->white);
    fprintf (fp, "whitelost:%d\n", oboard->lostwhite);

    fprintf (fp, "blackpieces:%d\n", oboard->black);
    fprintf (fp, "blacklost:%d\n", oboard->lostblack);

    fprintf (fp, "nextplayer:%c\n", oboard->nextpiece);
    
    temppos = (position *) malloc (sizeof (position));
    for (i = 1; i < 8; i++)
    {
	temppos->col = i;
	for (j = 2; j <= b_colsize (i); j++)
	{
	    temppos->row = j;
	    if (b_ppiece (oboard, temppos) != '.')
	    {
		tempstr = postostr (temppos);
		fprintf (fp, "%c:%2s\n",
			 b_ppiece (oboard, temppos), tempstr);
		free (tempstr);
	    }
	}
    }
    free (temppos);

    fprintf (fp, "# end of board\n");
    
    return (0);
}


/*
** read a boardsituation from a file and store it in a
** board-structure.
**
** returns NULL on failure
*/
board * b_from_file (FILE * fp)
{
    char buffer[100]="",
	tempchar,
	tempstr[100];
    board * nboard;
    int tempnr;
    position * temppos;
    
    nboard = b_new (T_TOURNAMENT);
    nboard->typewhite = 'n';
    nboard->typeblack = 'n';
    
    while (strncmp (buffer, "# end of board", 14) != 0)
    {
	if (fgets (buffer, 100, fp) == NULL)
	{
	    b_del (nboard);
	    return (NULL);
	}

	if (sscanf (buffer, "whitepieces:%d", &tempnr) == 1)
	{
	    nboard->white = tempnr;
	}
	else if (sscanf (buffer, "whitelost:%d", &tempnr) == 1)
	{
	    nboard->lostwhite = tempnr;
	}
	else if (sscanf (buffer, "blackpieces:%d", &tempnr) == 1)
	{
	    nboard->black = tempnr;
	}
	else if (sscanf (buffer, "blacklost:%d", &tempnr) == 1)
	{
	    nboard->lostblack = tempnr;
	}
	else if (sscanf (buffer, "nextplayer:%c", &tempchar) == 1)
	{
	    nboard->nextpiece = tempchar;
	}
	else if (sscanf (buffer, "%c:%s", &tempchar, tempstr) == 2)
	{
	    temppos = strtopos (tempstr);
	    nboard->pieces[temppos->col][temppos->row] = tempchar;
	    switch (tempchar)
	    {
	    case 'O':
		nboard->gipfwhite += 1;
		break;
	    case 'X':
		nboard->gipfblack += 1;
		break;
	    }
	    del_position (temppos);
	}
    }

    if ((nboard->gipfwhite == 0) && (nboard->gipfblack == 0))
    {
	nboard->gipfwhite = -1;
	nboard->gipfblack = -1;
    }
    
    return (nboard);
}
