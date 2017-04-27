/*
** $Id: player_info.h,v 1.5 1999/06/01 16:48:24 kurt Exp $
**
** pointers to data about players
*/
/*
**    Copyright (C) 1998-1999 Kurt Van den Branden
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

//#include "ai_minimax.h"
//#include "ai_mtdf.h"
#include "win_human.h"
#include "ai_gipf.h"

struct {
    char playername[20];
    void * (* newfunc)(char colour, int game);
    void (* movefunc)(board * oboard, void * self, float time, 
		      char * type, char * from, char * to);
    char (* gipffunc)(board * oboard, void * self, float time,
		      char * pos);
    char (* rowfunc)(board * oboard, void * self, float time,
		     char * start, char * end);
    void (* endfunc)(void * self);
} players[] = {
    {"human", human_new, human_move, human_gipf, human_row, human_end},

//    {"ai minimax", minimax_new, minimax_move, minimax_gipf,
//     minimax_row, minimax_end}
    {"ai new", ai_gipf_new, ai_gipf_move, ai_gipf_gipf, ai_gipf_row,
     ai_gipf_end}
};
#define playertypes 2

#define pnr(x)   (x == 'o'? 0 : 1)

