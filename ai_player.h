/*
** $Id: ai_player.h,v 1.7 1999/07/21 13:04:36 kurt Exp $
**
** a class for minimax-searching.
** implements methods for minimax with alfa-beta pruning and for
** mtdf and iterative deepening mtdf
**
** the class defined here must be subclassed for each game
*/
/*
**    Copyright (C) 1999 Kurt Van den Branden
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

#ifndef _AI_PLAYER_H_
#define _AI_PLAYER_H_ 1

#include <sys/time.h>
#include <unistd.h>
#include <vector>
#include "linklist.h"

#ifdef MSWIN
#include "gettimeofday.h"
#endif

#ifndef min
#define min(x, y)     (x < y ? x : y)
#endif
#ifndef max
#define max(x, y)     (x > y ? x : y)
#endif

enum {   // for the type of the searchnode
    MIN,
    MAX
};

enum {   // for the max-player
    PLAYER1,
    PLAYER2,
};

enum {   // for the status after a search is finished
    AI_OK,
    AI_TIMEUP,
    AI_STOPPED,
};


// nothing special, must be subclassed
class basemove {
 private:
 public:
    basemove () {};
//    virtual basemove (basemove &) {}; // copy constructor necessary

    virtual ~basemove () {};

    virtual basemove * copy () { return (new basemove); };
};


// structure containing information about nodes from the searchtree
struct searchnode {
    void * gameposition;
    int finished; /*
		  ** -1: gameposition not calculated yet
		  ** 0 : not a leafnode
		  ** 1 : this position is a leaf-node
		  ** 2 : never use this as a leaf-node, not even
		  **     if pas maximum depth
		  */
    int depth;
    int value;
    int upper;
    int lower;
    int type;   // MIN or MAX
    vector<basemove *> movelist;
    vector<searchnode *> childlist;
};


class ai_player {
 private:
    int maxdepth;            // maximum search-depth
    float maxtime;           // maximum allowed search-time in seconds
                             // (only used by mtdf_id)
    int memorydepth;         // maximum search-depth at which to keep
                             // searchnodes in memory
    int randomchoose;        // if 2 searchnodes have the same value
                             // choose one at random if this flag is set.
                             // otherwise, always choose the first one
    int player;              // PLAYER1 or PLAYER2
                             // this will be given to the evaluation-function
                             // as the 2nd parameter
    int lastvalue;           // value of the last move
    int state;               // status after a search is done

    struct timeval basetime;

    int stopnow;             // will be set to 1 if stopfunc() returns 1
    
    virtual int evalfunc (void * game, int maxplayer) = 0;
                 // evaluation function
                 // to be used when the maximum search-depth
                 // has been reached, or the game is finished
                 // (returns a value between -1000 and 1000)
    virtual void listfunc (void * game, vector<basemove *> &) = 0;
                 // function that produces a
                 // list of possible moves starting from
                 // the current game-position
    virtual void * newfunc (void * game, basemove * move, int * finished,
			    int * depthdelta, int * nodetype) = 0;
                 // function that produces
                 // a new game-position starting from the
                 // the current position and a move
                 // the third parameter is a flag to show
                 // if the game is finished.
                 // (also signals a change in searchdepth and
                 //  the nodetype of the new node => MIN or MAX)
                 // (the function must return NULL if the move is invalid)
#if 0
    virtual void * copymovefunc (void * move) = 0;
                 // a function for copying an item
                 // from the list as produced by 'listfunc'
    virtual void delmovefunc (void * move) = 0;
                 // a function for deleting an item
                 // from the list as produced by 'listfunc'
#endif
    virtual void delgamefunc (void * game) = 0;
                 // a function for deleting a
                 // a game-position as returned by 'newfunc'
    virtual int stopfunc (void) = 0;
                 // stop searching immediatly if this function returns 1.
                 // this function will be called regularly, so it can also
                 // be used for event-checking and things like that.
                 // minimax_ab, mtdf and mtdf_id will return NULL when
                 // stopfunc returns 1.
                 // (preferably make this function inline)
    
//    listheader * minimax_ab (searchnode * node, int alfa, int beta);
    vector<basemove *> * ab_mem (searchnode * node, int alfa, int beta);
    vector<basemove *> * mtdf (searchnode * node);

    int outoftime (void);
    int halfoutoftime (void);
    
    inline searchnode * nodechild (searchnode * node, int nr);
    void delmemtree (searchnode * node, int flag = 1);
    inline void delmovelist (vector<basemove *> * todel);
    void reset_ul (searchnode * node);

    int equ_h (int nr1, int nr2, int * chance);
    int equ_l (int nr1, int nr2, int * chance);

 public:
    ai_player ();
    virtual ~ai_player ();

    void searchdepth (int value) { maxdepth = value; };
    int searchdepth (void) { return (maxdepth); };
    void random (int flag) { if (flag > 0) randomchoose = 1;
                                else randomchoose = 0; };
    void memdepth (int value) { memorydepth = value; };
    void maxplayer (int value) { player = value; };
    int maxplayer (void) { return (player); };
    int gamevalue (void) { return (lastvalue); };
    int status (void) { return (state); };
    
    // the three following functions return a list of items as produced
    // by 'listfunc'
    // this list contains all the moves necessary to get to the best move
    // at the maximum searchdepth. you probably will only need the first item

    // iterative deepening mtdf
    vector<basemove *> * mtdf_id (void * startgame, int starttype = MAX,
			  float mtime = 0.0);
    // normal mtdf
    vector<basemove *> * mtdf (void * startgame, int starttype = MAX);
    // minimax with alfabeta pruning 
    vector<basemove *> * minimax_ab (void * startgame, int starttype = MAX);
};

#endif
