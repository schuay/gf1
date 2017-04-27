 /*
** $Id: drawgif.h,v 1.3 2000/01/25 19:16:45 kurt Exp $
**
** include file for the drawgif-class
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

#ifndef _DRAWGIF_H_
#define _DRAWGIF_H_

#include "board.h"
#include "linklist.h"

class drawgif {
 private:
    char * filen;
    int size;
    int colour;
    board * gipfb;
    listheader * extratext;

    double base;
    
    void drawgif::pos2coor (const position *pos, int& x, int& y);
    int splitline (listheader * llist, char * string, int maxchar);

 public:
    drawgif ();
    ~drawgif ();

    void filename (const char * fn);
    void gifsize (int gs) { size = gs; };
    void gifcolour (int gc) { colour = gc; };
    void gifboard (board * gb);
    void addtext (char * text);

    int draw ();
};

#endif
