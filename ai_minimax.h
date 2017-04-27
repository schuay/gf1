/*
** $Id: ai_minimax.h,v 1.2 1998/11/01 16:37:40 kurt Exp $
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

#ifndef _AI_MINIMAX_H_
#define _AI_MINIMAX_H_ 1

#include <stdio.h>
#include <stdlib.h>
#include "linklist.h"
#include "board.h"
#include "configfile.h"

#ifdef __cplusplus
extern "C" {
#endif
void * minimax_new (char colour, int game);
void minimax_move (board * oboard, void * self, 
		 char * type, char * from, char * to);
char minimax_gipf (board * oboard, void * self, char * pos);
char minimax_row (board * oboard, void * self, char * start, char * end);
void minimax_end (void * self);
#ifdef __cplusplus
}
#endif

#endif
