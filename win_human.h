/*
** $Id: win_human.h,v 1.4 1999/06/01 17:03:45 kurt Exp $
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

#ifndef _WIN_HUMAN_H_
#define _WIN_HUMAN_H_ 1

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

void * human_new (char colour, int game);
void human_move (board * oboard, void * self, float timeleft, 
		 char * type, char * from, char * to);
char human_gipf (board * oboard, void * self, float timeleft, char * pos);
char human_row (board * oboard, void * self, float timeleft,
		char * start, char * end);
void human_end (void * self);

#ifdef __cplusplus
}
#endif

#endif
