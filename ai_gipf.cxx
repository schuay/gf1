/*
** $Id: ai_gipf.cxx,v 1.14 1999/07/11 15:54:27 kurt Exp $
*/
/*
**    Copyright (C) 1998-1999 Kurt Van den Branden
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

#include <string.h>

#include "stream.h"
#include "ai_gipf.h"
#include "configfile.h"
#include "board.h"

#ifdef WINGIPF
extern "C" void checkwindowevents (void);
extern int interrupt_computer;
#endif

#define TIMEDIVIDER 20

/* 84 different possible moves, with gipf-pieces */
fromto newmoves[] = {
	      {"b1","b2",'g'},        {"a1","b2",'g'},	      {"a2","b2",'g'},
	      {"a2","b3",'g'},	      {"a3","b3",'g'},
	      {"a3","b4",'g'},	      {"a4","b4",'g'},
	      {"a4","b5",'g'},	      {"a5","b5",'g'},	      {"b6","b5",'g'},
	      {"b6","c6",'g'},	      {"c7","c6",'g'},
	      {"c7","d7",'g'},	      {"d8","d7",'g'},
	      {"d8","e8",'g'},	      {"e9","e8",'g'},	      {"f8","e8",'g'},
	      {"f8","f7",'g'},	      {"g7","f7",'g'},
	      {"g7","g6",'g'},	      {"h6","g6",'g'},
	      {"h6","h5",'g'},	      {"i5","h5",'g'},	      {"i4","h5",'g'},
	      {"i4","h4",'g'},	      {"i3","h4",'g'},
	      {"i3","h3",'g'},	      {"i2","h3",'g'},
	      {"i2","h2",'g'},	      {"i1","h2",'g'},	      {"h1","h2",'g'},
	      {"h1","g2",'g'},	      {"g1","g2",'g'},
	      {"g1","f2",'g'},	      {"f1","f2",'g'},
	      {"f1","e2",'g'},	      {"e1","e2",'g'},	      {"d1","e2",'g'},
	      {"d1","d2",'g'},	      {"c1","d2",'g'},
	      {"c1","c2",'g'},	      {"b1","c2",'g'},
	      {"b1","b2",'n'},	      {"a1","b2",'n'},	      {"a2","b2",'n'},
	      {"a2","b3",'n'},	      {"a3","b3",'n'},
	      {"a3","b4",'n'},	      {"a4","b4",'n'},
	      {"a4","b5",'n'},	      {"a5","b5",'n'},	      {"b6","b5",'n'},
	      {"b6","c6",'n'},	      {"c7","c6",'n'},
	      {"c7","d7",'n'},	      {"d8","d7",'n'},
	      {"d8","e8",'n'},	      {"e9","e8",'n'},	      {"f8","e8",'n'},
	      {"f8","f7",'n'},	      {"g7","f7",'n'},
	      {"g7","g6",'n'},	      {"h6","g6",'n'},
	      {"h6","h5",'n'},	      {"i5","h5",'n'},	      {"i4","h5",'n'},
	      {"i4","h4",'n'},	      {"i3","h4",'n'},
	      {"i3","h3",'n'},	      {"i2","h3",'n'},
	      {"i2","h2",'n'},	      {"i1","h2",'n'},	      {"h1","h2",'n'},
	      {"h1","g2",'n'},	      {"g1","g2",'n'},
	      {"g1","f2",'n'},	      {"f1","f2",'n'},
	      {"f1","e2",'n'},	      {"e1","e2",'n'},	      {"d1","e2",'n'},
	      {"d1","d2",'n'},	      {"c1","d2",'n'},
	      {"c1","c2",'n'},	      {"b1","c2",'n'}
};

// constructor
ai_gipf::ai_gipf (char playercolour, int gametype)
{
    listheader * configlist;
    int sd,
	md,
	rc;

    movelist = NULL;
    colour = playercolour;
    game = gametype;

    if (colour == 'o')
	maxplayer (PLAYER1);
    else
	maxplayer (PLAYER2);

    /* check for config-values from the config-file */
    configlist = readconfigfile ("gf1.cfg");
    
    sd = findconfigvalue (configlist, "searchdepth", colour, 3);
    searchdepth (sd);
    md = findconfigvalue (configlist, "memorydepth", colour, 2);
    memdepth (md);
    rc = findconfigvalue (configlist, "randomchoose", colour, 1);
    random (rc);

    remoppgipf = findconfigvalue (configlist, "remoppgipf", colour, 0);
    maxgipf = findconfigvalue (configlist, "maxgipf", colour, 3);
    randomfirstmove = findconfigvalue (configlist, "randomfirstmove", 
				       colour, 1);

    clearconfiglist (configlist);

    return;
}


// destructor
ai_gipf::~ai_gipf ()
{
    if (movelist != NULL)
    {
	int len = movelist->size(),
	    i;

	for (i = 0; i < len; i++)
	    delete (*movelist)[i];

	delete movelist;
    }

    return;
}


static struct {
    position coor;
    int value;
} pos_val[] = {
    {{4, 5}, 5}, 
    {{3, 4}, 2},{{3, 5}, 2},{{4, 6}, 2},{{5, 5}, 2},{{5, 4}, 2},{{4, 4}, 2}, 
    {{2, 3}, 1},{{2, 4}, 1},{{2, 5}, 1},{{3, 6}, 1},{{4, 7}, 1},{{5, 6}, 1}, 
    {{6, 5}, 1},{{6, 4}, 1},{{6, 3}, 1},{{5, 3}, 1},{{4, 3}, 1},{{3, 3}, 1}
};

int ai_gipf::evalfunc (void * game, int maxplayer)
{
    board * oboard = (board *) game;
    int total,
	i,
	value;
    double wvalue,
	bvalue;
    char myc,
	otherc,
	piece;
    position * pos;

    myc = (maxplayer == PLAYER1 ? 'o': 'x');
    otherc = b_opponent (myc);

    if (b_game_finished (oboard))
    {
	if (b_winner (oboard) == myc)
	{
	    return (1000);
	}
	else
	{
	    return (-1000);
	}
    }

    /* maybe I can return 1000 or -1000, but I'm not completely sure */
    /*
    ** not sure at all anymore
    ** what if you start with only GIPF-pieces and remove a GIPF
    ** only at the start of your move
    */
    if (b_colour (oboard, myc) == 0)
    {
	return (-999);
    }
    else if (b_colour (oboard, otherc) == 0)
    {
	return (999);
    }

    /* I need to start with a base-value, or I get a lot of 
    ** problems at the start of tournament games */
    wvalue = 20;
    bvalue = 20;

    /* capturing a piece from your opponent is worth 20 points */
    wvalue += 20 * b_black_lost (oboard);
    bvalue += 20 * b_white_lost (oboard);

    /* 1 point for each piece in use on the board */
    if (b_white_gipf (oboard) == -1)
	total = 15;
    else
	total = 18;

    wvalue += total - b_white (oboard) - b_white_lost (oboard);
    bvalue += total - b_black (oboard) - b_black_lost (oboard);

    /* 2 pieces or less left is getting dangerous */

    /* one gipf left can be dangerous, subtract 5 points */
    if (b_white_gipf (oboard) == 1)
    {
	wvalue -= 5;
    }
    if (b_black_gipf (oboard) == 1)
    {
	bvalue -= 5;
    }

    /* pieces closer to the center have a higher value */
    for (i = 0; i < 19; i++)
    {
	pos = &(pos_val[i].coor);
	piece = b_ppiece (oboard, pos);
	if (piece == '.')
	{
	    continue;
	}
	if (piece == 'o')
	{
	    wvalue += pos_val[i].value;
	}
	else if (piece == 'O')
	{
	    wvalue += pos_val[i].value * 1.5;
	}
	else if (piece == 'x')
	{
	    bvalue += pos_val[i].value;
	}
	else
	{
	    bvalue += pos_val[i].value * 1.5;
	}
    }

    /* normalize the result, should be between 1000 and -1000 */
    if (myc == 'o')
    {
	value = (int) ((wvalue - bvalue) * 1000 / (wvalue + bvalue));
    }
    else
    {
	value = (int) ((bvalue - wvalue) * 1000 / (wvalue + bvalue));
    }

    return (value);
}


int ai_gipf::stopfunc (void)
{
    checkwindowevents ();

    if (interrupt_computer)
	return (1);

    return (0);
}


void ai_gipf::listfunc (void * game, vector<basemove *> & movelist)
{
    board * oboard = (board *) game;
#if 0
    listheader * movelist;

    movelist = (listheader *) malloc (sizeof (listheader));
    newlist (movelist);
#endif

    switch (b_status (oboard))
    {
    case S_NORMAL:
    {
	int firstmove,
	    i;
	gipf_move * move;

	if ((b_colour_type (oboard, b_next_piece (oboard)) == 'g') && 
	    (b_colour_gipf (oboard, b_next_piece (oboard)) < maxgipf))
	{
	    firstmove = 0;
	}
	else
	{
	    firstmove = 42;
	}

	for (i = firstmove; i < 84; i++)
	{
	    if ((i > firstmove) && 
		(newmoves[i].to[0] == newmoves[i-1].to[0]) &&
		(newmoves[i].to[1] == newmoves[i-1].to[1]) &&
		(b_piece (oboard, newmoves[i].to) == '.'))
	    {   /*
		** don't add this move, it would have exactly the same
		** result as the previous one
		*/
		continue;
	    }
	    move = new gipf_move;

	    move->type = G_MOVE;
	    move->piece.nr = i;

//	    pushll (movelist, (void *) move);
	    movelist.push_back (move);
	}
	break;
    }

    case S_REMOVEROW:
    {
	listheader * rowlist;
	gipf_move * move;
	rem_row * rowi;
	char * tempstr;
	int counter = 1;

	rowlist = b_row_extra (oboard);
	while ((rowi = (rem_row *) llitembynr (rowlist, counter)) 
	       != NULL)
	{
	    counter++;

	    move = new gipf_move;

	    move->type = G_REMROW;
	    move->remrow.owner = row_owner (rowi);

	    tempstr = postostr (row_start(rowi));
	    move->remrow.from[0] = tempstr[0];
	    move->remrow.from[1] = tempstr[1];
	    move->remrow.from[2] = '\0';
	    free (tempstr);

	    tempstr = postostr (row_end(rowi));
	    move->remrow.to[0] = tempstr[0];
	    move->remrow.to[1] = tempstr[1];
	    move->remrow.to[2] = '\0';
	    free (tempstr);

//	    pushll (movelist, (void *) move);
	    movelist.push_back (move);
	}
	break;
    }

    case S_REMOVEGIPF:
    {
	gipf_move * move;
	rem_gipf * gipfi;
	int mypieces = 0,
	    counter = 1,
	    maxnr,
	    bits1, bits2;
	char * tempstr;

	while ((gipfi = (rem_gipf *) 
		llitembynr (b_gipf_extra (oboard) ,counter)) != NULL)
	{
	    counter++;

	    if (remoppgipf != 0)
	    {
		if (b_otherpiece (b_ppiece (oboard, gipfi->pos)) 
		    == gipfi->owner)
		{
		    mypieces++;
		}
	    }
	    else
	    {
		mypieces++;
	    }
	}

	maxnr = 1 << mypieces; /* 2 ^ mynr */

	for (bits1 = 0; bits1 < maxnr; bits1++)
	{
	    bits2 = bits1;

	    move = new gipf_move;

	    move->type = G_REMGIPF;
	    
//	    move->remgipf.gipflist = 
//		(listheader *) malloc (sizeof (listheader));
//	    newlist (move->remgipf.gipflist);

	    counter = 1;
	    while ((gipfi = (rem_gipf *) 
		    llitembynr (b_gipf_extra (oboard) ,counter)) != NULL)
	    {
		counter++;
		move->remgipf.owner = gipfi->owner;

		tempstr = b_gipf_position (gipfi);
		tempstr = (char *) realloc (tempstr, 4);
		tempstr[2] = 'n';
		tempstr[3] = '\0';
		
		if ((b_otherpiece (b_ppiece (oboard, gipfi->pos)) 
		     == gipfi->owner) || (remoppgipf == 0))
		{
		    if (bits2 & 1)
		    {
			tempstr[2] = 'y';
		    }
		    bits2 = bits2 >> 1;
		}
		else
		{
		    tempstr[2] = 'y';
		}
//		pushll (move->remgipf.gipflist, tempstr);
		move->remgipf.gipflist.push_back (tempstr);
	    }

//	    pushll (movelist, (void *) move);
	    movelist.push_back (move);
	}
	break;
    }

    case S_FINISHED:     // not normal
	break;
    }

    return;
}


void * ai_gipf::newfunc (void * game, basemove * move, int * finished,
			 int * depthdelta, int * nodetype)
{
    board * oboard = (board *) game,
	* nboard;
    gipf_move * gmove = (gipf_move *) move;
    
    switch (gmove->type)
    {
    case G_MOVE:
    {
	char piece;

	piece = b_next_piece (oboard);

	if (newmoves[gmove->piece.nr].type == 'g')
	    piece = b_otherpiece (piece);
	nboard = b_move (oboard, newmoves[gmove->piece.nr].from, 
			 newmoves[gmove->piece.nr].to, piece);

	if (nboard == NULL)
	    return (NULL);

	break;
    }

    case G_REMGIPF:
    {
	rem_gipf * gipfi;
	int counter1 = 1;
//	int counter2;
	board * tboard;
	char * tempstr;
//	char * str;
	vector<char *>::iterator curg, lastg;
	   
	nboard = oboard;
	while ((gipfi = (rem_gipf *) 
		llitembynr (b_gipf_extra (oboard) ,counter1)) != NULL)
	{
	    counter1++;

	    tempstr = b_gipf_position (gipfi);
//	    counter2 = 1;

	    curg = gmove->remgipf.gipflist.begin();
	    lastg = gmove->remgipf.gipflist.end();
	    while (curg != lastg)
//	    while ((str = (char *) 
//		    llitembynr (gmove->remgipf.gipflist, counter2)) != NULL)
	    {
//		counter2++;
		if (((*curg)[0] == tempstr[0]) && ((*curg)[1] == tempstr[1]))
		{
		    if ((*curg)[2] == 'y')
		    {
			tboard = b_remove_gipf (nboard, gipfi);
			if (nboard != oboard)
			    b_del (nboard);
			nboard = tboard;
		    }
		    break;
		}
		curg++;
	    }

	    free (tempstr);
	}

	/* check again for 4 in a row */
	tboard = b_checkfour (nboard);
	if (nboard != oboard)
	{
	    b_del (nboard);
	}
	nboard = tboard;

	break;
    }

    case G_REMROW:
    {
	listheader * rowlist;
	rem_row * rowi;
	char * tempstr;
	int counter = 1;

	rowlist = b_row_extra (oboard);
	while ((rowi = (rem_row *) llitembynr (rowlist, counter)) 
	       != NULL)
	{
	    counter++;

	    tempstr = postostr (rowi->startpos);
	    if ((tempstr[0] != gmove->remrow.from[0]) || 
		(tempstr[1] != gmove->remrow.from[1]))
	    {
		free (tempstr);
		continue;
	    }
	    free (tempstr);

	    tempstr = postostr (rowi->endpos);
	    if ((tempstr[0] != gmove->remrow.to[0]) || 
		(tempstr[1] != gmove->remrow.to[1]))
	    {
		free (tempstr);
		continue;
	    }
	    free (tempstr);

	    break;
	}
	nboard = b_remove_row (oboard, counter - 1);
	break;
    }
    default:
	break;
    }

    switch (b_status (nboard))
    {
    case S_NORMAL:
	*finished = b_game_finished (nboard);
	*depthdelta = 1;

	if (((b_next_piece (nboard) == 'o') && (maxplayer () == PLAYER1)) ||
	    ((b_next_piece (nboard) == 'x') && (maxplayer () == PLAYER2)))
	    *nodetype = MAX;
	else
	    *nodetype = MIN;
	break;

    case S_REMOVEROW:
    {
	listheader * rowlist = b_row_extra (nboard);
	rem_row * rowi = (rem_row *) llitembynr (rowlist, 1);

	*finished = 2;
	*depthdelta = 0;

	if (((row_owner (rowi) == 'o') && (maxplayer () == PLAYER1)) ||
	    ((row_owner (rowi) == 'x') && (maxplayer () == PLAYER2)))
	    *nodetype = MAX;
	else
	    *nodetype = MIN;
	
	break;
    }

    case S_REMOVEGIPF:
    {
	rem_gipf * gipfi = (rem_gipf *)  llitembynr (b_gipf_extra (nboard) ,1);

	*finished = 2;
	*depthdelta = 0;

	if (((b_gipf_owner (gipfi) == 'o') && (maxplayer () == PLAYER1)) ||
	    ((b_gipf_owner (gipfi) == 'x') && (maxplayer () == PLAYER2)))
	    *nodetype = MAX;
	else
	    *nodetype = MIN;
	break;
    }

    case S_FINISHED:     // not normal
	break;
    }


    return ((void *) nboard);
}


#if 0
//void * ai_gipf::copymovefunc (void * move)
gipf_move::gipf_move (gipf_move & tocopy)
{
//    gipf_move * omove = (gipf_move *) move,
//	* nmove;
    int counter;
    char * opos,
    char * npos;

//    nmove = (gipf_move *) malloc (sizeof (gipf_move));

    this->type = tocopy.type;
    switch (this->type)
    {
    case G_MOVE:
	this->piece.nr = tocopy.piece.nr;
	break;
    case G_REMROW:
	this->remrow.from[0] = tocopy.remrow.from[0];
	this->remrow.from[1] = tocopy.remrow.from[1];
	this->remrow.from[2] = '\0';
	this->remrow.to[0] = tocopy.remrow.to[0];
	this->remrow.to[1] = tocopy.remrow.to[1];
	this->remrow.to[2] = '\0';
	this->remrow.owner = tocopy.remrow.owner;
	break;
    case G_REMGIPF:
	this->remgipf.owner = tocopy.remgipf.owner;

	this->remgipf.gipflist = (listheader *) malloc (sizeof (listheader));
	newlist (this->remgipf.gipflist);
	counter = 1;
	while ((opos = (char *) 
		llitembynr (tocopy.remgipf.gipflist, counter)) != NULL)
	{
	    counter++;
	    npos = (char *) malloc (4);
	    npos[0] = opos[0];
	    npos[1] = opos[1];
	    npos[2] = opos[2];
	    npos[3] = '\0';
	    pushll (this->remgipf.gipflist, (void *) npos);
	}
	break;
    }


    return;
}
#endif

basemove * gipf_move::copy (void)
{
    gipf_move * nmove = new gipf_move;
//    int counter;
//    char * opos,
    char * npos;
    vector<char *>::iterator curg, lastg;

    nmove->type = this->type;
    switch (nmove->type)
    {
    case G_MOVE:
	nmove->piece.nr = this->piece.nr;
	break;
    case G_REMROW:
	nmove->remrow.from[0] = this->remrow.from[0];
	nmove->remrow.from[1] = this->remrow.from[1];
	nmove->remrow.from[2] = '\0';
	nmove->remrow.to[0] = this->remrow.to[0];
	nmove->remrow.to[1] = this->remrow.to[1];
	nmove->remrow.to[2] = '\0';
	nmove->remrow.owner = this->remrow.owner;
	break;
    case G_REMGIPF:
	nmove->remgipf.owner = this->remgipf.owner;

	curg = this->remgipf.gipflist.begin();
	lastg = this->remgipf.gipflist.end();
//	nmove->remgipf.gipflist = (listheader *) malloc (sizeof (listheader));
//	newlist (nmove->remgipf.gipflist);
//	counter = 1;
//	while ((opos = (char *) 
//		llitembynr (this->remgipf.gipflist, counter)) != NULL)
	while (curg != lastg)
	{
//	    counter++;
	    npos = (char *) malloc (4);
	    npos[0] = (*curg)[0];
	    npos[1] = (*curg)[1];
	    npos[2] = (*curg)[2];
	    npos[3] = '\0';
//	    pushll (nmove->remgipf.gipflist, (void *) npos);
	    nmove->remgipf.gipflist.push_back(npos);
	    curg++;
	}
	break;
    }

    return (nmove);
}


//void ai_gipf::delmovefunc (void * move)
gipf_move::~gipf_move ()
{
//    char * opos;

    if ((type == G_REMGIPF) && (!remgipf.gipflist.empty()))
    {
//	while ((opos = (char *) 
//		llrembynr (remgipf.gipflist, 1)) != NULL)
//	    free (opos);
//    
//	free (remgipf.gipflist);
	vector<char *>::iterator curg = remgipf.gipflist.begin(),
	    lastg = remgipf.gipflist.end();
	while (curg != lastg)
	{
	    delete (*curg);
	    curg++;
	}
    }

    return;
}


void ai_gipf::delgamefunc (void * game)
{
    board * oboard = (board *) game;

    if (oboard == NULL)
	return;

    b_del (oboard);

    return;
}


void ai_gipf::game_move (board * oboard, float timeleft, 
			 char * type, char * from, char * to)
{
    vector<basemove *> * newmovelist;
    gipf_move * move;
    int savedepth = -1;

    if (movelist != NULL)
    {
	/* 
	** this is probably a problem 
	** signal it and then ignore it
	*/
	cout << "\n WARNING (ai_gipf::game_move):\n";
	cout << "    movelist is not empty!!\n\n";

	int len = movelist->size(),
	    i;

	for (i = 0; i < len; i++)
	    delete (*movelist)[i];

	delete movelist;
	movelist = NULL;
    }

    /*
    ** if this is the first move of the game, reduce memorydepth
    ** to 1, it's not necessary to waste time when there are really
    ** only 2 possible moves
    */
    /* question: should I only do this for tournament games ? */
    if (randomfirstmove && // flag must be one
	(b_move_counter (oboard) == 0) && // first move
	(((b_white_gipf (oboard) == 0) && 
	  (b_white (oboard) == 18) && (b_black (oboard) == 18)) ||
	 ((b_white_gipf (oboard) == 3) && 
	  (b_white (oboard) == 12) && (b_black (oboard) == 12)) ||
	 ((b_white_gipf (oboard) == -1) && 
	  (b_white (oboard) == 12) && (b_black (oboard) == 12))))
    {
	savedepth = searchdepth ();
	searchdepth (1);
    }

    newmovelist = mtdf_id ((void *) oboard, MAX, timeleft/TIMEDIVIDER);

    /* reset the correct searchdepth if necessary */
    if (savedepth != -1)
    {
	searchdepth (savedepth);
    }

    if (newmovelist == NULL)
    {
	// have to check what happened here
	if (status () == AI_STOPPED)
	    return;
    }

    cleanupmovelist (newmovelist);
    movelist = newmovelist;

    // remove the first item from the movelist
    move = (gipf_move *) (*movelist)[0];
    movelist->erase (movelist->begin());
//    move = (gipf_move *) llrembynr (movelist, 1);

    if (movelist->empty())
    {
	delete movelist;
	movelist = NULL;
    }
//    if (llitembynr (movelist, 1) == NULL)
//    {   // delete movelist if empty
//	free (movelist);
//	movelist = NULL;
//    }

    if (move->type != G_MOVE)
    {   // very wrong, what happened here ?
    }
	
    from[0] = newmoves[move->piece.nr].from[0];
    from[1] = newmoves[move->piece.nr].from[1];
    to[0] = newmoves[move->piece.nr].to[0];
    to[1] = newmoves[move->piece.nr].to[1];
    *type = newmoves[move->piece.nr].type;

    delete move;

    return;
}


char ai_gipf::game_gipf (board * oboard, float timeleft, 
			 char * pos)
{
    vector<basemove *> * newmovelist;
    gipf_move * move;
//    char * gipfpos;
    char result;
    int counter;
    vector<char *>::iterator curg,
	lastg;

    if (movelist == NULL)
    {
	newmovelist = mtdf_id ((void *) oboard, MAX, timeleft/TIMEDIVIDER);

	if (newmovelist == NULL)
	{
	    // have to check what happened here
	    if (status () == AI_STOPPED)
		return (' ');
	}
	
	cleanupmovelist (newmovelist);
	movelist = newmovelist;
    }

//    move = (gipf_move *) llitembynr (movelist, 1);
    move = (gipf_move *) (*movelist)[0];
    if ((move == NULL) || (move->type != G_REMGIPF))
    {   // very wrong, what happened here ?
    }

//    counter = 1;
//    while ((gipfpos = (char *) 
//	    llitembynr (move->remgipf.gipflist, counter)) != NULL)
    curg = move->remgipf.gipflist.begin();
    lastg = move->remgipf.gipflist.end();
    while (curg != lastg)
    {
	if (strncmp (*curg, pos, 2) == 0)
	{
	    break;
	}
//	counter++;
	curg++;
    }

//    if (gipfpos == NULL)
    if (curg == lastg)
    {   // very wrong, gipf-position not found
    }

    result = (*curg)[2];
    free (*curg);
    move->remgipf.gipflist.erase (curg);
//    result = gipfpos[2];
//    gipfpos = (char *) llrembynr (move->remgipf.gipflist, counter);
//    free (gipfpos);

//    if (llitembynr (move->remgipf.gipflist, 1) == NULL)
    if (move->remgipf.gipflist.empty())
    {
	delete move;

	movelist->erase (movelist->begin());

	if (movelist->empty())
	{   // delete movelist if empty
	    delete movelist;
	    movelist = NULL;
	}
    }

    return (result);
}


char ai_gipf::game_row (board * oboard, float timeleft, 
			char * start, char * end)
{
    vector<basemove *> * newmovelist;
    gipf_move * move;
    char result = 'n';

    if (movelist == NULL)
    {
	newmovelist = mtdf_id ((void *) oboard, MAX, timeleft/TIMEDIVIDER);

	if (newmovelist == NULL)
	{
	    // have to check what happened here
	    if (status () == AI_STOPPED)
		return (' ');
	}
	
	cleanupmovelist (newmovelist);
	movelist = newmovelist;
    }

    move = (gipf_move *) (*movelist)[0];
//    move = (gipf_move *) llitembynr (movelist, 1);
    if ((move == NULL) || (move->type != G_REMROW))
    {   // very wrong, what happened here ?
    }

    if (strncmp (start, move->remrow.from, 2) == 0)
    {
	if (strncmp (end, move->remrow.to, 2) == 0)
	    result = 'y';
    }
    else if (strncmp (start, move->remrow.to, 2) == 0)
    {
	if (strncmp (end, move->remrow.from, 2) == 0)
	    result = 'y';
    }
    
    if (result == 'y')
    {
	delete move;

	movelist->erase (movelist->begin());

	if (movelist->empty())
	{   // delete movelist if empty
	    delete movelist;
	    movelist = NULL;
	}
    }

    return (result);
}


/*
** cleanup a list of moves as returned by minimax_ab, mtdf or mtdf_id
** keep only the necessary moves.
** this means:
**    - remove G_MOVE if it is not the first item in the list and remove
**      everything after it. (only keep everything for the first move)
**    - remove everything that is not an action for the current player.
*/
void ai_gipf::cleanupmovelist (vector<basemove *> * mlist)
{
    int counter = 1, /* skip the first item */
	len = mlist->size(),
	i;
    gipf_move * moveitem;

    /* look for the first item that can be deleted */
    for (; counter < len; counter++)
//    while ((moveitem = (gipf_move *) llitembynr (mlist, counter)) != NULL)
    {
	moveitem = (gipf_move *) (*mlist)[counter];
	if ((moveitem->type == G_MOVE) ||
	    ((moveitem->type == G_REMGIPF) && 
	     (moveitem->remgipf.owner != colour)) || 
	    ((moveitem->type == G_REMROW) && 
	     (moveitem->remrow.owner != colour)))
	{
	    break;
	}
//	counter++;
    }

    /* delete everything from now on */
    for (i = len - 1; i >= counter; i--)
//    while ((item = (gipf_move *) llrembynr (mlist, counter)) != NULL)
    {
	delete (* mlist)[i];
	mlist->pop_back();
    }

    return;
}


/*
** C-functions for calling from the main program
*/

void * ai_gipf_new (char colour, int game)
{
    ai_gipf * newgame;

    newgame = new ai_gipf (colour, game);

    return ((void *) newgame);
}

void ai_gipf_move (board * oboard, void * self, float timeleft, 
		   char * type, char * from, char * to)
{
    ai_gipf * game = (ai_gipf *) self;

    game->game_move (oboard, timeleft, type, from, to);
    return;
}

char ai_gipf_gipf (board * oboard, void * self, float timeleft, 
		   char * pos)
{
    ai_gipf * game = (ai_gipf *) self;

    return (game->game_gipf (oboard, timeleft, pos));
}

char ai_gipf_row (board * oboard, void * self, float timeleft, 
		  char * start, char * end)
{
    ai_gipf * game = (ai_gipf *) self;

    return (game->game_row (oboard, timeleft, start, end));
}

void ai_gipf_end (void * self)
{
    ai_gipf * delgame = (ai_gipf *) self;

    delete delgame;

    return;
}
