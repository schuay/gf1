/*
** $Id: rem_gipf.c,v 1.2 1998/11/01 18:51:29 kurt Exp $
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

#include "rem_gipf.h"

listheader * copy_rem_gipf_row (listheader * orig)
{
    listheader * new;
    
    new = copyll (orig, copy_rem_gipf);

    return (new);
}

void * copy_rem_gipf (void * orig)
{
    rem_gipf * o_item = orig,
	* n_item;

    n_item = new_rem_gipf();

    n_item->pos = (position *) copy_position ((void *) o_item->pos);
    n_item->owner = o_item->owner;

    return ((void *) n_item);
}

void del_rem_gipf (void * orig)
{
    rem_gipf * item = orig;

    del_position ((void *) item->pos);
    free (item);

    return;
}
