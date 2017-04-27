/*
** $Id: position.h,v 1.4 1998/11/01 18:50:42 kurt Exp $
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

#ifndef _POSITION_H_
#define _POSITION_H_ 1

#include <stdio.h>
#include <malloc.h>
#include "linklist.h"

typedef struct {
    int col;
    int row;
} position;

#define posp_col(pos)      pos->col
#define posp_row(pos)      pos->row
#define pos_col(pos)      pos.col
#define pos_row(pos)      pos.row

#ifdef __cplusplus
#define new_position()   (::position *) malloc (sizeof (::position))
#define del_position(x)      free ((::position *) x)
extern "C" position * strtopos (char *);
extern "C" char * postostr (position * pos);
extern "C" void del_position_f (void * item);
extern "C" listheader * copy_position_row (listheader * orig);
extern "C" void * copy_position (void * orig);
#else
#define new_position()   (position *) malloc (sizeof (position))
#define del_position(x)      free ((position *) x)
void del_position_f (void * item);
listheader * copy_position_row (listheader * orig);
void * copy_position (void * orig);
position * strtopos (char * str);
char * postostr (position * pos);
#endif

#endif
