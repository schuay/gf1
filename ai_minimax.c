/*
** $Id: ai_minimax.c,v 1.6 1999/05/24 17:22:15 kurt Exp $
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

#include "ai_minimax.h"

#ifdef WINGIPF
extern void checkwindowevents (void);
extern int interrupt_computer;
#endif

#define RANDOMCHOOSE
/*#define PRINTMOVES*/

/* structures for keeping my own info in */
typedef struct {
    char colour;
    int game;
    struct {
	int searchdepth;
	int remoppgipf;
	int maxgipf;
#ifdef RANDOMCHOOSE
	int randomchoose;
#endif
    } config;
    listheader * movelist;  /* list of items of type nextmove */
} minimax_info;

typedef struct {
    int remtype;   /* REM_GIPF or REM_ROW */
    listheader * gipflist; /* list of items of type gipfdata */
    struct {
	char * start;
	char * end;
    } rowdata;
} nextmove;

#define REM_GIPF 1
#define REM_ROW  2

typedef struct {
    char from[3];
    char to[3];
    char type;
    listheader * movelist; /* list of items of type nextmove */
    int value;
} movestruct;

typedef struct {
    board * sboard;
    char to[3];
} saveboard;

typedef struct {
    char pos[3];
    char flag;
} gipfdata;

typedef struct {
    rem_gipf * gipf;
    int flag;
} gipfitem;


#define MINNODE 1
#define MAXNODE 2

#define min(x, y)     (x < y ? x : y)
#define max(x, y)     (x > y ? x : y)

movestruct * alfabeta_move (board * oboard, int alfa, int beta, int minmax,
			    int depth, minimax_info * me);
movestruct * move (board * oboard, int alfa, int beta, int minmax, int depth,
		   minimax_info * me, fromto * todo, saveboard * lmove);
movestruct * alfabeta_gipf (board * oboard, int alfa, int beta, int minmax,
			    int depth, minimax_info * me);
movestruct * remove_gipflist (board * oboard, int alfa, int beta, int minmax,
			      int depth, minimax_info * me, listheader * myg,
			      listheader * otherg);
movestruct * alfabeta_row (board * oboard, int alfa, int beta, int minmax,
			   int depth, minimax_info * me);
movestruct * remove_row (board * oboard, int alfa, int beta, int minmax,
			 int depth, minimax_info * me, int rem);
int evaluate (board * oboard, minimax_info * me);
void del_move (movestruct * todel);

#ifdef RANDOMCHOOSE
int equ_l (int nr1, int nr2, int * chance, minimax_info * me, int depth);
int equ_h (int nr1, int nr2, int * chance, minimax_info * me, int depth);
#endif

void * minimax_new (char colour, int game)
{
    minimax_info * me;
    listheader * configlist;
    
    me = (minimax_info *) malloc (sizeof (minimax_info));
    me->colour = colour;
    me->game = game;
    me->movelist = NULL;

    /* check for config-values from the config-file */
    configlist = readconfigfile ("gf1.cfg");
    
    me->config.searchdepth = findconfigvalue (configlist, "searchdepth",
					      colour, 2);
    me->config.remoppgipf = findconfigvalue (configlist, "remoppgipf",
					     colour, 1);
    me->config.maxgipf = findconfigvalue (configlist, "maxgipf",
					  colour, 3);
#ifdef RANDOMCHOOSE
    me->config.randomchoose = findconfigvalue (configlist, "randomchoose",
					       colour, 1);
#endif

    clearconfiglist (configlist);
    
    return ((void *) me);
}


void minimax_move (board * oboard, void * self, 
		   char * type, char * from, char * to)
{
    minimax_info * me = self;
    int depth = 1;
    movestruct * move; 

    b_nolog (oboard);

    move = alfabeta_move (oboard, -1001, 1001, MAXNODE, depth, me);

    from[0] = move->from[0];
    from[1] = move->from[1];
    to[0] = move->to[0];
    to[1] = move->to[1];
    *type = move->type;

    me->movelist = move->movelist;

#ifdef PRINTMOVES
    printf ("ai_minimax %c (%d): move %2s-%2s\n", 
	    me->colour, move->value, from, to);
#endif

    move->movelist = NULL;
    del_move (move);

    return;
}


char minimax_gipf (board * oboard, void * self, char * pos)
{
    minimax_info * me = self;
    nextmove * movedata;
    gipfdata * gipfd;
    int counter;
    char val;

    b_nolog (oboard);

    /* check if movelist contains info about removing gipf-pieces */
    if (me->movelist == NULL)
    {
	/* 
	** no info in movelist, my opponent made 4 in a row
	** with my pieces (only possible explanation)
	*/
	int depth = 1;
	movestruct * nmove;

	/* remark: I have to start this as a min-node, 
	** because this is really my opponents move */
	nmove = alfabeta_gipf (oboard, -1001, 1001, MINNODE, depth, me);
	me->movelist = nmove->movelist;
	nmove->movelist = NULL;
	del_move (nmove);
    }

    /* now check the movelist for the move we have to make */
    if (((movedata = (nextmove *) llitembynr (me->movelist, 1)) == NULL) || 
	(movedata->remtype != REM_GIPF))
    {
	/* very wrong */
#ifdef PRINTMOVES
        printf ("action removegipf, but nextmove contains something else\n\n");
#endif
	exit (0);
    }
	    
    /* look for the gipf to remove */
    counter = 1;
    while ((gipfd = (gipfdata *) llitembynr (movedata->gipflist, counter)) 
	   != NULL)
    {
	if ((gipfd->pos[0] == pos[0]) && (gipfd->pos[1] == pos[1]))
	{
	    break;
	}
	counter++;
    }
    if (gipfd == NULL)
    {
	/* very wrong */
#ifdef PRINTMOVES
	printf ("action removegipf, but the gipf isn't in nextmove\n\n");
#endif
	exit (0);
    }
    /* remove gipf from list */
    gipfd = (gipfdata *) llrembynr (movedata->gipflist, counter);
    
    /* remove nextmove from movelist if gipflist is empty */
    if (llitembynr (movedata->gipflist, 1) == NULL)
    {
	movedata = (nextmove *) llrembynr (me->movelist, 1);
	free (movedata->gipflist);
	free (movedata);

	/* remove movelist if empty */
	if (llitembynr (me->movelist, 1) == NULL)
	{
	    free (me->movelist);
	    me->movelist = NULL;
	}
    }

    val = gipfd->flag;
    free (gipfd);

#ifdef PRINTMOVES
    printf ("ai_minimax %c: remove gipf %2s (%c)\n", me->colour, pos, val);
#endif
    return (val);
}


char minimax_row (board * oboard, void * self, char * start, char * end)
{
    minimax_info * me = self;
    nextmove * movedata;
    int correct;

    b_nolog (oboard);

    /* check if movelist contains info about removing rows */
    if (me->movelist == NULL)
    {
	/* 
	** no info in movelist, my opponent made several "4 in a row"s
	** with my pieces (only possible explanation)
	*/
	int depth = 1;
	movestruct * nmove;

	/* remark: I have to start this as a min-node, 
	** because this is really my opponents move */
	nmove = alfabeta_row (oboard, -1001, 1001, MINNODE, depth, me);
	me->movelist = nmove->movelist;
	nmove->movelist = NULL;
	del_move (nmove);
    }

    /* now check the movelist for the move we have to make */
    if (((movedata = (nextmove *) llitembynr (me->movelist, 1)) == NULL) || 
	(movedata->remtype != REM_ROW))
    {
	/* very wrong */
#ifdef PRINTMOVES
        printf ("action removerow, but nextmove contains something else\n\n");
#endif
	exit (0);
    }

    correct = 0;
    /* check if this is the correct row to be removed */
    if ((start[0] == movedata->rowdata.start[0]) && 
	(start[1] == movedata->rowdata.start[1]))
    {
	if ((end[0] == movedata->rowdata.end[0]) && 
	    (end[1] == movedata->rowdata.end[1]))
	{
	    correct = 1;
	}
    }
    else if ((start[0] == movedata->rowdata.end[0]) && 
	     (start[1] == movedata->rowdata.end[1]))
    {
	if ((end[0] == movedata->rowdata.start[0]) && 
	    (end[1] == movedata->rowdata.start[1]))
	{
	    correct = 1;
	}
    }

    if (correct)
    {
	/* remove nextmove from movelist */
	movedata = (nextmove *) llrembynr (me->movelist, 1);
	free (movedata->rowdata.start);
	free (movedata->rowdata.end);
	free (movedata);

	/* remove movelist if empty */
	if (llitembynr (me->movelist, 1) == NULL)
	{
	    free (me->movelist);
	    me->movelist = NULL;
	}

#ifdef PRINTMOVES
	printf ("ai_minimax %c: remove row %2s-%2s\n", me->colour, start, end);
#endif
	return ('y');
    }

    /* don't remove the row */
#ifdef PRINTMOVES
    printf ("ai_minimax %c: don't remove row %2s-%2s\n", 
	    me->colour, start, end);
#endif

    return ('n');
}


void minimax_end (void * self)
{
    minimax_info * me = self;

    /* do I have to clean up the movelist first */

    free (me);
    return;
}


movestruct * alfabeta_move (board * oboard, int alfa, int beta, int minmax,
			    int depth, minimax_info * me)
{
    movestruct * nmove,
	* bestmove = NULL;
    fromto * allmoves;
    int maxmoves,
	chance=1,
	value,
	i;
    saveboard lastmove;

    /* maximum search depth or end of game */
    if ((b_game_finished (oboard)) ||
	(depth > me->config.searchdepth))
    {
	nmove = (movestruct *) malloc (sizeof (movestruct));
	nmove->from[0] = '\0';
	nmove->to[0] = '\0';
	nmove->type = 0;
	nmove->movelist = NULL;
	nmove->value = evaluate (oboard, me);
	return (nmove);
    }

#ifdef WINGIPF
    if ((depth == 1) || (depth < me->config.searchdepth))
    {
	checkwindowevents ();
    }
#endif

    /* list of moves to try depends on the question if we can 
    ** place a gipf or not.
    ** cutoff after a predetermined number of gipf-pieces */
    if ((b_colour_type (oboard, b_next_piece (oboard)) == 'g') && 
	(b_colour_gipf (oboard, b_next_piece (oboard)) < me->config.maxgipf))
    {
	allmoves = allmovesg;
	maxmoves = 84;
    }
    else
    {
	allmoves = allmovesn;
	maxmoves = 42;
    }

    lastmove.sboard = NULL;

    if (minmax == MAXNODE)
    {
	value = -1001;

	for (i = 0; i < maxmoves; i++)
	{
	    nmove = move (oboard, alfa, beta, minmax, depth, me, 
			  &(allmoves[i]), &lastmove);
	    if (nmove == NULL)
	    {
		continue;
	    }
#ifdef RANDOMCHOOSE
	    if (equ_h (nmove->value, value, &chance, me, depth))
#else
	    if (nmove->value > value)
#endif
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		del_move (nmove);
	    }
	    alfa = max (alfa, value);

#ifdef RANDOMCHOOSE
	    if (value > beta)
#else
	    if (value >= beta)
#endif
	    {
		/*
		** problems if value is the same as beta, this means that
		** when randomly selecting a move from the moves with the
		** same value, this one could be chosen.
		** solution: change value to (-1001 or 1001)
		**           or the current value (+1 or -1)
		**
		** is this correct? or does it depend on the fact if
		** this was called from a min- or a max-node?
		*/
		break;
	    }
	}
    }
    else
    {
	value = 1001;

	for (i = 0; i < maxmoves; i++)
	{
	    nmove = move (oboard, alfa, beta, minmax, depth, me, 
			  &(allmoves[i]), &lastmove);
	    if (nmove == NULL)
	    {
		continue;
	    }
#ifdef RANDOMCHOOSE
	    if (equ_l (nmove->value, value, &chance, me, depth))
#else
	    if (nmove->value < value)
#endif
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		del_move (nmove);
	    }
	    /* how and when did I throw this out ?????? */
	    beta = min (beta, value);

#ifdef RANDOMCHOOSE
	    if (value < alfa)
#else
	    if (value <= alfa)
#endif	  
	    {
		break;
	    }
	}
    }

    if (lastmove.sboard != NULL)
    {
	b_del (lastmove.sboard);
    }

    return (bestmove);
}


movestruct * move (board * oboard, int alfa, int beta, int minmax, int depth,
		   minimax_info * me, fromto * todo, saveboard * lmove)
{
    char piece;
    int newminmax;
    board * newboard;
    movestruct * mymove,
	* nmove;

    if (todo->type == 'g')
    {
	piece = b_otherpiece (b_next_piece (oboard));
    }
    else
    {
	piece = b_next_piece (oboard);
    }

    newboard = b_move (oboard, todo->from, todo->to, piece);

    if (newboard == NULL)
    {
	return (NULL);
    }

    /* check if the result is the same as the last move */
    if ((lmove->sboard != NULL) &&
	((lmove->to[0] == todo->to[0]) && (lmove->to[1] == todo->to[1])) &&
	(b_compare (newboard, lmove->sboard) == 0))
    {
        b_del (newboard);
        return (NULL);
    }
    if (lmove->sboard != NULL)
    {
	b_del (lmove->sboard);
    }
    lmove->sboard = newboard; /* don't delete the board at the end */
    lmove->to[0] = todo->to[0];
    lmove->to[1] = todo->to[1];

    /* create new movestruct */
    mymove = (movestruct *) malloc (sizeof (movestruct));
    mymove->from[0] = todo->from[0];
    mymove->from[1] = todo->from[1];
    mymove->to[0] = todo->to[0];
    mymove->to[1] = todo->to[1];
    mymove->type = todo->type;
    mymove->movelist = NULL;

    newminmax = (minmax == MINNODE ? MAXNODE : MINNODE);
    if ((b_status (newboard) == S_NORMAL) || 
	(b_status (newboard) == S_FINISHED))
    {
	nmove = alfabeta_move (newboard, alfa, beta, newminmax, depth+1, me);
	mymove->value = nmove->value;
	del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEROW)
    {
	nmove = alfabeta_row (newboard, alfa, beta, minmax, depth, me);
	mymove->movelist = nmove->movelist;
	mymove->value = nmove->value;
	nmove->movelist = NULL;
	del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEGIPF)
    {
	nmove = alfabeta_gipf (newboard, alfa, beta, minmax, depth, me);
	mymove->movelist = nmove->movelist;
	mymove->value = nmove->value;
	nmove->movelist = NULL;
	del_move (nmove);
    }
    else
    {   /* impossible */
#ifdef PRINTMOVES
	printf ("ERROR: impossible in ai_minimax::move\n\n");
#endif
	exit (0);
    }

    /* don't cleanup newboard here */
    return (mymove);
}


movestruct * alfabeta_gipf (board * oboard, int alfa, int beta, int minmax,
			    int depth, minimax_info * me)
{
    movestruct * nmove,
	* bestmove = NULL;
    int value,
	realminmax,
	counter,
	mynr,
	i,
	maxnr;
    unsigned char bits1,
	bits2;
    rem_gipf * gipfi;
    listheader * myg,
	* otherg;
    char * gipfpos;
    gipfitem * gipfflag;

    gipfi = (rem_gipf *) llitembynr (b_gipf_extra (oboard), 1);

    /* 
    ** I may have to change if I am a min or a max-node here
    ** it's possible that my opponent won a row, so he has to
    ** decide if gipf-pieces must be removed
    */
    if (b_gipf_owner(gipfi) == b_next_piece (oboard))
    {
	realminmax = (minmax == MINNODE ? MAXNODE : MINNODE);
    }
    else
    {
	realminmax = minmax;
    }

    /* always remove gipf-pieces from my opponent if the flag is set */
    myg = (listheader *) malloc (sizeof (listheader));
    newlist (myg);
    otherg = (listheader *) malloc (sizeof (listheader));
    newlist (otherg);
    counter = 1;
    mynr = 0;
    while ((gipfi = (rem_gipf *) llitembynr (b_gipf_extra (oboard), counter)) 
	   != NULL)
    {
	counter++;
	gipfflag = (gipfitem *) malloc (sizeof (gipfitem));
	gipfflag->gipf = gipfi;
	gipfflag->flag = 1;

	gipfpos = b_gipf_position (gipfi);
	if ((me->config.remoppgipf == 1) &&
	    (b_otherpiece (b_gipf_owner (gipfi)) != 
	     b_piece (oboard, gipfpos)))
	{
	    pushll (otherg, (void *) gipfflag);
	}
	else
	{
	    mynr++;
	    pushll (myg, (void *) gipfflag);
	}
	free (gipfpos);
    }

    maxnr = 1 << mynr; /* 2 ^ mynr */

    if (realminmax == MAXNODE)
    {
	value = -1001;

	for (bits1 = 0; bits1 < maxnr ; bits1++)
	{
	    bits2 = bits1;
	    for (i = 0; i < mynr; i++)
	    {
		gipfflag = (gipfitem *) llitembynr (myg, i+1);
		gipfflag->flag = bits2 & 1;
		bits2 = bits2 >> 1;
	    }
	    nmove = remove_gipflist (oboard, alfa, beta, minmax, depth, me, 
				     myg, otherg);

	    if (nmove->value > value)
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		del_move (nmove);
	    }
	    alfa = max (alfa, value);

#ifdef RANDOMCHOOSE
	    if (value > beta)
#else
	    if (value >= beta)
#endif
	    {
		break;
	    }
	}
    }
    else
    {
	value = 1001;

	for (bits1 = 0; bits1 < maxnr ; bits1++)
	{
	    bits2 = bits1;
	    for (i = 0; i < mynr; i++)
	    {
		gipfflag = (gipfitem *) llitembynr (myg, i+1);
		gipfflag->flag = bits2 & 1;
		bits2 = bits2 >> 1;
	    }
	    nmove = remove_gipflist (oboard, alfa, beta, minmax, depth, me, 
				     myg, otherg);

	    if (nmove->value < value)
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		del_move (nmove);
	    }
	    beta = min (beta, value);

#ifdef RANDOMCHOOSE
	    if (value < alfa)
#else
	    if (value <= alfa)
#endif	  
	    {
		break;
	    }
	}
    }

    /* cleanup temporary lists */
    while ((gipfflag = (gipfitem *) llrembynr (myg, 1)) != NULL)
    {
	free (gipfflag);
    }
    free (myg);
    while ((gipfflag = (gipfitem *) llrembynr (otherg, 1)) != NULL)
    {
	free (gipfflag);
    }
    free (otherg);

    return (bestmove);
}


movestruct * remove_gipflist (board * oboard, int alfa, int beta, int minmax,
			      int depth, minimax_info * me, listheader * myg,
			      listheader * otherg)
{
    board * nboard,
	* newboard;
    movestruct * mymove,
	* nmove;
    int counter;
    gipfitem * gipfflag;
    listheader * remlist;
    gipfdata * gipfd;
    char * strpos,
	owner;
    nextmove * movedata;
    int newminmax;

    newboard = oboard;
    remlist = (listheader *) malloc (sizeof (listheader));;
    newlist (remlist);
    /* list of gipf-pieces that may or may not have to be removed */
    counter = 1;
    while ((gipfflag = (gipfitem *) llitembynr (myg, counter)) != NULL)
    {
	counter++;
	gipfd = (gipfdata *) malloc (sizeof (gipfdata));
	strpos = b_gipf_position (gipfflag->gipf);
	owner = b_gipf_owner (gipfflag->gipf);
	gipfd->pos[0] = strpos[0];
	gipfd->pos[1] = strpos[1];
	gipfd->pos[2] = '\0';
	free (strpos);

	if (gipfflag->flag == 1)
	{
	    gipfd->flag = 'y';
	    nboard = b_remove_gipf (newboard, gipfflag->gipf);
	    if (newboard != oboard)
	    {
		b_del (newboard);
	    }
	    newboard = nboard;
	}
	else
	{
	    gipfd->flag = 'n';
	}
	pushll (remlist, (void *) gipfd);
    }

    /* list of gipf-pieces that always have to be removed */
    counter = 1;
    while ((gipfflag = (gipfitem *) llitembynr (otherg, counter)) != NULL)
    {
	counter++;
	gipfd = (gipfdata *) malloc (sizeof (gipfdata));
	strpos = b_gipf_position (gipfflag->gipf);
	owner = b_gipf_owner (gipfflag->gipf);
	gipfd->pos[0] = strpos[0];
	gipfd->pos[1] = strpos[1];
	gipfd->pos[2] = '\0';
	free (strpos);
	
	gipfd->flag = 'y';
	nboard = b_remove_gipf (newboard, gipfflag->gipf);
	if (newboard != oboard)
	{
	    b_del (newboard);
	}
	newboard = nboard;
	pushll (remlist, (void *) gipfd);
    }

    /* check again for 4 in a row */
    nboard = b_checkfour (newboard);
    if (newboard != oboard)
    {
	b_del (newboard);
    }
    newboard = nboard;

    mymove = (movestruct *) malloc (sizeof (movestruct));
    mymove->from[0] = '\0';
    mymove->to[0] = '\0';
    mymove->type = 0;
    mymove->movelist = (listheader *) malloc (sizeof (listheader));
    newlist (mymove->movelist);

    /* 
    ** only add actions to the movelist if they have to be executed
    ** by the max-player
    ** this means moves for the min-player will not be executable,
    ** but that really isn't necessary
    */
    if (me->colour == owner)
    {
	movedata = (nextmove *) malloc (sizeof (nextmove));
	movedata->remtype = REM_GIPF;
	movedata->gipflist = remlist;
	pushll (mymove->movelist, (void *) movedata);
    }
    else
    {   /* remove remlist, we don't need it here */
	while ((gipfd = (gipfdata *) llrembynr (remlist, 1)) != NULL)
	{
	    free (gipfd);
	}
	free (remlist);
    }

    newminmax = (minmax == MINNODE ? MAXNODE : MINNODE);

    if ((b_status (newboard) == S_NORMAL) || 
	(b_status (newboard) == S_FINISHED))
    {
	nmove = alfabeta_move (newboard, alfa, beta, newminmax, depth+1, me);
	mymove->value = nmove->value;
	del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEROW)
    {
	nmove = alfabeta_row (newboard, alfa, beta, minmax, depth, me);

	while ((movedata = (nextmove *) llrembynr (nmove->movelist, 1)) 
	       != NULL)
	{
	    pushll (mymove->movelist, (void *) movedata);
	}

	mymove->value = nmove->value;
	del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEGIPF)
    {
	nmove = alfabeta_gipf (newboard, alfa, beta, minmax, depth, me);

	while ((movedata = (nextmove *) llrembynr (nmove->movelist, 1)) 
	       != NULL)
	{
	    pushll (mymove->movelist, (void *) movedata);
	}

	mymove->value = nmove->value;
	del_move (nmove);
    }
    else
    {   /* impossible */
#ifdef PRINTMOVES
	printf ("ERROR: impossible in ai_minimax::remove_gipf\n\n");
#endif
	exit (0);
    }

    b_del (newboard);

    /* check if the movelist of mymove contains something */
    if (llitembynr (mymove->movelist, 1) == NULL)
    {
	free (mymove->movelist);
	mymove->movelist = NULL;
    }

    return (mymove);
}


movestruct * alfabeta_row (board * oboard, int alfa, int beta, int minmax,
			   int depth, minimax_info * me)
{
    int counter,
	value,
	realminmax;
    movestruct * nmove,
	* bestmove = NULL;
    rem_row * rowi;
    listheader * rowlist;

    rowlist = b_row_extra (oboard);
    rowi = (rem_row *) llitembynr (rowlist, 1);
    
    /* 
    ** I may have to change if I am a min or a max-node here
    ** it's possible that my opponent won several rows, so he has to
    ** decide what row to remove
    */
    if (row_owner(rowi) == b_next_piece (oboard))
    {
	realminmax = (minmax == MINNODE ? MAXNODE : MINNODE);
    }
    else
    {
	realminmax = minmax;
    }

    if (realminmax == MAXNODE)
    {
	value = -1001;

	counter = 1;
	while ((rowi = (rem_row *) llitembynr (rowlist, counter)) 
	       != NULL)
	{
	    nmove = remove_row (oboard, alfa, beta, minmax, depth, me, 
				counter);
	    counter++;

	    if (nmove->value > value)
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		del_move (nmove);
	    }
	    alfa = max (alfa, value);

#ifdef RANDOMCHOOSE
	    if (value > beta)
#else
	    if (value >= beta)
#endif
	    {
		break;
	    }
	}
    }
    else
    {
	value = 1001;

	counter = 1;
	while ((rowi = (rem_row *) llitembynr (rowlist, counter)) 
	       != NULL)
	{
	    nmove = remove_row (oboard, alfa, beta, minmax, depth, me, 
				counter);
	    counter++;

	    if (nmove->value < value)
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		del_move (nmove);
	    }
	    beta = min (beta, value);

#ifdef RANDOMCHOOSE
	    if (value < alfa)
#else
	    if (value <= alfa)
#endif	  
	    {
		break;
	    }
	}
    }

    return (bestmove);
}


movestruct * remove_row (board * oboard, int alfa, int beta, int minmax,
			 int depth, minimax_info * me, int rem)
{
    board * newboard;
    movestruct * nmove,
	* mymove;
    rem_row * remi;
    nextmove * movedata;
    int newminmax;

    newboard = b_remove_row (oboard, rem);

    mymove = (movestruct *) malloc (sizeof (movestruct));
    mymove->from[0] = '\0';
    mymove->to[0] = '\0';
    mymove->type = 0;
    mymove->movelist = (listheader *) malloc (sizeof (listheader));
    newlist (mymove->movelist);

    /* 
    ** only add actions to the movelist if they have to be executed
    ** by the max-player
    ** this means moves for the min-player will not be executable,
    ** but that really isn't necessary
    */
    remi = (rem_row *) llitembynr (b_row_extra (oboard), rem);
    if (me->colour == row_owner (remi))
    {
	movedata = (nextmove *) malloc (sizeof (nextmove));
	movedata->remtype = REM_ROW;
	movedata->gipflist = NULL;
	movedata->rowdata.start = postostr (remi->startpos);
	movedata->rowdata.end = postostr (remi->endpos);
	pushll (mymove->movelist, (void *) movedata);
    }

    newminmax = (minmax == MINNODE ? MAXNODE : MINNODE);

    if ((b_status (newboard) == S_NORMAL) || 
	(b_status (newboard) == S_FINISHED))
    {
	nmove = alfabeta_move (newboard, alfa, beta, newminmax, depth+1, me);
	mymove->value = nmove->value;
	del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEROW)
    {
	nmove = alfabeta_row (newboard, alfa, beta, minmax, depth, me);

	while ((movedata = (nextmove *) llrembynr (nmove->movelist, 1)) 
	       != NULL)
	{
	    pushll (mymove->movelist, (void *) movedata);
	}

	mymove->value = nmove->value;
	del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEGIPF)
    {
	nmove = alfabeta_gipf (newboard, alfa, beta, minmax, depth, me);

	while ((movedata = (nextmove *) llrembynr (nmove->movelist, 1)) 
	       != NULL)
	{
	    pushll (mymove->movelist, (void *) movedata);
	}

	mymove->value = nmove->value;
	del_move (nmove);
    }
    else
    {   /* impossible */
#ifdef PRINTMOVES
	printf ("ERROR: impossible in ai_minimax::remove_row\n\n");
#endif
	exit (0);
    }

    b_del (newboard);

    /* check if the movelist of mymove contains something */
    if (llitembynr (mymove->movelist, 1) == NULL)
    {
	free (mymove->movelist);
	mymove->movelist = NULL;
    }

    return (mymove);
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

/* 
** calculate the value of a certain board position
**
** returns a value between 1000 and -1000 (inclusive)
**   1000 : I win
**  -1000 : my opponent wins
*/
int evaluate (board * oboard, minimax_info * me)
{
    int total,
	i,
	value;
    double wvalue,
	bvalue;
    char myc,
	otherc,
	piece;
    position * pos;

    myc = me->colour;
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


void del_move (movestruct * todel)
{
    nextmove * movedata;
    gipfdata * gipfd;

    if (todel->movelist != NULL)
    {
	while ((movedata = (nextmove *) llrembynr (todel->movelist, 1)) 
	       != NULL)
	{
	    if (movedata->remtype == REM_ROW)
	    {
		free (movedata->rowdata.start);
		free (movedata->rowdata.end);
	    }
	    else
	    {
		while ((gipfd = (gipfdata *) 
			llrembynr (movedata->gipflist, 1)) != NULL)
		{
		    free (gipfd);
		}
		free (movedata->gipflist);
	    }

	    free (movedata);
	}
	free (todel->movelist);
    }

    free (todel);
    return;
}

#ifdef RANDOMCHOOSE
/*
** just like a normal compare (<)
** returns:
**   1 : nr1 < nr2
**   0: nr1 > nr2
**   0/1: when nr1 and nr2 are equal, chooses one at random
**
** chance is the chance that nr1 will be chosen if nr1 == nr2
** this parameter will be incremented each time an equal occurs
** and will be set to 1 if nr1 < nr2
** (it should be initialised to 1 by the caller and then left alone)
*/
int equ_l (int nr1, int nr2, int * chance, minimax_info * me, int depth)
{
    int randnr;

    if ((me->config.randomchoose == 0) || (depth > 1))
	return (nr1 < nr2);

    if (nr1 < nr2)
    {
 	*chance = 1;
	return (1);
    }
    if (nr1 > nr2)
    {
	return (0);
    }

    randnr = (int) ( (*chance + 1.0) * rand() / (RAND_MAX+1.0) );
    (*chance)++;

    if (randnr < 1)
    {
	return (1);
    }

    return (0);
}


/*
** just like a normal compare (>)
** returns:
**   1 : nr1 > nr2
**   0: nr1 < nr2
**   0/1: when nr1 and nr2 are equal, chooses one at random
**
** chance is the chance that nr1 will be chosen if nr1 == nr2
** this parameter will be incremented each time an equal occurs
** and will be set to 1 if nr1 > nr2
** (it should be initialised to 1 by the caller and then left alone)
*/	
int equ_h (int nr1, int nr2, int * chance, minimax_info * me, int depth)
{
    int randnr;

    if ((me->config.randomchoose == 0) || (depth > 1))
	return (nr1 > nr2);

    if (nr1 > nr2)
    {
 	*chance = 1;
	return (1);
    }
    if (nr1 < nr2)
    {
	return (0);
    }

    randnr = (int) ( (*chance + 1.0) * rand() / (RAND_MAX+1.0) );
    (*chance)++;

    if (randnr < 1)
    {
	return (1);
    }

    return (0);
}
#endif

