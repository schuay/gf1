/*
** $Id: fl_pile.h,v 1.4 1998/11/01 16:31:19 kurt Exp $
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

#ifndef _fl_pile_h_
#define _fl_pile_h_ 1

#include <FL/Fl_Widget.H>
	
class fl_pile : public Fl_Widget {
 private:
    int nrpieces;           // number of pieces to draw
    char color;             // color of the pieces (o -> white, x -> black)
    
    int xoffset;            // variables for drawing
    int yoffset;
    int piecew;
    int pieceh;
    uchar lyellow;
    
    void calcsizes ();
    
 protected:
    void draw();

 public:
    fl_pile (int, int, int, int, const char * = 0);
    virtual ~fl_pile();
    int handle (int);
    void resize (int, int, int, int);
    void setvalue (int);
    void setcolor (char);
};

#endif
