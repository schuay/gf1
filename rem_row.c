/*
** $Id: rem_row.c,v 1.2 1998/11/01 18:53:34 kurt Exp $
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

#include "rem_row.h"

listheader * copy_rem_row_row (listheader * orig)
{
    listheader * new;
    
    new = copyll (orig, copy_rem_row);

    return (new);
}

void * copy_rem_row (void * orig)
{
    rem_row * o_item = orig,
	* n_item;

    n_item = new_rem_row();

    n_item->startpos = (position *) copy_position ((void *) o_item->startpos);
    n_item->endpos = (position *) copy_position ((void *) o_item->endpos);
    n_item->piecelist = 
	(listheader *) copy_position_row ((void *) o_item->piecelist);
    n_item->rowindex = o_item->rowindex;
    n_item->owner = o_item->owner;

    return ((void *) n_item);
}

void del_rem_row (void * orig)
{
    rem_row * item = orig;

    if (item != NULL)
    {
	del_position ((void *) item->startpos);
	del_position ((void *) item->endpos);
	emptyll (item->piecelist, del_position_f);

	free (item->piecelist);
	free (item);
    }
    return;
}
