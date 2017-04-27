/*
** $Id: fl_pile.cxx,v 1.8 1998/11/01 16:30:53 kurt Exp $
**
** all that is necessary for drawing a pile of pieces
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

#include <FL/fl_draw.H>
#include "fl_pile.h"

#ifndef min
#define min(x,y)       (x < y ? x : y)
#endif

#ifndef max
#define max(x,y)       (x > y ? x : y)
#endif

#define round(x)       (int)(x + 0.5)


/*
** constructor
*/
fl_pile::fl_pile(int X,int Y,int W,int H,const char *l) 
    : Fl_Widget (X, Y, W, H, l) 
{
    nrpieces = 0;
    color = 'o';

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
fl_pile::~fl_pile ()
{
    return;
}


/*
** I don't think I have to handle anything here
*/
int fl_pile::handle (int event)
{
    switch (event)
    {
    default:
	return (0);
    }
}


void fl_pile::draw ()
{
    int i,
	x1, y1;

    /* set everything to background color */
    fl_color (Fl_Widget::color());
    fl_rectf (x()+1, y()+1, w()-2, h()-2);

    draw_box ();

    if (color == 'o')
	fl_color (lyellow); // yellow
    else
	fl_color (FL_BLACK); // black

    for (i = 1; i <= nrpieces; i++)
    {
	x1 = xoffset;
	y1 = yoffset - round (i * pieceh * 1.5);

	fl_rectf (x1, y1, piecew, pieceh);
    }

    fl_color (FL_BLACK);
    for (i = 1; i <= nrpieces; i++)
    {
	x1 = xoffset;
	y1 = yoffset - round (i * pieceh * 1.5);

	fl_rect (x1, y1, piecew, pieceh);
    }

    return;
}


void fl_pile::resize (int x, int y, int w, int h)
{
    Fl_Widget::resize (x, y, w, h); 

    calcsizes ();

    return;
}


/*
** calculate 
**    xoffset
**    yoffset
**    piecew
**    pieceh
**
** to be used at every resize-event and at class-creation
*/
void fl_pile::calcsizes ()
{
    int height,
	width;

    width = round (w() / 1.5);
    height = round (h() / 5.5);
    piecew = min (width, height);
    pieceh = (int) (piecew / 5.0);

    xoffset = round((w() - piecew) / 2) + x();
    yoffset = h() + y();

    return;
}


void fl_pile::setvalue (int val)
{
    nrpieces = val;
    redraw ();

    return;
}


void fl_pile::setcolor (char col)
{
    color = col;
    redraw ();

    return;
}

