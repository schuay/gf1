/*
** $Id: gamelog.c,v 1.12 1998/11/28 13:19:52 kurt Exp $
**
** a lot of things for dealing with the log from a gipf-game
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#include "gamelog.h"

/*
** create a new, empty log structure
**
** parameters:
**    type: gametype (string)
**    wname: name of the player with white
**    bname: name of the player with black
*/
gamelog * newlog (char * type, char * wname, char * bname)
{
    gamelog * log;

    log = (gamelog *) malloc (sizeof (gamelog));
    log->gametype = strdup (type);
    log->whitename = strdup (wname);
    log->blackname = strdup (bname);

    log->moves = (listheader *) malloc (sizeof (listheader));
    newlist (log->moves);

    return (log);
}


/*
** delete a complete gamelog structure
*/
void deletelog (gamelog * log)
{
    logitem * item;
    char * pos;
    
    free (log->gametype);
    free (log->whitename);
    free (log->blackname);

    while ((item = (logitem *) llrembynr (log->moves, 1)) != NULL)
    {
	if (item->plist != NULL)
	{
	    while ((pos = (char *) llrembynr (item->plist, 1)) != NULL)
	    {
		free (pos);
	    }
	    free (item->plist);
	}

	free (item);
    }
    free (log->moves);

    return;
}

/*
** add to the gamelog
**
** the contents of 'data' depend on 'type'
**    LOGMOVE:  <piece>:<from>:<last moved piece>
**    LOGREMGIPF: <gipfowner>:<gipf position><piece>
**    LOGREMROW: <rowowner>:<rowstart>:<rowend>:[<piece position><piece>:]...
**         (remark: the data for LOGREMROW must end on ':')
*/
int addtolog (gamelog * log, int type, char * data)
{
    logitem * item;
    char * kar,
	piece,
	start[10]="  ",
	end[10]="  ",
	pos[10]="  ";

    item = (logitem *) malloc (sizeof (logitem));
    item->plist = NULL;
    item->type = type;
    
    switch (type)
    {
    case LOGMOVE:
	if (sscanf (data, "%c:%2s:%2s", &piece, start, end) != 3)
	{
	    free (item);
	    return (-1);
	}
	break;
    case LOGREMGIPF:
	if (sscanf (data, "%c:%3s", &piece, pos) != 2)
	{
	    free (item);
	    return (-1);
	}
	item->plist = (listheader *) malloc (sizeof (listheader));
	newlist (item->plist);
	pushll (item->plist, (void *) strdup (pos));
	break;
    case LOGREMROW:
	if (sscanf (data, "%c:%2s:%2s:", &piece, start, end) != 3)
	{
	    free (item);
	    return (-1);
	}
	item->plist = (listheader *) malloc (sizeof (listheader));
	newlist (item->plist);
       	kar = data + 8;
	while (kar[0] != '\0')
	{
	    strncpy (pos, kar, 3);
	    pushll (item->plist, (void *) strdup (pos));
	    kar += 4;
	}
	break;
    }
    item->player = piece;
    strcpy (item->start, start);
    strcpy (item->end, end);

    pushll (log->moves, (void *) item);
    return (0);
}


/*
** write contents of a gamelog-structure to a file
**
** parameters:
**    log: gamelog structure
**    fp: pointer to an open file
*/
int logtofile (gamelog * log, FILE * fp)
{
    logitem * item;
    char * pos;
    int counter = 1,
	counter2;

    if (log == NULL)
    {
	return (-1);
    }
    
    fprintf (fp, "game: %s\n", log->gametype);
    fprintf (fp, "white player: %s\n", log->whitename);
    fprintf (fp, "black player: %s\n", log->blackname);
    fprintf (fp, "\nmoves:\n");
    
    while ((item = (logitem *) llitembynr (log->moves, counter)) != NULL)
    {
	counter++;

	switch (item->type)
	{
	case LOGMOVE:
	    fprintf (fp, "%c: %s-%s\n", item->player, item->start, item->end);
	    break;
	case LOGREMGIPF:
	    pos = (char *) llitembynr (item->plist, 1);
	    fprintf (fp, "  %c removegipf: %s\n", item->player, pos);
	    break;
	case LOGREMROW:
	    fprintf (fp, "  %c removerow (%s-%s):", item->player,
		     item->start, item->end);
	    counter2 = 1;
	    while ((pos = (char *) llitembynr (item->plist, counter2)) != NULL)
	    {
		counter2++;
		fprintf (fp, " %s", pos);
	    }
	    fprintf (fp, "\n");
	    break;
	default: ;
	}
    }

    fprintf (fp, "end of gamelog\n");
	
    return (0);
}


/*
** read log from a file and put it in a gamelog-structure
** (the layout must be the same as written by logtofile)
**
** the inputfile must already be open and should be at the first
** line from the gamelog
*/
gamelog * logfromfile (FILE * fp)
{
    gamelog * log;
    char buffer[100],
	type[100],
	wname[100],
	bname[100];
    logitem * item;
    char * kar,
	piece,
	start[10]="  ",
	end[10]="  ",
	pos[10]="  ";

    if (fgets (buffer, 100, fp) == NULL)
	return (NULL);
    buffer[strlen(buffer)-1] = '\0';
    if (strncmp (buffer, "game: ", 6) != 0)
	return (NULL);
    strcpy (type, buffer+6);
    
    if (fgets (buffer, 100, fp) == NULL)
	return (NULL);
    buffer[strlen(buffer)-1] = '\0';
    if (strncmp (buffer, "white player: ", 14) != 0)
	return (NULL);
    strcpy (wname, buffer+14);
    
    if (fgets (buffer, 100, fp) == NULL)
	return (NULL);
    buffer[strlen(buffer)-1] = '\0';
    if (strncmp (buffer, "black player: ", 14) != 0)
	return (NULL);
    strcpy (bname, buffer+14);

    log = newlog (type, wname, bname);
    
    fgets (buffer, 100, fp);
    fgets (buffer, 100, fp);

    while (1)
    {
	if (fgets (buffer, 100, fp) == NULL)
	{
	    deletelog (log);
	    return (NULL);
	}
	if (strncmp (buffer, "end of gamelog", 14) == 0)
	    break;

	item = (logitem *) malloc (sizeof (logitem));
	item->plist = NULL;
	
	if (sscanf (buffer, "%c: %2s-%2s", &piece, start, end) == 3)
	{
	    item->type = LOGMOVE;
	}
	else if (sscanf (buffer, " %c removegipf: %3s", &piece, pos) == 2)
	{
	    item->type = LOGREMGIPF;
	    item->plist = (listheader *) malloc (sizeof (listheader));
	    newlist (item->plist);
	    pushll (item->plist, (void *) strdup (pos));
	}
	else if (sscanf (buffer, " %c removerow (%2s-%2s):",
			 &piece, start, end) == 3)
	{
	    item->type = LOGREMROW;
	    item->plist = (listheader *) malloc (sizeof (listheader));
	    newlist (item->plist);

	    /* find pieces */
	    kar = strchr (buffer, ':');
	    kar++;
	    while (1)
	    {
		while (isspace ((int) kar[0]))
		    kar++;
		if (sscanf (kar, "%3s", pos) != 1)
		    break;
		pushll (item->plist, (void *) strdup (pos));
		kar += 4;
	    }
	}
	item->player = piece;
	strcpy (item->start, start);
	strcpy (item->end, end);
	
	pushll (log->moves, (void *) item);
    }
    
    return (log);
}


/*
** take the contents of a gamelog-structure and format it in
** a nice way to show in an fltk browser-widget
**
** this function returs a list of lines that can be sent to the
** widget without change
**
** !! don't forget to cleanup the list !!
*/
listheader * logtobrowser (gamelog * log)
{
    listheader * lines;
    char * newline,
	whitepieces[50] = "",
	blackpieces[50] = "",
	owner = ' ',
	* tempstr;
    int movecounter = 0,
	count = 1,
	count2;
    logitem * item;

    lines = (listheader *) malloc (sizeof (listheader));
    newlist (lines);

    newline = (char *) malloc (40); /* is 40 always enough ? */
    sprintf (newline, "@b%s", log->gametype);
    pushll (lines, newline);
    newline = (char *) malloc (1);
    newline[0] = '\0';
    pushll (lines, newline);

    newline = (char *) malloc (40); /* is 40 always enough ? */
    sprintf (newline, "@iwhite: %s", log->whitename);
    pushll (lines, newline);
    newline = (char *) malloc (40); /* is 40 always enough ? */
    sprintf (newline, "@iblack: %s", log->blackname);
    pushll (lines, newline);
    newline = (char *) malloc (1);
    newline[0] = '\0';
    pushll (lines, newline);

    while ((item = (logitem *) llitembynr (log->moves, count)) != NULL)
    {
	count++;

	if ((owner != ' ') &&
	    ((item->type == LOGMOVE) || (item->type == LOGREMROW)))
	{
	    newline = (char *) malloc (50); /* is 50 enough ? */
	    if (owner == 'o')
		sprintf (newline, "@s   w: %sx %s",
			 whitepieces, blackpieces);
	    else
		sprintf (newline, "@s   b: %sx %s",
			 blackpieces, whitepieces);

	    pushll (lines, newline);
	    
	    owner = ' ';
	    whitepieces[0] = '\0';
	    blackpieces[0] = '\0';
	}
	
	if (item->type == LOGMOVE)
	{
	    if ((movecounter % 2) == 0)
	    {   /* need to show the nr of the move here */
		newline = (char *) malloc (40); /* is 40 always enough ? */
		sprintf (newline, "@c@b%d", movecounter/2 + 1);
		pushll (lines, newline);
	    }
	    movecounter++;

	    newline = (char *) malloc (40); /* is 40 always enough ? */
	    if (tolower (item->player) == 'o')
		sprintf (newline, "white: ");
	    else
		sprintf (newline, "black: ");

	    if ((item->player == 'O') || (item->player == 'X'))
		sprintf (newline + 7, "G");
	    
	    sprintf (newline + strlen (newline), "%2s-%2s",
		     item->start, item->end);
	    
	    pushll (lines, newline);
	}
	else if (item->type == LOGREMGIPF)
	{
	    tempstr = (char *) llitembynr (item->plist, 1);
	    if (tolower (tempstr[2]) == 'o')
		sprintf (whitepieces + strlen (whitepieces), "G%2.2s ",
			 tempstr);
	    else
		sprintf (blackpieces + strlen (blackpieces), "G%2.2s ",
			 tempstr);
	}
	else /* must be LOGREMROW now */
	{
	    count2 = 1;
	    owner = tolower (item->player);
	    
	    while ((tempstr = (char *) llitembynr (item->plist, count2))
		   != NULL)
	    {
		count2++;
		if (tolower (tempstr[2]) == 'o')
		    sprintf (whitepieces + strlen (whitepieces), "%2.2s ",
			     tempstr);
		else
		    sprintf (blackpieces + strlen (blackpieces), "%2.2s ",
			     tempstr);
	    }
	}
    }

    if (owner != ' ')
    {
	newline = (char *) malloc (50); /* is 50 enough ? */
	if (owner == 'o')
	    sprintf (newline, "@s   w: %sx %s",
		     whitepieces, blackpieces);
	else
	    sprintf (newline, "@s   b: %sx %s",
		     blackpieces, whitepieces);

	pushll (lines, newline);
    }

    return (lines);
}


/*
** return 1 if this logitem represents a move
*/
int isamove (void * data)
{
    logitem * item = data;

    if (item->type == LOGMOVE)
	return (1);

    return (0);
}


/*
** remove the last move from a gamelog
** (include all the removerows and removegipfs)
**
** returns:
**    -1: error
**    number of items removed from the log
*/
int remlastmove (gamelog * log)
{
    int nr,
	counter = 0;
    logitem * item;
    char * pos;
    
    if ((nr = findlastll (log->moves, isamove)) == 0)
    {
	return (-1);
    }

    /* delete everything from the last move to the end of the list */
    while ((item = (logitem *) llrembynr (log->moves, nr)) != NULL)
    {
	if (item->plist != NULL)
	{
	    while ((pos = (char *) llrembynr (item->plist, 1)) != NULL)
	    {
		free (pos);
	    }
	    free (item->plist);
	}

	free (item);
	counter++;
    }
    
    return (counter);
}
