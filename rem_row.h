/*
** $Id: rem_row.h,v 1.4 1999/03/02 20:25:45 kurt Exp $
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

#ifndef _REM_ROW_H_
#define _REM_ROW_H_ 1

#include <stdio.h>
#include "position.h"
#include "linklist.h"

typedef struct {
    position * startpos;
    position * endpos;
    listheader * piecelist;
    int rowindex;
    char owner;     /* the owner is always lowercase */
} rem_row;

#define new_rem_row()   (rem_row *) malloc (sizeof (rem_row))
#define row_owner(x)    x->owner
#define row_start(x)    x->startpos
#define row_end(x)      x->endpos

#ifndef __cplusplus
void * copy_rem_row (void * orig);
listheader * copy_rem_row_row (listheader * orig);
void del_rem_row (void * orig);
#else
extern "C" void * copy_rem_row (void * orig);
extern "C" listheader * copy_rem_row_row (listheader * orig);
extern "C" void del_rem_row (void * orig);
#endif

#endif
