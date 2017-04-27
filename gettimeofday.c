/*
** $Id: gettimeofday.c,v 1.2 1999/01/07 12:39:51 kurt Exp $
**
** implement gettimeofday() using ftime()
** to be used on systems that don't have a gettimeofday() but
** have an ftime() with a resolution smaller than 1 second
**
**   => Windows
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

#ifdef MSWIN

#include "gettimeofday.h"

int gettimeofday (struct timeval * tv, struct timezone * tz)
{
    struct timeb tp;

    if (tv == 0)
	return (0);

    ftime (&tp);
    tv->tv_sec = tp.time;
    tv->tv_usec = tp.millitm * 1000;

    return (0);
}

#endif
