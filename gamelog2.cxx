/*
** $Id: gamelog2.cxx,v 1.3 2000/02/28 19:49:55 kurt Exp $
**
** a lot of things for dealing with the log from a gipf-game
**
** the original of this file was gamelog.c
** but because I had to add c++ code I had to rename it
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
#include <string>

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

xmlite_entity * logtoxml (gamelog * log)
{
    xmlite_entity *x1, *x2, *x3;
    char tempstr[100],
	* strptr;
    int counter = 1,
	counter2;
    logitem * item;
    char * pos;

    x1 = new xmlite_entity ("gamelog");
    x1->addattribute ("gametype", log->gametype);
    x1->addattribute ("whiteplayer", log->whitename);
    x1->addattribute ("blackplayer", log->blackname);
    
    while ((item = (logitem *) llitembynr (log->moves, counter)) != NULL)
    {
	counter++;

	switch (item->type)
	{
	case LOGMOVE:
	    x2 = new xmlite_entity ("move");
	    x1->addcontent (x2);
	    x2->addattribute ("start", item->start);
	    x2->addattribute ("end", item->end);
	    sprintf (tempstr, "%c", item->player);
	    x2->addattribute ("piece", tempstr);
	    break;
	case LOGREMGIPF:
	    x2 = new xmlite_entity ("removegipf");
	    x1->addcontent (x2);
	    sprintf (tempstr, "%c", item->player);
	    x2->addattribute ("owner", tempstr);
	    pos = (char *) llitembynr (item->plist, 1);
	    x2->setvalue (pos);
	    break;
	case LOGREMROW:
	    x2 = new xmlite_entity ("removerow");
	    x1->addcontent (x2);
	    x2->addattribute ("start", item->start);
	    x2->addattribute ("end", item->end);
	    sprintf (tempstr, "%c", item->player);
	    x2->addattribute ("owner", tempstr);

	    counter2 = 1;
	    while ((pos = (char *) llitembynr (item->plist, counter2)) != NULL)
	    {
		counter2++;

		x3 = new xmlite_entity ("position");
		x2->addcontent (x3);
		x3->setvalue (pos);
	    }
	    break;
	default: ;
	}
    }

    return (x1);
}

gamelog * logfromxml (xmlite_entity * root)
{
    gamelog * log;
    int counter = 0,
	counter2;
    xmlite_entity * move,
	* pos;
    logitem * item;

    if (root->getname () != "gamelog")
	return (NULL);

    log = newlog (root->getattribute ("gametype").c_str(),
		  root->getattribute ("whiteplayer").c_str(),
		  root->getattribute ("blackplayer").c_str());

    while ((move = root->getcontentbynr (counter)) != NULL)
    {
	counter++;

	item = (logitem *) malloc (sizeof (logitem));
	item->plist = NULL;
	pushll (log->moves, (void *) item);

	if (move->getname () == "move")
	{
	    item->type = LOGMOVE;
	    item->player = (move->getattribute("piece"))[0];
	    strcpy (item->start, move->getattribute("start").c_str());
	    strcpy (item->end, move->getattribute("end").c_str());
	}
	else if (move->getname() == "removerow")
	{
	    item->type = LOGREMROW;
	    item->player = (move->getattribute("owner"))[0];
	    strcpy (item->start, move->getattribute("start").c_str());
	    strcpy (item->end, move->getattribute("end").c_str());
	    item->plist = (listheader *) malloc (sizeof (listheader));
	    newlist (item->plist);
	    counter2 = 0;
	    while ((pos = move->getcontentbynr (counter2)) != NULL)
	    {
		counter2++;

		pushll (item->plist, 
			(void *) strdup (pos->getvalue().c_str()));
	    }
	}
	else if (move->getname() == "removegipf")
	{
	    item->type = LOGREMGIPF;
	    item->player = (move->getattribute("owner"))[0];
	    item->plist = (listheader *) malloc (sizeof (listheader));
	    newlist (item->plist);
	    pushll (item->plist, (void *) strdup (move->getvalue().c_str()));
	}
	else
	{
	    printf ("\nERROR: not a valid move-entry\n\n");
	}
    }

    return (log);
}


/*
** write contents of a gamelog-structure to a file
** in the format appropriate for writing down a gipf-game
**
** parameters:
**    log: gamelog structure
**    fp: pointer to an open file
*/
int logtofile (gamelog * log, FILE * fp)
{
    char player = 'o',
	newplayer,
	* tempstr;
    logitem * item;
    int count = 1,
	movecounter = 0,
	textlength = 0,
	before = 1,
	totallength = 0,
	i,
	count2;
    string move,
	remove,
	removerow,
	removewhite = "",
	removeblack = "";

    fprintf (fp, " %s\n\n", log->gametype);

    fprintf (fp, "    %-37.37s %s\n\n01. ", 
	     log->whitename, log->blackname);

    while ((item = (logitem *) llitembynr (log->moves, count)) != NULL)
    {
	count++;

	if ((item->type == LOGMOVE) ||
	    (item->type == LOGREMROW))
	{
	    if ((removewhite.length() > 0) ||
		(removeblack.length() > 0))
	    {
		if (player == 'o')
		    remove = removewhite + removerow + removeblack;
		else
		    remove = removeblack + removerow + removewhite;

		if (before)
		{
		    fprintf (fp, "%s; ", remove.c_str());
		    totallength += 2 + remove.length();
		}
		else
		{
		    fprintf (fp, " ; %s", remove.c_str());
		    totallength += 3 + remove.length();
		}
		removewhite = "";
		removeblack = "";
	    }

	    // check if the next action is for the other player
	    newplayer = tolower (item->player);
	    if (newplayer != player)
	    {
		if (newplayer == 'o')
		{
		    fprintf (fp, "\n%2.2d. ", movecounter/2 + 1);
		}
		else
		{
		    for (i = totallength; i < 38; i++)
			fprintf (fp, " ");
		}
		totallength = 0;
		player = newplayer;
		before = 1;
	    }
	}

	if (item->type == LOGMOVE)
	{
	    movecounter++;
			
	    move = "";
	    if ((item->player == 'O') || (item->player == 'X'))
		move += "G";
			
	    move += item->start;
	    move += "-"; 
	    move += item->end;
	    fprintf (fp, "%s", move.c_str());

	    totallength += move.length();
	    before = 0;
	}
	else if (item->type == LOGREMGIPF)
	{
	    tempstr = (char *) llitembynr (item->plist, 1);
	    if (tolower (tempstr[2]) == 'o')
	    {
		removewhite += 'G';
		removewhite += tempstr[0];
		removewhite += tempstr[1];
		removewhite += ' ';
	    }
	    else
	    {
		removeblack += 'G';
		removeblack += tempstr[0];
		removeblack += tempstr[1];
		removeblack += ' ';
	    }
	}
	else /* must be LOGREMROW now */
	{
	    removerow = "x(";
	    removerow += item->start;
	    removerow += "-"; 
	    removerow += item->end;
	    removerow += ") ";

	    count2 = 1;
	    while ((tempstr = (char *) llitembynr (item->plist, count2))
		   != NULL)
	    {
		count2++;
		if (tolower (tempstr[2]) == 'o')
		{
		    removewhite += tempstr[0];
		    removewhite += tempstr[1];
		    removewhite += ' ';
		}
		else
		{
		    removeblack += tempstr[0];
		    removeblack += tempstr[1];
		    removeblack += ' ';
		}
	    }
	}
    }

    if ((removewhite.length() > 0) ||
	(removeblack.length() > 0))
    {
	if (player == 'o')
	    remove = removewhite + removerow + removeblack;
	else
	    remove = removeblack + removerow + removewhite;

	if (before)
	    fprintf (fp, "%s; ", remove.c_str());
	else
	    fprintf (fp, " ; %s", remove.c_str());
    }
    fprintf (fp, "\n");

    return (0);
}


/*
** read log from a file and put it in a gamelog-structure
** (the layout must be the same as written by logtofile)
**
** the inputfile must already be open and should be at the first
** line from the gamelog
**
** REMARK: this is only used to read logs in the old fileformat 
**         (before version 1.02)
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
** this function returns a list of lines that can be sent to the
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
    logitem * item = (logitem *) data;

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
