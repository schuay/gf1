/*
** $Id: gettimeofday.h,v 1.3 1999/01/17 14:03:49 kurt Exp $
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

#ifndef _GETTIMEOFDAY_H_
#define _GETTIMEOFDAY_H_ 1

#include <sys/timeb.h>
#include <time.h>

#ifndef timeval
struct timeval {
    time_t tv_sec;              /* Seconds.  */
    time_t tv_usec;             /* Microseconds.  */
};
#endif

#ifndef timezone
struct timezone {
    int tz_minuteswest;         /* Minutes west of GMT.  */
    int tz_dsttime;             /* Nonzero if DST is ever in effect.  */
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
int gettimeofday (struct timeval * tv, struct timezone * tz);
#ifdef __cplusplus
}
#endif

#endif
