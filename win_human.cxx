/*
** $Id: win_human.cxx,v 1.5 1999/06/01 17:03:09 kurt Exp $
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

#include "win_human.h"
#include "callbacks.h"

void chartocolour (char c, char * str);

typedef struct {
    char colour;
    int game;
} human_info;

void * human_new (char colour, int game)
{
    human_info * me;

    me = (human_info *) malloc (sizeof (human_info));
    me->colour = colour;
    me->game = game;

    return ((void *) me);
}


void human_move (board * oboard, void * self, float timeleft, 
		 char * type, char * from, char * to)
{
    human_info * me = (human_info *) self;
    char buffer[10];

    if (*type == 'g')
    {
#ifndef WINGIPF
	printf ("player %c, do you want to add a GIPF (y/n): ", me->colour);
	gets (buffer);
	if ((buffer[0] != 'y') && (buffer[0] != 'Y'))
	{
	    *type = 'n';
	}
#endif
    }

    buffer[0] = '\0';
    while (strlen (buffer) < 5)
    {
#ifndef WINGIPF
	printf ("player %c, please enter your move (ex. h6-g6): ", 
		me->colour);
	gets (buffer);
#endif
    }
    from[0] = buffer[0];
    from[1] = buffer[1];
    to[0] = buffer[3];
    to[1] = buffer[4];

    return;
}


char human_gipf (board * oboard, void * self, float timeleft, char * pos)
{
    human_info * me = (human_info *) self;
    char line1[100],
	line2[100],
	player[10];
    int value;

    chartocolour (me->colour, player);
    sprintf (line1, "%s player, do you want to remove", player);
    sprintf (line2, "the gipf at %s ?", pos);
    value = gf1_question (line1, line2);

    if (value)
    {
	return ('y');
    }
    else
    {
	return ('n');
    }
}


char human_row (board * oboard, void * self, float timeleft, 
		char * start, char * end)
{
    human_info * me = (human_info *) self;
    char line1[100],
	line2[100],
	player[10];
    int value;
    
    chartocolour (me->colour, player);
    sprintf (line1, "%s player, do you want to remove", player);
    sprintf (line2, "the row from %s to %s ?", start, end);
    value = gf1_question (line1, line2);

    if (value)
    {
	return ('y');
    }
    else
    {
	return ('n');
    }
}


void human_end (void * self)
{
    human_info * me = (human_info *) self;

    free (me);
    return;
}


void chartocolour (char c, char * str)
{
    if (c == 'o')
	strcpy (str, "white");
    else
	strcpy (str, "black");

    return;
}
