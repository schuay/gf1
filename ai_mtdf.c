/*
** $Id: ai_mtdf.c,v 1.3 1998/11/15 17:50:30 kurt Exp $
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

/*
** keep a tree of all searched nodes
** this is not useful for the normal minimax with alfabeta-pruning
** but can be used for negascout or mtd(f), algoritms wich do re-searching
*/

#include "ai_mtdf.h"

#ifdef WINGIPF
extern void checkwindowevents (void);
extern int interrupt_computer;
#endif

/* structures for keeping my own info in */
typedef struct {
    char colour;
    int game;
    int lastvalue;
    struct {
	int searchdepth;
	int remoppgipf;
	int maxgipf;
	int memorydepth;
	int randomchoose;
    } config;
    listheader * movelist;  /* list of items of type nextmove */
} mtdf_info;

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

/* things for the node-tree */
typedef struct {
    board * father;
    int upper;
    int lower;
    listheader * children;  /* list of treeitems */
} treenode;


struct {
    position coor;
    int value;
} mem_pos_val[] = {
    {{4, 5}, 5}, 
    {{3, 4}, 2},{{3, 5}, 2},{{4, 6}, 2},{{5, 5}, 2},{{5, 4}, 2},{{4, 4}, 2}, 
    {{2, 3}, 1},{{2, 4}, 1},{{2, 5}, 1},{{3, 6}, 1},{{4, 7}, 1},{{5, 6}, 1}, 
    {{6, 5}, 1},{{6, 4}, 1},{{6, 3}, 1},{{5, 3}, 1},{{4, 3}, 1},{{3, 3}, 1}
};

#define MINNODE 1
#define MAXNODE 2

#define min(x, y)     (x < y ? x : y)
#define max(x, y)     (x > y ? x : y)

movestruct * mtdf_id (board * oboard, treenode * node, int f, mtdf_info * me,
		      int type);
movestruct * mtdf (board * oboard, treenode * node, int f, mtdf_info * me,
		   int type);

movestruct * mem_ab_move (board * oboard, treenode * node, int alfa, int beta, 
			  int minmax, int depth, mtdf_info * me);
movestruct * mem_move (board * oboard, treenode * node, int alfa, int beta,
		       int minmax, int depth, mtdf_info * me, fromto * todo,
		       saveboard * lmove);
movestruct * mem_ab_gipf (board * oboard, treenode * node, int alfa, int beta, 
			  int minmax, int depth, mtdf_info * me);
movestruct * mem_rem_gipflist (board * oboard, treenode * node, int alfa,
			       int beta, int minmax, int depth,
			       mtdf_info * me, listheader * myg,
			       listheader * otherg);
movestruct * mem_ab_row (board * oboard, treenode * node, int alfa, int beta, 
			 int minmax, int depth, mtdf_info * me);
movestruct * mem_rem_row (board * oboard, treenode * node, int alfa, int beta,
			  int minmax, int depth, mtdf_info * me, int rem);
int mem_evaluate (board * oboard, mtdf_info * me);
void mem_del_move (movestruct * todel);

void mem_deltree (treenode * node);
void mem_delnode (void * item);
treenode * mem_makeroot (board * father, mtdf_info * me);
treenode * mem_child (treenode * node, int nr, mtdf_info * me, int depth);
void mem_resetul (void * item);

int mem_equ_l (int nr1, int nr2, int * chance, mtdf_info * me);
int mem_equ_h (int nr1, int nr2, int * chance, mtdf_info * me);

void * mtdf_new (char colour, int game)
{
    mtdf_info * me;
    listheader * configlist;
    
    me = (mtdf_info *) malloc (sizeof (mtdf_info));
    me->colour = colour;
    me->game = game;
    me->lastvalue = 0;
    me->movelist = NULL;

    /* check for config-values from the config-file */
    configlist = readconfigfile ("gf1.cfg");
    
    me->config.searchdepth = findconfigvalue (configlist, "searchdepth",
					      colour, 3);
    me->config.remoppgipf = findconfigvalue (configlist, "remoppgipf",
					     colour, 1);
    me->config.maxgipf = findconfigvalue (configlist, "maxgipf",
					  colour, 3);
    me->config.memorydepth = findconfigvalue (configlist, "memorydepth",
					      colour, 3);
    me->config.randomchoose = findconfigvalue (configlist, "randomchoose",
					      colour, 1);

    clearconfiglist (configlist);

    return ((void *) me);
}


void mtdf_move (board * oboard, void * self, 
		char * type, char * from, char * to)
{
    mtdf_info * me = self;
    movestruct * move; 
    treenode * root;
    
    b_nolog (oboard);

    root = mem_makeroot (oboard, me);
/*
    move = mtdf (oboard, root, me->lastvalue, me, 1);
*/
    move = mtdf_id (oboard, root, me->lastvalue, me, 1);
    
    mem_deltree (root);

    from[0] = move->from[0];
    from[1] = move->from[1];
    to[0] = move->to[0];
    to[1] = move->to[1];
    *type = move->type;

    me->movelist = move->movelist;

    move->movelist = NULL;
    mem_del_move (move);

    return;
}


char mtdf_gipf (board * oboard, void * self, char * pos)
{
    mtdf_info * me = self;
    nextmove * movedata;
    gipfdata * gipfd;
    int counter;
    char val;
    treenode * root;

    b_nolog (oboard);

    /* check if movelist contains info about removing gipf-pieces */
    if (me->movelist == NULL)
    {
	/* 
	** no info in movelist, my opponent made 4 in a row
	** with my pieces (only possible explanation)
	*/
	movestruct * nmove;

	/* remark: I have to start this as a min-node, 
	** because this is really my opponents move */
	root = mem_makeroot (oboard, me);
/*
	nmove = mtdf (oboard, root, me->lastvalue, me, 2);
*/
        nmove = mtdf_id (oboard, root, me->lastvalue, me, 2);
	mem_deltree (root);
	
	me->movelist = nmove->movelist;
	nmove->movelist = NULL;
	mem_del_move (nmove);
    }

    /* now check the movelist for the move we have to make */
    if (((movedata = (nextmove *) llitembynr (me->movelist, 1)) == NULL) || 
	(movedata->remtype != REM_GIPF))
    {
	/* very wrong */
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

    return (val);
}


char mtdf_row (board * oboard, void * self, char * start, char * end)
{
    mtdf_info * me = self;
    nextmove * movedata;
    int correct;
    treenode * root;

    b_nolog (oboard);

    /* check if movelist contains info about removing rows */
    if (me->movelist == NULL)
    {
	/* 
	** no info in movelist, my opponent made several "4 in a row"s
	** with my pieces (only possible explanation)
	*/
	movestruct * nmove;

	/* remark: I have to start this as a min-node, 
	** because this is really my opponents move */
	root = mem_makeroot (oboard, me);
/*
	nmove = mtdf (oboard, root, me->lastvalue, me, 3);
*/
	nmove = mtdf_id (oboard, root, me->lastvalue, me, 3);
	mem_deltree (root);
	
	me->movelist = nmove->movelist;
	nmove->movelist = NULL;
	mem_del_move (nmove);
    }

    /* now check the movelist for the move we have to make */
    if (((movedata = (nextmove *) llitembynr (me->movelist, 1)) == NULL) || 
	(movedata->remtype != REM_ROW))
    {
	/* very wrong */
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

	return ('y');
    }

    /* don't remove the row */
    return ('n');
}


void mtdf_end (void * self)
{
    mtdf_info * me = self;

    /* do I have to clean up the movelist first */

    free (me);
    return;
}


/*
** iterative deepening around mtdf
**
** REMARK: doesn't work, don't know why yet
*/
movestruct * mtdf_id (board * oboard, treenode * node, int f, mtdf_info * me,
		      int type)
{
    int savemaxdepth,
        i;
    movestruct * bestmove = NULL;

    savemaxdepth = me->config.searchdepth;

    for (i = 1; i <= savemaxdepth; i++)
    {
	if (bestmove != NULL)
	{
	    mem_del_move (bestmove);

	    /* reset upper and lower limits in the memory-tree */
	    mem_resetul (node);
	}

	me->config.searchdepth = i;
	bestmove = mtdf (oboard, node, f, me, type);
    }

    me->config.searchdepth = savemaxdepth;

    return (bestmove);
}

movestruct * mtdf (board * oboard, treenode * node, int f, mtdf_info * me,
		   int type)
{
    int value,
	upper,
	lower,
	beta;
    movestruct * bestmove = NULL;
    
    value = f;
    upper = 1001;
    lower = -1001;

    while (lower < upper)
    {
	if (value == lower)
	{
	    beta = value + 1;
	}
	else
	{
	    beta = value;
	}

	if (bestmove != NULL)
	{
	    mem_del_move (bestmove);
	}

	switch (type)
	{
	case 1:
	    bestmove = mem_ab_move (oboard, node, beta-1, beta, 
				    MAXNODE, 1, me);
	    break;

	case 2:
	    bestmove = mem_ab_gipf (oboard, node, beta-1, beta, 
				    MINNODE, 1, me);
	    break;

	case 3:
	    bestmove = mem_ab_row (oboard, node, beta-1, beta, 
				    MINNODE, 1, me);
	    break;
	}
	value = bestmove->value;

	if (value < beta)
	{
	    upper = value;
	}
	else
	{
	    lower = value;
	}
    }

    me->lastvalue = value;
    return (bestmove);
}


movestruct * mem_ab_move (board * oboard, treenode * node, int alfa, int beta, 
			  int minmax, int depth, mtdf_info * me)
{
    movestruct * nmove,
	* bestmove = NULL;
    fromto * allmoves;
    int maxmoves,
	value,
	chance = 1,
	i;
    saveboard lastmove;
    treenode * childnode;

    /* maximum search depth or end of game */
    if ((b_game_finished (oboard)) ||
	(depth > me->config.searchdepth))
    {
	nmove = (movestruct *) malloc (sizeof (movestruct));
	nmove->from[0] = '\0';
	nmove->to[0] = '\0';
	nmove->type = 0;
	nmove->movelist = NULL;
	nmove->value = mem_evaluate (oboard, me);
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
	(b_colour_gipf (oboard, b_next_piece (oboard)) <
	 me->config.maxgipf))
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
	    childnode = mem_child (node, i+1, me, depth);
	    nmove = mem_move (oboard, childnode, alfa, beta, minmax,
			      depth, me, &(allmoves[i]), &lastmove);
	    if (nmove == NULL)
	    {
		continue;
	    }
	    if (mem_equ_h (nmove->value, value, &chance, me))
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    mem_del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		mem_del_move (nmove);
	    }
	    alfa = max (alfa, value);

	    if (value > beta)
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
	    childnode = mem_child (node, i+1, me, depth);
	    nmove = mem_move (oboard, childnode, alfa, beta, minmax,
			      depth, me, &(allmoves[i]), &lastmove);
	    if (nmove == NULL)
	    {
		continue;
	    }
	    if (mem_equ_l (nmove->value, value, &chance, me))
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    mem_del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		mem_del_move (nmove);
	    }
	    beta = min (beta, value);

	    if (value < alfa)
	    {
		break;
	    }
	}
    }

    if ((childnode == NULL) && (lastmove.sboard != NULL))
    {
	b_del (lastmove.sboard);
    }

    return (bestmove);
}


movestruct * mem_move (board * oboard, treenode * node, int alfa, int beta,
		       int minmax, int depth, mtdf_info * me, fromto * todo,
		       saveboard * lmove)
{
    board * newboard;
    char piece;
    int newminmax;
    movestruct * mymove,
	* nmove;

    if ((node != NULL) && (node->children != NULL))
    {   /* node already visited, reuse from memory */
	if (node->father == NULL)
	{
	    return (NULL);
	}
	newboard = node->father;

#if 0
/*
** removing this solves the problem of bad play
** but it makes the computerplayer twice as slow as ai_minimax
**
** it also solves the problem of iterative deepening not working
*/
	if (node->lower >= beta)
	{
	    nmove = (movestruct *) malloc (sizeof (movestruct));
	    nmove->from[0] = todo->from[0];
	    nmove->from[1] = todo->from[1];
	    nmove->to[0] = todo->to[0];
	    nmove->to[1] = todo->to[1];
	    nmove->type = 0;
	    nmove->movelist = NULL;
	    nmove->value = node->lower;
	    return (nmove);
	}
	if (node->upper <= alfa)
	{
	    nmove = (movestruct *) malloc (sizeof (movestruct));
	    nmove->from[0] = todo->from[0];
	    nmove->from[1] = todo->from[1];
	    nmove->to[0] = todo->to[0];
	    nmove->to[1] = todo->to[1];
	    nmove->type = todo->type;
	    nmove->movelist = NULL;
	    nmove->value = node->upper;
	    return (nmove);
	}
	alfa = max (alfa, node->lower);
	beta = min (beta, node->upper);
#endif
    }
    else
    {
	if (node != NULL)
	{
	    /* create children list */
	    node->children = (listheader *) malloc (sizeof (listheader));
	    newlist (node->children);
	}

	if (todo->type == 'g')
	{
	    piece = b_otherpiece (b_next_piece (oboard));
	}
	else
	{
	    piece = b_next_piece (oboard);
	}

	newboard = b_move (oboard, todo->from, todo->to, piece);
	if (node != NULL)
	{
	    node->father = newboard;
	}

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
	    if (node != NULL)
	    {
		node->father = NULL;
	    }
	    return (NULL);
	}
	if ((node == NULL) && (lmove->sboard != NULL))
	{
	    b_del (lmove->sboard);
	}
	lmove->sboard = newboard; /* don't delete the board at the end */
	lmove->to[0] = todo->to[0];
	lmove->to[1] = todo->to[1];
    }
    
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
	nmove = mem_ab_move (newboard, node, alfa, beta, newminmax, 
			     depth+1, me);
	mymove->value = nmove->value;
	mem_del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEROW)
    {
	nmove = mem_ab_row (newboard, node, alfa, beta, minmax, depth, me);
	mymove->movelist = nmove->movelist;
	mymove->value = nmove->value;
	nmove->movelist = NULL;
	mem_del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEGIPF)
    {
	nmove = mem_ab_gipf (newboard, node, alfa, beta, minmax, depth, me);
	mymove->movelist = nmove->movelist;
	mymove->value = nmove->value;
	nmove->movelist = NULL;
	mem_del_move (nmove);
    }
    else
    {   /* impossible */
	exit (0);
    }

    if (node != NULL)
    {
	if (mymove->value <= alfa)
	{
	    node->upper = mymove->value;
	}
	else if ((mymove->value > alfa) && (mymove->value < beta))
	{   /* this shouldn't happen with zero window searches */
	    node->upper = node->lower = mymove->value;
	}
	else if (mymove->value >= beta)
	{
	    node->lower = mymove->value;
	}
    }
	
    return (mymove);
}


movestruct * mem_ab_gipf (board * oboard, treenode * node, int alfa, int beta, 
			  int minmax, int depth, mtdf_info * me)
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
    treenode * childnode;
    
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
	    childnode = mem_child (node, bits1 + 1, me, depth);
	    nmove = mem_rem_gipflist (oboard, childnode, alfa, beta,
				      minmax, depth, me, myg, otherg);

	    if (nmove->value > value)
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    mem_del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		mem_del_move (nmove);
	    }
	    alfa = max (alfa, value);

	    if (value > beta)
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
	    childnode = mem_child (node, bits1 + 1, me, depth);
	    nmove = mem_rem_gipflist (oboard, childnode, alfa, beta,
				      minmax, depth, me, myg, otherg);

	    if (nmove->value < value)
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    mem_del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		mem_del_move (nmove);
	    }
	    beta = min (beta, value);

	    if (value < alfa)
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


movestruct * mem_rem_gipflist (board * oboard, treenode * node, int alfa,
			       int beta, int minmax, int depth,
			       mtdf_info * me, listheader * myg,
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

    if ((node != NULL) && (node->children != NULL))
    {   /* node already visited, reuse from memory */
	if (node->father == NULL)
	{
	    return (NULL);
	}
	newboard = node->father;

	/* I have to produce remlist here again */
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
	    pushll (remlist, (void *) gipfd);
	}

    }
    else
    {
	if (node != NULL)
	{
	    /* create children list */
	    node->children = (listheader *) malloc (sizeof (listheader));
	    newlist (node->children);
	}

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
	if (node != NULL)
	{
	    node->father = newboard;
	}
    }
    
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
	nmove = mem_ab_move (newboard, node, alfa, beta, newminmax, 
			     depth+1, me);
	mymove->value = nmove->value;
	mem_del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEROW)
    {
	nmove = mem_ab_row (newboard, node, alfa, beta, minmax, depth, me);

	while ((movedata = (nextmove *) llrembynr (nmove->movelist, 1)) 
	       != NULL)
	{
	    pushll (mymove->movelist, (void *) movedata);
	}

	mymove->value = nmove->value;
	mem_del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEGIPF)
    {
	nmove = mem_ab_gipf (newboard, node, alfa, beta, minmax, depth, me);

	while ((movedata = (nextmove *) llrembynr (nmove->movelist, 1)) 
	       != NULL)
	{
	    pushll (mymove->movelist, (void *) movedata);
	}

	mymove->value = nmove->value;
	mem_del_move (nmove);
    }
    else
    {   /* impossible */
	exit (0);
    }

    /* check if the movelist of mymove contains something */
    if (llitembynr (mymove->movelist, 1) == NULL)
    {
	free (mymove->movelist);
	mymove->movelist = NULL;
    }

    if (node == NULL)
    {
	b_del (newboard);
    }
    else
    {
	if (mymove->value <= alfa)
	{
	    node->upper = mymove->value;
	}
	else if ((mymove->value > alfa) && (mymove->value < beta))
	{   /* this shouldn't happen with zero window searches */
	    node->upper = node->lower = mymove->value;
	}
	else if (mymove->value >= beta)
	{
	    node->lower = mymove->value;
	}
    }
	

    return (mymove);
}


movestruct * mem_ab_row (board * oboard, treenode * node, int alfa, int beta, 
			 int minmax, int depth, mtdf_info * me)
{
    int counter,
	value,
	realminmax;
    movestruct * nmove,
	* bestmove = NULL;
    rem_row * rowi;
    listheader * rowlist;
    treenode * childnode;

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
	    childnode = mem_child (node, counter, me, depth);
	    nmove = mem_rem_row (oboard, childnode, alfa, beta, minmax,
				 depth, me, counter);
	    counter++;

	    if (nmove->value > value)
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    mem_del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		mem_del_move (nmove);
	    }
	    alfa = max (alfa, value);

	    if (value > beta)
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
	    childnode = mem_child (node, counter, me, depth);
	    nmove = mem_rem_row (oboard, childnode, alfa, beta, minmax,
				depth, me, counter);
	    counter++;

	    if (nmove->value < value)
	    {
		value = nmove->value;
		if (bestmove != NULL)
		{
		    mem_del_move (bestmove);
		}
		bestmove = nmove;
	    }
	    else
	    {
		mem_del_move (nmove);
	    }
	    beta = min (beta, value);

	    if (value < alfa)
	    {
		break;
	    }
	}
    }

    return (bestmove);
}


movestruct * mem_rem_row (board * oboard, treenode * node, int alfa, int beta,
			  int minmax, int depth, mtdf_info * me, int rem)
{
    board * newboard;
    movestruct * nmove,
	* mymove;
    rem_row * remi;
    nextmove * movedata;
    int newminmax;

    if ((node != NULL) && (node->children != NULL))
    {   /* node already visited, reuse from memory */
	if (node->father == NULL)
	{
	    return (NULL);
	}
	newboard = node->father;
    }
    else
    {
	if (node != NULL)
	{
	    /* create children list */
	    node->children = (listheader *) malloc (sizeof (listheader));
	    newlist (node->children);
	}

	newboard = b_remove_row (oboard, rem);
	if (node != NULL)
	{
	    node->father = newboard;
	}
    }

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
	nmove = mem_ab_move (newboard, node, alfa, beta, newminmax, 
			     depth+1, me);
	mymove->value = nmove->value;
	mem_del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEROW)
    {
	nmove = mem_ab_row (newboard, node, alfa, beta, minmax, depth, me);

	while ((movedata = (nextmove *) llrembynr (nmove->movelist, 1)) 
	       != NULL)
	{
	    pushll (mymove->movelist, (void *) movedata);
	}

	mymove->value = nmove->value;
	mem_del_move (nmove);
    }
    else if (b_status (newboard) == S_REMOVEGIPF)
    {
	nmove = mem_ab_gipf (newboard, node, alfa, beta, minmax, depth, me);

	while ((movedata = (nextmove *) llrembynr (nmove->movelist, 1)) 
	       != NULL)
	{
	    pushll (mymove->movelist, (void *) movedata);
	}

	mymove->value = nmove->value;
	mem_del_move (nmove);
    }
    else
    {   /* impossible */
	exit (0);
    }

    /* check if the movelist of mymove contains something */
    if (llitembynr (mymove->movelist, 1) == NULL)
    {
	free (mymove->movelist);
	mymove->movelist = NULL;
    }

    if (node == NULL)
    {
	b_del (newboard);
    }
    else
    {
	if (mymove->value <= alfa)
	{
	    node->upper = mymove->value;
	}
	else if ((mymove->value > alfa) && (mymove->value < beta))
	{   /* this shouldn't happen with zero window searches */
	    node->upper = node->lower = mymove->value;
	}
	else if (mymove->value >= beta)
	{
	    node->lower = mymove->value;
	}
    }
	

    return (mymove);
}

/* 
** calculate the value of a certain board position
**
** returns a value between 1000 and -1000 (inclusive)
**   1000 : I win
**  -1000 : my opponent wins
*/
int mem_evaluate (board * oboard, mtdf_info * me)
{
    int total,
	i,
	value;
    double wvalue,
	bvalue;
    char myc,
	otherc;
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
	pos = &(mem_pos_val[i].coor);
	switch (b_ppiece (oboard, pos))
	{
	case '.':
	    break;

	case 'o':
	    wvalue += mem_pos_val[i].value;
	    break;

	case 'O':
	    wvalue += mem_pos_val[i].value * 1.5;
	    break;

	case 'x':
	    bvalue += mem_pos_val[i].value;
	    break;

	default:
	    bvalue += mem_pos_val[i].value * 1.5;
	}
/*
	piece = b_ppiece (oboard, pos);
	if (piece == '.')
	{
	    continue;
	}
	if (piece == 'o')
	{
	    wvalue += mem_pos_val[i].value;
	}
	else if (piece == 'O')
	{
	    wvalue += mem_pos_val[i].value * 1.5;
	}
	else if (piece == 'x')
	{
	    bvalue += mem_pos_val[i].value;
	}
	else
	{
	    bvalue += mem_pos_val[i].value * 1.5;
	}
*/
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

void mem_del_move (movestruct * todel)
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


/*
** create the root of the tree
**
** parameter:
**     father: the board at the root of the tree
*/
treenode * mem_makeroot (board * father, mtdf_info * me)
{
    treenode * newnode;

    if (me->config.memorydepth == 0)
    {
	return (NULL);
    }

    newnode = (treenode *) malloc (sizeof (treenode));
    newnode->father = father;
    newnode->upper = 1001;
    newnode->lower = -1001;

    newnode->children = (listheader *) malloc (sizeof (listheader));
    newlist (newnode->children);

    return (newnode);
}


/*
** delete a tree
** don't delete the father from the root-node
*/
void mem_deltree (treenode * node)
{
    if (node == NULL)
    {
	return;
    }

    emptyll (node->children, mem_delnode);
    free (node->children);
    free (node);

    return;
}

void mem_delnode (void * item)
{
    treenode * node = item;

    b_del (node->father);
    emptyll (node->children, mem_delnode);
    free (node->children);
    free (node);

    return;
}


/*
** reset upper and lower limits in the memory-tree 
*/
void mem_resetul (void * item)
{
    treenode * node = item;

    node->upper = 1001;
    node->lower = -1001;
    doll (node->children, mem_resetul);

    return;
}


/*
** check if a child-node already exists,
** if not, create a new one
*/
treenode * mem_child (treenode * node, int nr, mtdf_info * me, int depth)
{
    treenode * newnode;

    if (me->config.memorydepth < depth)
    {
	return (NULL);
    }

    newnode = (treenode *) llitembynr (node->children, nr);
    if (newnode != NULL)
    {   /* this node has already been handled, return it */
	return (newnode);
    }

#if 0
/* just for testing */
/* check if the previous nr exists, it should */
if ((nr > 1) && (llitembynr (node->children, nr-1) == NULL))
{
    printf ("\nERROR: mem_child\n\n");
    exit (0);
}
#endif 

    /* create a new node */
    newnode = (treenode *) malloc (sizeof (treenode));
    newnode->father = NULL;
    newnode->children = NULL;
    newnode->upper = 1001;
    newnode->lower = -1001;
    pushll (node->children, newnode);
    
    return (newnode);
}

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
int mem_equ_l (int nr1, int nr2, int * chance, mtdf_info * me)
{
    int randnr;

    if (me->config.randomchoose == 0)
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
int mem_equ_h (int nr1, int nr2, int * chance, mtdf_info * me)
{
    int randnr;

    if (me->config.randomchoose == 0)
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
