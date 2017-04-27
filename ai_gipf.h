/*
** $Id: ai_gipf.h,v 1.8 1999/07/11 15:53:34 kurt Exp $
**
** subclass of ai_player for a computer GIPF player
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

#include <vector>
#include <deque>
#include "ai_player.h"
#include "board.h"

class ai_gipf : public ai_player {
 private:
    char colour;
    int game;
    int remoppgipf;
    int maxgipf;
    int randomfirstmove;
    vector<basemove *> * movelist;

    int evalfunc (void * game, int maxplayer);
    void listfunc (void * game, vector<basemove *> &);
    void * newfunc (void * game, basemove * move, int * finished,
		    int * depthdelta, int * nodetype);
#if 0
    void * copymovefunc (void * move);
    void delmovefunc (void * move);
#endif
    void delgamefunc (void * game);
    int stopfunc (void);

    void cleanupmovelist (vector<basemove *> * mlist);
    
 public:
    ai_gipf (char playercolour, int gametype);
    ~ai_gipf ();

    void game_move (board * oboard, float timeleft,
		    char * type, char * from, char * to);
    char game_gipf (board * oboard, float timeleft, char * pos);
    char game_row (board * oboard, float timeleft, char * start, char * end);
};


enum { // value for the type-field in gipf_move
    G_MOVE,
    G_REMGIPF,
    G_REMROW
};


class gipf_move : public basemove {
 private:
 public:
    gipf_move () {};
    ~gipf_move ();

    virtual basemove * copy ()   ;

    int type;              // G_MOVE, G_REMGIPF or G_REMROW

//    union {
	struct {
	    int nr;
	} piece;
	struct {
	    char from[3];
	    char to[3];
	    char owner;
	} remrow;
	struct {
	    vector<char *> gipflist;
//	    listheader * gipflist; // (items are char-strings)
	    char owner;
	} remgipf;
//    };
};


/*
** C-functions for calling from the main program
*/
extern "C" {
void * ai_gipf_new (char colour, int game);
void ai_gipf_move (board * oboard, void * self, float timeleft, 
		 char * type, char * from, char * to);
char ai_gipf_gipf (board * oboard, void * self, float timeleft, char * pos);
char ai_gipf_row (board * oboard, void * self, float timeleft,
		  char * start, char * end);
void ai_gipf_end (void * self);
}
