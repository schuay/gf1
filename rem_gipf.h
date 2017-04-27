/*
** $Id: rem_gipf.h,v 1.2 1998/11/01 18:52:34 kurt Exp $
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

#ifndef _REM_GIPF_H_
#define _REM_GIPF_H_ 1

#include <stdio.h>
#include "position.h"
#include "linklist.h"

typedef struct {
    position * pos;
    char owner;
} rem_gipf;
#define new_rem_gipf()   (rem_gipf *) malloc (sizeof (rem_gipf))

#ifndef __cplusplus
void * copy_rem_gipf (void * orig);
listheader * copy_rem_gipf_row (listheader * orig);
void del_rem_gipf (void * orig);
#else
extern "C" void * copy_rem_gipf (void * orig);
extern "C" listheader * copy_rem_gipf_row (listheader * orig);
extern "C" void del_rem_gipf (void * orig);
#endif

#endif
