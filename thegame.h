/*
** $Id: thegame.h,v 1.16 2000/01/25 19:18:47 kurt Exp $
**
** structures for playing a gipf-game
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

#ifndef _NEWGAME_H_
#define _NEWGAME_H_ 1

#include "board.h"
#include "gamelog.h"
#include "setup_win.h"

enum {
    GAME_GO,
    GAME_STOP
};

typedef struct {
    struct {
	int type;
	char name[40];
	int fulltime;    // in seconds
	float timer;     // in seconds
	void * self;
    } player [2];
    
    int game_type;
    int state;           // GAME_GO, GAME_STOP
    board * boards[3];   // 0-> current board, 1->previous board, ...
    gamelog * movelog;
    int movecounter;     // last item from the movelog checked with
                         // analysemove
    configvalues * config;
} gamestruct;

gamestruct * show_new (gamestruct *);
gamestruct * show_start (board *);
gamestruct * show_onemove (board *, configvalues * conf);
board * verify_board (board * sboard);
void delete_game (gamestruct * gameptr);
void savegame (gamestruct * gameptr);
gamestruct * oldloadgame (char * filename);
gamestruct * loadgame (void);
void setupmove (gamestruct * thegame);
void gipf_questions (gamestruct * thegame);
void row_questions (gamestruct * thegame);
void computermove (gamestruct * thegame);
int humanmove (gamestruct * thegame);
void analysemove (gamestruct * thegame, int playertype);
void showmove (gamestruct * thegame, int playertype, logitem * item);
void showremgipf (gamestruct * thegame, int playertype, logitem * item);
void showremrow (gamestruct * thegame, int playertype, logitem * item);

void updatetimer (void * data);
void stoptimer (gamestruct * thegame);
void starttimer (gamestruct * thegame, char color);

void show_makegif (board * game);

#endif
