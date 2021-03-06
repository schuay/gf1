/*
** $Id: position.c,v 1.2 1998/11/01 18:48:34 kurt Exp $
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

#include "position.h"

listheader * copy_position_row (listheader * orig)
{
    listheader * new;
    
    new = copyll (orig, copy_position);

    return (new);
}


void * copy_position (void * orig)
{
    position * o_item = orig,
	* n_item;

    if (o_item == NULL)
	return (NULL);

    n_item = new_position ();

    n_item->col = o_item->col;
    n_item->row = o_item->row;

    return ((void *) n_item);
}


void del_position_f (void * item)
{
    position * pos = item;

    if (pos != NULL)
    {
	free (pos);
    }
    return;
}


position * strtopos (char * str)
{
    position * ptr;
    int col, row;

    if (str == NULL)
    {
	return (NULL);
    }

    col = str[0] - 'a'; /* should be the colnr macro */
    if ((col < 0) || (col > 8))
    {
	return (NULL);
    }
    row = str[1] - '0';
    if ((row < 1) || (row > 9))
    {
	return (NULL);
    }

    ptr = new_position();
    ptr->col = col;
    ptr->row = row;
    return (ptr);
}

char * postostr (position * pos)
{
    char * str;

    if (pos == NULL)
    {
	return (NULL);
    }

    str = (char *) malloc (3);
    str[0] = pos->col + 'a';
    str[1] = pos->row + '0';
    str[2] = '\0';

    return (str);
}
