/*
** $Id: fl_board.h,v 1.19 2000/02/28 19:59:46 kurt Exp $
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

#ifndef _fl_board_h_
#define _fl_board_h_ 1

#include <FL/Fl_Widget.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Return_Button.H>
#include "board.h"
#include "linklist.h"
#include "fl_pile.h"

enum {
    BOARD_EDIT,
    BOARD_PLAY,
    BOARD_NONE
};

const int FL_DAMAGE_ANIM = 0x10;

class fl_board : public Fl_Widget {
 private:
    board * theboard;       // the most important
    ::position * from;        // from-::position for a new move
    ::position * to;          // to-::position for a new move
    listheader * row;       // list of pieces when removing a row
    int fromtype;           // type of piece a player wants to move
    int gipfpossible;       // flag to show if a player can enter a gipf-piece
    int state;              // BOARD_EDIT, BOARD_PLAY, BOARD_NONE
    char editpiece;         // piece to add when editing a board
    
    int xoffset;            // variables for drawing
    int yoffset;
    int base;
    uchar lyellow;          // save the color-index

    int positionhints;      // set to 1 to show the ::position names on the board
    ::position * showpos;     // variables for showing at what
    int showflag;           // ::position the mouse is situated

    listheader * animlist;  // variables for move-animation
    int maxstep;
    int step;
    int clip_x, clip_y, clip_w, clip_h;
    
    Fl_Output * wpieces;
    Fl_Output * wlost;
    fl_pile * wpile;
    Fl_Output * bpieces;
    Fl_Output * blost;
    fl_pile * bpile;

    Fl_Output * t_from;
    Fl_Output * t_to;

    Fl_Return_Button * execbutton;
    
    void calcsizes ();
    inline void pos2coor (const ::position *, int&, int&);
    void changecountervalues ();
    void changefromtovalues ();
    
    void drawpiece (::position *, char);
    void drawpiece (int, int, char);
    void drawcross (int, int);

    int checkfrompush ();
    int checktopush ();
    int checkeditpush ();
    ::position * getmousepos ();
    void drawanimpieces (void);
    void calc_clipping (void );
    
 protected:
    void draw();

 public:
    fl_board (int, int, int, int, const char * = 0);
    virtual ~fl_board();
    int handle (int);
    void resize (int, int, int, int);
    void setoutputwidgets (Fl_Output *, Fl_Output *, fl_pile *,
			   Fl_Output *, Fl_Output *, fl_pile *,
			   Fl_Output *, Fl_Output *);
    void setexecbutton (Fl_Return_Button * rbutton)
	{ execbutton = rbutton; };
    void setboard (board *);
    void setfrom (const ::position *);
    void setto (const ::position *);
    void setrow (listheader *);
    void seteditpiece (char);
    int setlostwhite (int);
    int setlostblack (int);
    void setstate (int);
    void setgipfpossible (int flag);
    void showmousepos ();
    void setshowposname (int value) { positionhints = value; };
    
    board * getboard (void) { return (theboard); };
    ::position * getfrom (void) { return (from); };
    ::position * getto (void) { return (to); };
    int getptype (void) { return (fromtype); };

    void initanim (int st, listheader * al);
    void animstep (int st) { step = st; damage (FL_DAMAGE_ANIM); };
};

#endif
