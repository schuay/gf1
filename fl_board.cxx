/*
** $Id: fl_board.cxx,v 1.29 2000/02/28 19:58:39 kurt Exp $
**
** all that is necessary for drawing a gipf-board and the pieces on it
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

#include <string.h>
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include "fl_board.h"

#ifndef min
#define min(x,y)       (x < y ? x : y)
#endif

#ifndef max
#define max(x,y)       (x > y ? x : y)
#endif

#define round(x)       (int)(x + 0.5)

void shmpos (void * data);

/*
** all measurements are based on a square of 2 * 2.5
*/
// x and y-position of each first point of a column
const double rowbase[9][2] = {
    {.134, 2},          // row a
    {.3505, 2.125},     // row b
    {.567, 2.25},       // row c
    {.7835, 2.375},     // row d
    {1, 2.5},           // row e
    {1.2165, 2.375},    // row f
    {1.433, 2.25},      // row g
    {1.6495, 2.125},    // row h
    {1.866, 2}          // row i
};

/* lines on the board */
const position linetable[21][2] = {
    {{0, 4}, {5, 8}},    {{0, 3}, {6, 7}},    {{0, 2}, {7, 6}},
    {{0, 1}, {8, 5}},    {{1, 1}, {8, 4}},    {{2, 1}, {8, 3}},
    {{3, 1}, {8, 2}},    {{0, 2}, {5, 1}},    {{0, 3}, {6, 1}},
    {{0, 4}, {7, 1}},    {{0, 5}, {8, 1}},    {{1, 6}, {8, 2}},
    {{2, 7}, {8, 3}},    {{3, 8}, {8, 4}},    {{1, 6}, {1, 1}},
    {{2, 7}, {2, 1}},    {{3, 8}, {3, 1}},    {{4, 9}, {4, 1}},
    {{5, 8}, {5, 1}},    {{6, 7}, {6, 1}},    {{7, 6}, {7, 1}}
};

/* possible from-points */
const position fromtable[24] = {
    {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {1, 6},
    {2, 7}, {3, 8}, {4, 9}, {5, 8}, {6, 7}, {7, 6},
    {8, 5}, {8, 4}, {8, 3}, {8, 2}, {8, 1}, {7, 1},
    {6, 1}, {5, 1}, {4, 1}, {3, 1}, {2, 1}, {1, 1},
};

/* possible to-points */
const position totable[18] = {
    {1, 2}, {1, 3}, {1, 4}, {1, 5}, {2, 6}, {3, 7},
    {4, 8}, {5, 7}, {6, 6}, {7, 5}, {7, 4}, {7, 3},
    {7, 2}, {6, 2}, {5, 2}, {4, 2}, {3, 2}, {2, 2}
};

/* white hexagon */
const position hexagon[7] = {
    {1, 2}, {1, 5}, {4, 8}, {7, 5}, {7, 2}, {4, 2}, {1, 2}
};

/*
** constructor
*/
fl_board::fl_board(int X,int Y,int W,int H,const char *l) 
    : Fl_Widget (X, Y, W, H, l) 
{
    theboard = NULL;
    from = NULL;
    to = NULL;
    row = NULL;
    fromtype = 0;
    gipfpossible = 0;
    state = BOARD_NONE;
    editpiece = 'o';
    positionhints = 1;
    showpos = NULL;
    showflag = 0;
    animlist = NULL;

    wpieces = NULL;
    wlost = NULL;
    wpile = NULL;
    bpieces = NULL;
    blost = NULL;
    bpile = NULL;
    execbutton = NULL;

    calcsizes ();

    // I don't know if this is a good solution
//    lyellow = FL_COLOR_CUBE + 
//	FL_RED_MULTIPLY * 4 +
//	FL_GREEN_MULTIPLY * 7 +
//	FL_BLUE_MULTIPLY * 3;
    lyellow = fl_color_cube(255 * FL_NUM_RED/256,
			    255 * FL_NUM_GREEN/256,
			    200 * FL_NUM_BLUE/256);
    return;
}


/*
** destructor
*/
fl_board::~fl_board ()
{
    b_del (theboard);

    if (row != NULL)
    {
	emptyll (row, del_position_f);
	free (row);
    }

    if (from != NULL)
	free (from);
    if (to != NULL)
	free (to);

    return;
}


/*
** the fl_board class should only handle one event on its own
** when an FL_PUSH occurs, the widget must check in what state it is
** and where a push can have effect in this state
*/
int fl_board::handle (int event)
{
    ::position * temppos;

    switch (event)
    {
    case FL_PUSH:
	/* check what was pushed */
	if (theboard == NULL)
	    return (1);

	if (state == BOARD_PLAY)
	{
	    if (checkfrompush ())
	    {
		return (1);
	    }
	    checktopush ();

	}
	else if (state == BOARD_EDIT)
	{
	    checkeditpush ();
	}

	// state = BOARD_NONE
	return (1);

    case FL_ENTER:
    case FL_MOVE:
	if (positionhints == 0)
	    return (0);

	temppos = getmousepos ();

	// mouse was moved to an unrecognized position
	if (temppos == NULL)
	{
	    // stop timer, even if it is not running
	    Fl::remove_timeout (shmpos, (void *) this);

	    if (showpos != NULL)
	    {
		del_position (showpos);
		showpos = NULL;
		if (showflag != 0)
		{
		    showflag = 0;
		    redraw ();
		}
	    }
	    return (1);
	}

	// mouse was moved to a known position
	// previous position was NULL
	if (showpos == NULL)
	{
	    // start timer
	    Fl::add_timeout (.5, shmpos, (void *) this);

	    showpos = temppos;
	    return (1);
	}

	// check if mouse was moved to a different position
	if ((showpos->row == temppos->row) && 
	    (showpos->col == temppos->col))
	{   // do nothing
	    del_position (temppos);
	    return (1);
	}

	// new known position

	// stop timer, even if it is not running
	Fl::remove_timeout (shmpos, (void *) this);

	del_position (showpos);
	showpos = NULL;
	if (showflag != 0)
	{
	    showflag = 0;
	    redraw ();
	}
	showpos = temppos;

	// start timer
	Fl::add_timeout (.5, shmpos, (void *) this);

	return (1);
	break;

    case FL_LEAVE:
	if (positionhints == 0)
	    return (0);

	Fl::remove_timeout (shmpos, (void *) this);

	if (showpos != NULL)
	{
	    del_position (showpos);
	    showpos = NULL;
	    if (showflag != 0)
	    {
		showflag = 0;
		redraw ();
	    }
	}
	break;
	
    default:
	return (0);
    }

    return (0);
}


/*
** check if one of the from-points was pushed
*/
int fl_board::checkfrompush ()
{
    int pushx,
	pushy,
	i,
	x, y,
	box;

    pushx = Fl::event_x ();
    pushy = Fl::event_y ();
    box = base / 15;

    for (i = 0; i < 24; i++)
    {
	pos2coor (&(fromtable[i]), x, y);
	
	if ((pushx > (x - box)) &&
	    (pushy > (y - box)) &&
	    (pushx < (x + box)) &&
	    (pushy < (y + box)))
	{
	    if ((from == NULL) || 
		(from->col != fromtable[i].col) || 
		(from->row != fromtable[i].row))
	    {
		setfrom (& fromtable[i]);
		redraw ();
	    }
	    else if (gipfpossible == 1)
	    {
		fromtype = (fromtype == 1 ? 0 : 1);
		redraw ();
	    }
	    return (1);
	}
    }

    return (0);
}


/*
** check if one of the to-points was pushed
*/
int fl_board::checktopush ()
{
    int pushx,
	pushy,
	i,
	x, y,
	box;

    pushx = Fl::event_x ();
    pushy = Fl::event_y ();
    box = base / 15;

    for (i = 0; i < 18; i++)
    {
	pos2coor (&(totable[i]), x, y);
	
	if ((pushx > (x - box)) &&
	    (pushy > (y - box)) &&
	    (pushx < (x + box)) &&
	    (pushy < (y + box)))
	{
	    setto (& totable[i]);
	    redraw ();
	    return (1);
	}
    }

    return (0);
}


/*
** check if any point on the board was pushed
*/
int fl_board::checkeditpush ()
{
    int pushx,
	pushy,
	i, j,
	x, y,
	box;
    ::position * temppos;
    board * nboard;

    pushx = Fl::event_x ();
    pushy = Fl::event_y ();
    box = base / 15;

    temppos = new_position ();
    /* check all board positions */
    for (i = 1; i < 8; i++)
    {
	temppos->col = i;
	for (j = 2; j <= b_colsize (i); j++)
	{
	    temppos->row = j;

	    pos2coor (temppos, x, y);
	    if ((pushx > (x - box)) &&
		(pushy > (y - box)) &&
		(pushx < (x + box)) &&
		(pushy < (y + box)))
	    {
		if (b_ppiece (theboard, temppos) != '.')
		{
		    if ((nboard = b_edit_piece (theboard, temppos, '.')) 
			!= NULL)
		    {
			b_del (theboard);
			theboard = nboard;
		    }
		}
		else
		{
		    if ((nboard = b_edit_piece (theboard, temppos, editpiece)) 
			!= NULL)
		    {
			b_del (theboard);
			theboard = nboard;
		    }
		}

		changecountervalues ();
		redraw ();
		return (1);
	    }
	}
    }
    free (temppos);

    return (0);
}


position * fl_board::getmousepos ()
{
    int pushx,
	pushy,
	i, j,
	x, y,
	box;
    ::position * temppos;
    int colsize[] = {5, 6, 7, 8, 9, 8, 7, 6, 5};

    pushx = Fl::event_x ();
    pushy = Fl::event_y ();
    box = base / 10;

    temppos = new_position ();
    /* draw pieces */
    for (i = 0; i < 9; i++)
    {
	temppos->col = i;
	for (j = 1; j <= colsize [i]; j++)
	{
	    temppos->row = j;

	    pos2coor (temppos, x, y);
	    if ((pushx > (x - box)) &&
		(pushy > (y - box)) &&
		(pushx < (x + box)) &&
		(pushy < (y + box)))
	    {
		return (temppos);
	    }
	}
    }
    free (temppos);

    return (NULL);
}


void fl_board::draw ()
{
    int i, j, nr,
	x1, y1,
	x2, y2,
	width, height,
	diam, straal;
    char tempstr[10],
	piece,
	* posstr;
    ::position * temppos;
    uchar dam;

    dam = damage();
    if (!(dam & FL_DAMAGE_ALL) &&
	(dam & FL_DAMAGE_ANIM))
    {   /* draw for move-animation */
	/* maybe things can be speeded up by using clipping */
	/* calculate clipping-region */
	calc_clipping ();
    }
    else
    {
	clip_x = x();
	clip_y = y();
	clip_w = w();
	clip_h = h();
    }
    fl_clip (clip_x, clip_y, clip_w, clip_h);

    /* set everything to background color */
//    fl_color (Fl_Widget::color());
    fl_color (FL_GRAY);
    fl_rectf (clip_x, clip_y, clip_w, clip_h);

    draw_box ();

    fl_color (FL_WHITE); // white

    // hexagon 
    fl_begin_polygon ();
    for (i = 0; i < 7; i++)
    {
	pos2coor (&(hexagon[i]), x1, y1);
#ifdef MSWIN
	/* didn't look nice on mswin */
	if ((i > 0) && (i < 4))
	    y1 += 1;
	else
	    y1 -= 1;
#endif
	fl_vertex (x1, y1);
    }
    fl_end_polygon ();

    fl_color (FL_BLACK); // black

    // lines
    for (i = 0; i < 21; i++)
    {
	pos2coor (&(linetable[i][0]), x1, y1);
	pos2coor (&(linetable[i][1]), x2, y2);

	/* I have to add 1 to the third parameter, or
	** vertical lines don't get drawn on ms windows */
	if (fl_not_clipped (min (x1, x2), min (y1, y2), 
			    max (x1, x2) - min (x1, x2) + 1,
			    max (y1, y2) - min (y1, y2)))
	    fl_line (x1, y1, x2, y2);
    }

    // from-positions
    straal = base / 24;
    diam = straal * 2;
    for (i = 0; i < 24; i++)
    {
	pos2coor (&(fromtable[i]), x1, y1);
	if (fl_not_clipped (x1 - straal, y1 - straal,
			    x1 + straal, y1 + straal))
	{
	    fl_begin_polygon ();
	    fl_circle (x1, y1, straal);
	    fl_end_polygon ();
	}
#if 0
	    fl_pie (x1 - straal, y1 - straal, diam, diam, 0, 360);
#endif
    }

    /* position names */
    fl_font (FL_HELVETICA, base/12);

    height = fl_height () / 2 - fl_descent ();
    for (i = 0; i < 9; i++)
    {
	sprintf (tempstr, "%c1", i+'a');
	width = round (fl_width (tempstr) / 2);
	x1 = xoffset + round (rowbase[i][0] * base);
	y1 = yoffset + round ((rowbase[i][1] - .5 * .25) * base);
	fl_draw (tempstr, x1 - width, y1 + height);

	nr = (i < 5 ? i + 5 : 13 - i);
	sprintf (tempstr, "%c%d", i+'a', nr);
	width = round (fl_width (tempstr) / 2);
	y1 = yoffset + round ((rowbase[i][1] - (nr + .5) * .25) * base);
	fl_draw (tempstr, x1 - width, y1 + height);
    }

    /* draw pieces if there is a board */
    if (theboard != NULL)
    {
	temppos = new_position ();
	/* draw pieces */
	for (i = 1; i < 8; i++)
	{
	    temppos->col = i;
	    for (j = 2; j <= b_colsize (i); j++)
	    {
		temppos->row = j;
		if (b_ppiece (theboard, temppos) != '.')
		{
		    drawpiece (temppos, b_ppiece (theboard, temppos)); 
		}
	    }
	}
	free (temppos);

	/* draw from and to (if necessary) */
	if (state == BOARD_PLAY)
	{
	    if (from != NULL)
	    {
		piece = b_next_piece (theboard);
		if (fromtype == 1)
		{
		    piece = b_otherpiece (piece);
		}
		drawpiece (from, piece);
	    }

	    if (to != NULL)
	    {
		drawcross (to->col, to->row);
	    }
	}

	/* draw crosses on row of pieces, if necessary */
	if (row != NULL)
	{
	    int counter = 1;
	    ::position * posp;
	    while ((posp = (::position *) llitembynr (row, counter)) != NULL)
	    {
		drawcross (posp->col, posp->row);
		counter++;
	    }
	}

	if (animlist != NULL)
	{
	    drawanimpieces ();
	}
    }

    // draw position-box
    if ((positionhints != 0) && (showpos != NULL) && (showflag == 1))
    {
	posstr = postostr (showpos);

	pos2coor (showpos, x1, y1);
	width = 8 + (int) fl_width (posstr);
	height = 4 + fl_height ();
	y1 -= height + 2;
	x1 -= 2;

	if (fl_not_clipped (x1, y1, width + 2, height + 2))
	{
	    fl_color (FL_DARK2); // 125,125,125
	    fl_rectf (x1 + 2, y1 + 2, width, height);
	    fl_color (lyellow); // 255,255,200
	    fl_rectf (x1, y1, width, height);
	    fl_color (FL_BLACK);
	    fl_rect (x1, y1, width, height);
	    fl_draw (posstr, x1 + 4, y1 + height - fl_descent () - 2);
	}

	free (posstr);
    }

    fl_pop_clip ();

    return;
}


void fl_board::drawpiece (::position * pos, char piece)
{
    int counter,
	x, y;
    struct movepiece {
	char piece;
	::position * from;
	::position * to;
    } * mpiece;

    /* don't draw piece if in animation-list */
    counter = 2;
    while ((mpiece = (struct movepiece *) llitembynr (animlist, counter))
	   != NULL)
    {
	counter++;
	if ((posp_col (pos) == posp_col (mpiece->from)) && 
	    (posp_row (pos) == posp_row (mpiece->from)))
	    return;
    }

    /* calculate position of center of the piece */
    pos2coor (pos, x, y);

    drawpiece (x, y, piece);

    return;
}

void fl_board::drawpiece (int x, int y, char piece)
{
    int straal, diam,
	straal2, diam2,
	straal3,
	counter;

    /* calculate size of piece */
    straal = base / 12;
    diam = straal * 2;
    straal2 = base / 18;
    diam2 = straal2 * 2;
    straal3 = base / 22;
    if (straal3 > (straal2 - 2))
	straal3 = straal2 - 2;

    if (!fl_not_clipped (x - straal, y - straal, diam, diam))
	return;

    if ((piece == 'o') || 
	(piece == 'O'))
    {
	fl_color (lyellow);
    }
    else
    {
	fl_color (FL_BLACK);
    }

    fl_begin_polygon ();
    fl_circle (x, y, straal);
    fl_end_polygon ();

    fl_color (FL_BLACK);
    fl_begin_loop ();
    fl_circle (x, y, straal);
    fl_end_loop ();

    if ((piece == 'O') || 
	(piece == 'X')) 
    {
	fl_color (FL_DARK1);
//	fl_begin_complex_polygon ();
//	fl_arc (x, y, straal2, 0, 360);
//	fl_gap ();
//	fl_arc (x, y, straal3, 0, 360);
//	fl_end_complex_polygon ();
	fl_begin_polygon ();
	fl_circle (x, y, straal2);
	fl_end_polygon ();

	if (piece == 'O')
	    fl_color (lyellow);
	else
	    fl_color (FL_BLACK);
	fl_begin_polygon ();
	fl_circle (x, y, straal3);
	fl_end_polygon ();	
    }

#if 0
#ifdef MSWIN
    fl_pie (x - straal + 1, y - straal + 1, diam - 2, diam - 2, 0, 360);
#else
    fl_pie (x - straal, y - straal, diam - 1, diam - 1, 0, 360);
#endif

    fl_color (FL_BLACK);
    fl_arc (x - straal, y - straal, diam, diam, 0, 360);

    if ((piece == 'O') || 
	(piece == 'X')) 
    {
	fl_color (FL_DARK1);
	fl_arc (x - straal2, y - straal2, diam2, diam2, 0, 360);
	fl_arc (x - straal2 + 1, y - straal2 + 1, diam2-2, diam2-2, 0, 360);
    }
#endif

    return;
}


void fl_board::drawanimpieces (void)
{
    int counter;
    struct movepiece {
	char piece;
	::position * from;
	::position * to;
    } * mpiece;
    int x, y, x1, y1, x2, y2;

    counter = 1;
    while ((mpiece = (struct movepiece *) llitembynr (animlist, counter))
	   != NULL)
    {
	counter++;

	/* calculate position of center of the piece */
	pos2coor (mpiece->from, x1, y1);
	pos2coor (mpiece->to, x2, y2);
	x = x1 + (x2 - x1) * step / maxstep;
	y = y1 + (y2 - y1) * step / maxstep;

	drawpiece (x, y, mpiece->piece);
    }

    if (step == maxstep)
    {
	while ((mpiece = (struct movepiece *) llrembynr (animlist, 1)) != NULL)
	{
	    free (mpiece->from);
	    free (mpiece->to);
	    free (mpiece);
	}
	free (animlist);

	animlist = NULL;
    }
    return;
}


void fl_board::drawcross (int col, int row)
{
    ::position temppos;
    int x, y,
	straal;

    /* calculate position of center of the piece */
    temppos.col = col;
    temppos.row = row;
    pos2coor (&temppos, x, y);

    straal = base / 16;

    if (!fl_not_clipped (x - straal, y - straal, straal * 2, straal * 2))
	return;

    fl_color (FL_RED);

    /*
    ** I draw a lot of lines here to get the impression of a
    ** thick cross
    */
    fl_line (x - straal, y - straal, x + straal, y + straal);
    fl_line (x - straal + 1, y - straal, x + straal, y + straal - 1);
    fl_line (x - straal, y - straal + 1, x + straal - 1, y + straal);

    fl_line (x + straal, y - straal, x - straal, y + straal);
    fl_line (x + straal - 1, y - straal, x - straal, y + straal - 1);
    fl_line (x + straal, y - straal + 1, x - straal + 1, y + straal);

    return;
}


void fl_board::resize (int x, int y, int w, int h)
{
    Fl_Widget::resize (x, y, w, h); 

    calcsizes ();

    if (showpos != NULL)
    {
	del_position (showpos);
	showpos = NULL;
	showflag = 0;
    }

    return;
}


/*
** calculate 
**    xoffset
**    yoffset
**    base
**
** to be used at every resize-event and at class-creation
*/
void fl_board::calcsizes ()
{
    int height,
	width;

    width = round (w() / 2.0);
    height = round (h() / 2.5);
    base = min (width, height);

    xoffset = round((w() - base * 2.0) / 2) + x();
    yoffset = round((h() - base * 2.5) / 2) + y();

    return;
}


/*
** calculate screen-coordinates starting from a board-position
*/
// solution: Fl_Input_ has a 'position' member function
inline void fl_board::pos2coor (const ::position *pos, int& x, int& y)
{
    x = xoffset + round (rowbase[pos->col][0] * base);
    y = yoffset + round ((rowbase[pos->col][1] - pos->row * .25) * base);

    return;
}


void fl_board::setoutputwidgets (Fl_Output * wp, Fl_Output * wl, 
				 fl_pile * wpi, Fl_Output * bp,
				 Fl_Output * bl, fl_pile * bpi,
				 Fl_Output * from_o, Fl_Output * to_o)
{
    wpieces = wp;
    wlost = wl;
    wpile = wpi;
    wpile->setcolor ('o');

    bpieces = bp;
    blost = bl;
    bpile = bpi;
    bpile->setcolor ('x');

    t_from = from_o;
    t_to = to_o;

    changecountervalues ();
    changefromtovalues ();

    return;
}

void fl_board::changefromtovalues ()
{
    char * tempstr;

    if (from != NULL)
    {
	tempstr = postostr (from);
	t_from->value (tempstr);
	free (tempstr);
    }
    else
    {
	t_from->value (NULL);
    }

    if (to != NULL)
    {
	tempstr = postostr (to);
        t_to->value (tempstr);
	free (tempstr);
    }
    else
    {
	t_to->value (NULL);
    }

    redraw ();

    return;
}


void fl_board::changecountervalues ()
{
    char tempstr[20];

    if (wpieces != NULL)
    {
	if (theboard != NULL)
	{
	    sprintf (tempstr, "%d", b_white (theboard));
	    wpieces->value (tempstr);
	    sprintf (tempstr, "lost: %d", b_white_lost (theboard));
	    wlost->value (tempstr);
	    wpile->setvalue (b_white (theboard));

	    sprintf (tempstr, "%d", b_black (theboard));
	    bpieces->value (tempstr);
	    sprintf (tempstr, "lost: %d", b_black_lost (theboard));
	    blost->value (tempstr);
	    bpile->setvalue (b_black (theboard));
	}
	else
	{
	    wpieces->value (NULL);
	    wlost->value (NULL);
	    wpile->setvalue (-1);
	    bpieces->value (NULL);
	    blost->value (NULL);
	    bpile->setvalue (-1);
	}
    }

    return;
}


void fl_board::setboard (board * newboard)
{
    if (b_compare (newboard, theboard) == 0)
    {   /* the new is the same as the old */
	return;
    }

    if (theboard != NULL)
    {
	b_del (theboard);
    }
    theboard = b_copy (newboard);

    changecountervalues ();
    redraw ();

    return;
}


void fl_board::setfrom (const ::position * newfrom)
{
    if (from != NULL)
    {
	free (from);
    }
    from = (::position *) copy_position ((void *) newfrom);

    changefromtovalues ();

    return;
}


void fl_board::setto (const ::position * newto)
{
    if ((newto != NULL) && (from != NULL) && (to != NULL) && 
	(execbutton != NULL) &&
	(posp_col (to) == posp_col (newto)) && 
	(posp_row (to) == posp_row (newto)))
    {
	execbutton->do_callback ();
	return;
    }
    if (to != NULL)
    {
	free (to);
    }
    to = (::position *) copy_position ((void *) newto);

    changefromtovalues ();

    return;
}


void fl_board::seteditpiece (char newpiece)
{
    editpiece = newpiece;
}


int fl_board::setlostwhite (int val)
{
    board * nboard;

    if (theboard == NULL)
	return (0);

    if ((nboard = b_edit_lostwhite (theboard, val)) != NULL)
    {
	b_del (theboard);
	theboard = nboard;
    }
    changecountervalues ();

    return (b_white_lost (theboard));
}


int fl_board::setlostblack (int val)
{
    board * nboard;

    if (theboard == NULL)
	return (0);

    if ((nboard = b_edit_lostblack (theboard, val)) != NULL)
    {
	b_del (theboard);
	theboard = nboard;
    }
    changecountervalues ();

    return (b_black_lost (theboard));
}


void fl_board::setstate (int newstate)
{
    state = newstate;

    if (state != BOARD_PLAY)
    {
	setfrom (NULL);
	setto (NULL);
    }
}


void fl_board::showmousepos ()
{
    if (showpos != NULL)
    {
	showflag = 1;
	redraw ();
    }

    return;
}


void shmpos (void * data)
{
    fl_board * boardobject = (fl_board *) data;

    boardobject->showmousepos ();

    return;
}


void fl_board::setgipfpossible (int flag)
{
    if (flag == 1)
    {
	gipfpossible = 1;
	fromtype = 1;
    }
    else
    {
	gipfpossible = 0;
	fromtype = 0;
    }
    return;
}

void fl_board::setrow (listheader * rowp)
{
    if (row != NULL)
    {
	emptyll (row, del_position_f);
	free (row);
    }
    if (rowp != NULL)
	row = copy_position_row (rowp);
    else
	row = NULL;

    redraw ();
    return;
}


void fl_board::initanim (int st, listheader * al)
{
    int counter;
    struct movepiece {
	char piece;
	::position * from;
	::position * to;
    } * mpiece, * npiece;
    listheader * newl;

    maxstep = st; 

    newl = (listheader *) malloc (sizeof (listheader));
    newlist (newl);

    counter = 1;
    while ((mpiece = (struct movepiece *) llitembynr (al, counter))
	   != NULL)
    {
	counter++;

	npiece = (struct movepiece *) malloc (sizeof (struct movepiece));
	npiece->piece = mpiece->piece;
	npiece->from = (::position *) copy_position ((void *) mpiece->from);
	npiece->to = (::position *) copy_position ((void *) mpiece->to);
	
	pushll (newl, npiece);
    }

    if (animlist != NULL)
    {
	while ((mpiece = (struct movepiece *) llrembynr (animlist, 1)) != NULL)
	{
	    free (mpiece->from);
	    free (mpiece->to);
	    free (mpiece);
	}
	free (animlist);
    }
    
    animlist = newl;

    return;
}


void fl_board::calc_clipping ()
{
    int counter;
    struct movepiece {
	char piece;
	::position * from;
	::position * to;
    } * mpiece;
    int x1, y1, 
	x2, y2,
	straal;

    mpiece = (struct movepiece *) llitembynr (animlist, 1);
    pos2coor (mpiece->from, x1, y1);
    counter = 1;
    while ((mpiece = (struct movepiece *) llitembynr (animlist, counter))
	   != NULL)
    {
	counter++;
	pos2coor (mpiece->to, x2, y2);
    }
    straal = base / 12;

    /* the -2 and +2 are used for a little security */
    clip_x = min (x1, x2) - straal - 2;
    clip_y = min (y1, y2) - straal - 2;
    clip_w = max (x1, x2) - clip_x + straal + 2;
    clip_h = max (y1, y2) - clip_y + straal + 2;

    return;
}
