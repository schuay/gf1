/*
** $Id: gamelog.h,v 1.11 2000/01/25 19:17:17 kurt Exp $
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

#ifndef _GAMELOG_H_
#define _GAMELOG_H_ 1

#include "linklist.h"
#include "xmlite.h"

typedef struct {
    char * gametype;
    char * whitename;
    char * blackname;
    listheader * moves;
} gamelog;

typedef struct {
    int type;           /* LOGMOVE, LOGREMGIPF, LOGREMROW */
    char start[3];      /* from   ,           , startpos  */
    char end[3];        /* endmove,           , endpos    */
    char player;        /* piece  , owner     , owner     */
    listheader * plist; /*        , pos       , pieces    */
} logitem;

#define LOGMOVE       1
#define LOGREMGIPF    2
#define LOGREMROW     3

#define loglength(log)           lllength (log->moves)
#define logitemnr(log,nr)        (logitem *) llitembynr (log->moves, nr)
#define logitem_type(logitem)    logitem->type
#define logitem_start(logitem)   logitem->start
#define logitem_end(logitem)     logitem->end
#define logitem_player(logitem)  logitem->player
#define logitem_plist(logitem)   logitem->plist

#ifdef __cplusplus
extern "C" {
#endif

gamelog * newlog (char * type, char * wname, char * bname);
void deletelog (gamelog * log);
int addtolog (gamelog * log, int type, char * data);
int logtofile (gamelog * log, FILE * fp);
xmlite_entity * logtoxml (gamelog * log);
gamelog * logfromxml (xmlite_entity * root);
gamelog * logfromfile (FILE * fp); 
listheader * logtobrowser (gamelog * log);
int remlastmove (gamelog * log);

#ifdef __cplusplus
}
#endif

#endif
