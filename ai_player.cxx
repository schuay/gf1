/*
** $Id: ai_player.cxx,v 1.6 1999/07/21 13:06:07 kurt Exp $
*/
/*
**    Copyright (C) 1999 Kurt Van den Branden
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

#include <iostream.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include "ai_player.h"


// constructor
ai_player::ai_player  ()
{
    maxdepth = 2;
    maxtime = 0;
    memorydepth = 0;
    randomchoose = 1;
    player = PLAYER1;
    lastvalue = 0;
    state = AI_OK;
    stopnow = 0;

    return;
}


// destructor
ai_player::~ai_player ()
{
    return;
}


vector<basemove *> * ai_player::minimax_ab (void * startgame, int starttype)
{
    searchnode * firstnode;
    vector<basemove *> * move;

    state = AI_OK;
    stopnow = 0;

    if (randomchoose)
	srand (time (NULL));

    firstnode = new searchnode;

    firstnode->gameposition = startgame;
    firstnode->finished = 0;
    firstnode->depth = 1; 
    firstnode->value = 0;
    firstnode->upper = 1001;
    firstnode->lower = -1001;
    firstnode->type = starttype;
//    firstnode->movelist = NULL;
//    firstnode->childlist = NULL;

//    move = minimax_ab (firstnode, -1001, 1001);
    move = ab_mem (firstnode, -1001, 1001);
    lastvalue = firstnode->value;

    delmemtree (firstnode, 0);
    
    if (move != NULL)
	reverse (move->begin(), move->end());
    return (move);
}


#if 0
/* 
** correct, but replaced by ab_mem
*/
listheader * ai_player::minimax_ab (searchnode * node, int alfa, int beta)
{
    int value,
	counter,
	ddelta,
	chance = 1;
    listheader * movel = NULL,
	* newmovel;
    void * moveitem;
    searchnode * newnode;

    // check for leafnode
    // (end-of-game or maxdepth exceeded)
    if ((node->finished) ||
	(node->depth > maxdepth))
    {
	value = evalfunc (node->gameposition, player);

	// create an empty move
	movel = (listheader *) malloc (sizeof (listheader));
	newlist (movel);
    }
    // search the tree
    else
    {
	if (node->movelist == NULL)
	{
	    node->movelist = listfunc (node->gameposition);
	    node->childlist = (listheader *) malloc (sizeof (listheader));
	    newlist (node->childlist);
	}

	if (node->type == MAX)
	{
	    value = -1001;

	    counter = 1;
	    while ((moveitem = llitembynr (node->movelist, counter)) != NULL)
	    {
		// generate child (if necessary)
		newnode = nodechild (node, counter);

		counter++;

		if (newnode->finished == -1)
		{   // calculate new position
		    newnode->finished = 0;
		    newnode->gameposition = 
			newfunc (node->gameposition, moveitem, 
				 &(newnode->finished), &ddelta, 
				 &(newnode->type));
		    newnode->depth += ddelta;
		}

		if (newnode->gameposition == NULL)
		    continue;

		newmovel = minimax_ab (newnode, alfa, beta);

		// is the new move better then the previous best
//		if (equ_h (newnode->value, value, &chance))
		if (((node->depth == 1) && 
		     (equ_h (newnode->value, value, &chance))) ||
		    ((node->depth > 1) && (newnode->value > value)))
		{
		    value = newnode->value;
		    delmovelist (movel);
		    movel = newmovel;
		    unshiftll (movel, copymovefunc (moveitem));
		}
		else
		{
		    delmovelist (newmovel);
		}

		alfa = max (alfa, value);

		if (value > beta)
//		if (value >= beta)
		{
		    break;
		}
	    }
	}
	else // MIN
	{
	    value = 1001;

	    counter = 1;
	    while ((moveitem = llitembynr (node->movelist, counter)) != NULL)
	    {
		// generate child (if necessary)
		newnode = nodechild (node, counter);

		counter++;

		if (newnode->finished == -1)
		{   // calculate new position
		    newnode->finished = 0;
		    newnode->gameposition = 
			newfunc (node->gameposition, moveitem, 
				 &(newnode->finished), &ddelta, 
				 &(newnode->type));
		    newnode->depth += ddelta;
		}

		if (newnode->gameposition == NULL)
		    continue;

		newmovel = minimax_ab (newnode, alfa, beta);

		// is the new move better then the previous best
//		if (equ_l (newnode->value, value, &chance))
		if (((node->depth == 1) && 
		     (equ_l (newnode->value, value, &chance))) ||
		    ((node->depth > 1) && (newnode->value < value)))
		{
		    value = newnode->value;
		    delmovelist (movel);
		    movel = newmovel;
		    unshiftll (movel, copymovefunc (moveitem));
		}
		else
		{
		    delmovelist (newmovel);
		}

		beta = min (beta, value);

		if (value < alfa)
//		if (value <= alfa)
		{
		    break;
		}
	    }
	}
    }

    node->value = value;

    // cleanup children-nodes
    while ((newnode = (searchnode *) llrembynr (node->childlist, 1))
	   != NULL)
    {
	delmemtree (newnode);
    }

    // what if no valid move ?
    // is it enough that movel contains NULL in that case ?

    return (movel);
}
#endif


vector<basemove *> * ai_player::ab_mem (searchnode * node, int alfa, int beta)
{
    int value,
	ddelta,
	chance = 1,
	len, i,
	a, b;
    vector<basemove *> * movel = NULL,
	* newmovel;
    searchnode * newnode;

    if (node->lower > beta)  // >=
    {
	node->value = node->lower;
	// create an empty move
	movel = new vector<basemove *>;
	return (movel);
    }
    if (node->upper < alfa)  // <=
    {
	node->value = node->upper;
	// create an empty move
	movel = new vector<basemove *>;
	return (movel);
    }
    alfa = max (alfa, node->lower);
    beta = min (beta, node->upper);

    // check for leafnode
    // (end-of-game or maxdepth exceeded)
    if ((node->finished == 1) ||
	((node->depth > maxdepth) && (node->finished != 2)))
    {
	value = evalfunc (node->gameposition, player);

	// create an empty move
	movel = new vector<basemove *>;
    }
    // search the tree
    else
    {
	if (node->movelist.empty())
	{
	    listfunc (node->gameposition, node->movelist);
	}

	if (node->type == MAX)
	{
	    value = -1001;
	    a = alfa;

	    len = node->movelist.size();
	    for (i = 0; i < len; i++)
//	    while ((moveitem = llitembynr (node->movelist, counter)) != NULL)
	    {
		// generate child (if necessary)
		newnode = nodechild (node, i);

		if (newnode->finished == -1)
		{   // calculate new position
		    newnode->finished = 0;
		    newnode->gameposition = 
			newfunc (node->gameposition, node->movelist[i], 
				 &(newnode->finished), &ddelta, 
				 &(newnode->type));
		    newnode->depth += ddelta;
		}

		if (newnode->gameposition == NULL)
		    continue;

		newmovel = ab_mem (newnode, a, beta);

		if (stopnow)
		{
		    delmovelist (movel);
		    delmovelist (newmovel);
		    return (NULL);
		}
		if (outoftime ())
		{
		    stopnow = 1;
		    state = AI_TIMEUP;
		    delmovelist (movel);
		    delmovelist (newmovel);
		    return (NULL);
		}
		if (stopfunc () == 1)
		{
		    state = AI_STOPPED;
		    delmovelist (movel);
		    delmovelist (newmovel);
		    return (NULL);
		}

		// is the new move better then the previous best
		if (((node->depth == 1) && 
		     (equ_h (newnode->value, value, &chance))) ||
		    ((node->depth > 1) && (newnode->value > value)))
		{
		    value = newnode->value;
		    delmovelist (movel);
		    movel = newmovel;
//		    unshiftll (movel, node->movelist[i]->copy());
		    movel->push_back (node->movelist[i]->copy());

		    a = max (a, value);
		    
		    if (value > beta)
		    {
			break;
		    }
		}
		else
		{
		    delmovelist (newmovel);
		}
	    }
	}
	else // MIN
	{
	    value = 1001;
	    b = beta;

	    len = node->movelist.size();
	    for (i = 0; i < len; i++)
//	    while ((moveitem = llitembynr (node->movelist, counter)) != NULL)
	    {
		// generate child (if necessary)
		newnode = nodechild (node, i);

		if (newnode->finished == -1)
		{   // calculate new position
		    newnode->finished = 0;
		    newnode->gameposition = 
			newfunc (node->gameposition, node->movelist[i], 
				 &(newnode->finished), &ddelta, 
				 &(newnode->type));
		    newnode->depth += ddelta;
		}

		if (newnode->gameposition == NULL)
		    continue;

		newmovel = ab_mem (newnode, alfa, b);

		if (stopnow)
		{
		    delmovelist (movel);
		    delmovelist (newmovel);
		    return (NULL);
		}
		if (outoftime ())
		{
		    stopnow = 1;
		    state = AI_TIMEUP;
		    delmovelist (movel);
		    delmovelist (newmovel);
		    return (NULL);
		}
		if (stopfunc () == 1)
		{
		    state = AI_STOPPED;
		    delmovelist (movel);
		    delmovelist (newmovel);
		    return (NULL);
		}

		// is the new move better then the previous best
//		if (equ_l (newnode->value, value, &chance))
		if (((node->depth == 1) && 
		     (equ_l (newnode->value, value, &chance))) ||
		    ((node->depth > 1) && (newnode->value < value)))
		{
		    value = newnode->value;
		    delmovelist (movel);
		    movel = newmovel;
//		    unshiftll (movel, node->movelist[i]->copy());
		    movel->push_back (node->movelist[i]->copy());

		    b = min (b, value);

		    if (value < alfa)
		    {
			break;
		    }
		}
		else
		{
		    delmovelist (newmovel);
		}
	    }
	}
    }

    node->value = value;

    if (value <= alfa)
	node->upper = value;
    if ((value > alfa) && (value < beta))
	node->lower = node->upper = value;
    if (value >= beta)
	node->lower = value;

    // cleanup children-nodes if past maximum memory-depth
    if (node->depth > memorydepth)
    {
	for (int i = node->childlist.size() - 1; i >= 0; i--)
	{
	    delmemtree (node->childlist[i]);
	    node->childlist.pop_back();
	}
    }

    // what if no valid move ?
    // is it enough that movel contains NULL in that case ?

    return (movel);
}


vector<basemove *> * ai_player::mtdf (void * startgame, int starttype)
{
    searchnode * firstnode;
    vector<basemove *> * move;

    state = AI_OK;
    stopnow = 0;

    if (randomchoose)
	srand (time (NULL));

    firstnode = new searchnode;

    firstnode->gameposition = startgame;
    firstnode->finished = 0;
    firstnode->depth = 1; 
    firstnode->value = 0;
    firstnode->upper = 1001;
    firstnode->lower = -1001;
    firstnode->type = starttype;
//    firstnode->movelist = NULL;
//    firstnode->childlist = NULL;

    move = mtdf (firstnode);
    lastvalue = firstnode->value;

    delmemtree (firstnode, 0);

    if (move != NULL)
	reverse (move->begin(), move->end());
    return (move);
}


vector<basemove *> * ai_player::mtdf (searchnode * node)
{
    int value,
	upper,
	lower,
	gamma;
    vector<basemove *> * bestmove = NULL;
    
    value = node->value;
    upper = 1001;
    lower = -1001;

    while ((lower < upper) && (stopnow == 0))
    {
	if (value == lower)
	{
	    gamma = value + 1;
	}
	else
	{
	    gamma = value;
	}

	delmovelist (bestmove);

//	bestmove = mt (node, gamma);  // werkt niet
//	bestmove = minimax_ab (node, gamma-1, gamma); // werkt wel
	bestmove = ab_mem (node, gamma-1, gamma);  // werkt ook, en goed
	value = node->value;

	if (value < gamma)
	{
	    upper = value;
	}
	else
	{
	    lower = value;
	}
    }

    return (bestmove);
}


// iterative deepening mtdf
vector<basemove *> * ai_player::mtdf_id (void * startgame, 
					 int starttype = MAX, 
					 float mtime = 0.0)
{
    searchnode * firstnode;
    int oldvalue,
	newvalue,
	savedepth,
	i;
    vector<basemove *> * bestmove = NULL,
	* newmove;
    struct timezone tz;
    
    maxtime = mtime;
    gettimeofday (&basetime, &tz);
    state = AI_OK;
    stopnow = 0;

    if (randomchoose)
	srand (time (NULL));

    firstnode = new searchnode;

    firstnode->gameposition = startgame;
    firstnode->finished = 0;
    firstnode->depth = 1; 
    firstnode->value = 0;
    firstnode->upper = 1001;
    firstnode->lower = -1001;
    firstnode->type = starttype;
//    firstnode->movelist = NULL;
//    firstnode->childlist = NULL;

    savedepth = maxdepth;
    oldvalue = newvalue = 0;
    for (i = 1; i <= savedepth; i++)
    {
	maxdepth = i;
	// each mtdf-call is not started with the value from the previous run
	// but with the one before that
	firstnode->value = oldvalue;
	reset_ul (firstnode);

	newmove = mtdf (firstnode);

	if (stopnow)
	{
	    delmovelist (newmove);
	    break;
	}

	oldvalue = newvalue;
	newvalue = firstnode->value;
	
	delmovelist (bestmove);
	bestmove = newmove;

	if (halfoutoftime ())
	{   /* soft timeout */
	    state = AI_TIMEUP;
	    break;
	}
    }

    maxdepth = savedepth;
    lastvalue = firstnode->value;

    delmemtree (firstnode, 0);

    if (bestmove != NULL)
	reverse (bestmove->begin(), bestmove->end());

    return (bestmove);
}


/*
** if flag is 0, then the gameposition will not be deleted
** this should be used for the root of the nodetree
*/
void ai_player::delmemtree (searchnode * node, int flag)
{
//    searchnode * child;
//    void * moveitem;
    int i, len;

    if (flag)
    {
	delgamefunc (node->gameposition);
    }

    if (!node->movelist.empty())
    {
	len = node->movelist.size();
	for (i = 0; i < len; i++)
	{
	    delete node->movelist[i];
	}
    }

    if (!node->childlist.empty())
    {
	len = node->childlist.size();
	for (i = 0; i < len; i++)
	{
	    delmemtree (node->childlist[i]);
	}
    }

    delete node;

    return;
}


/*
** reset upper and lower limits in all nodes from the memory tree
*/
void ai_player::reset_ul (searchnode * node)
{
//    searchnode * child;
    int i, len;

    node->upper = 1001;
    node->lower = -1001;

    if (!node->childlist.empty())
    {
	len = node->childlist.size();
	for (i = 0; i < len; i++)
	{
	    reset_ul (node->childlist[i]);
	}
    }

    return;
}


inline void ai_player::delmovelist (vector<basemove *> * todel)
{
    if (todel != NULL)
    {
	int len = todel->size(),
	    i;
	
	for (i = 0; i < len; i++)
	    delete (*todel)[i];
	
	delete todel;
    }

    return;
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
int ai_player::equ_l (int nr1, int nr2, int * chance)
{
    int randnr;

    if (randomchoose == 0)
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
int ai_player::equ_h (int nr1, int nr2, int * chance)
{
    int randnr;

    if (randomchoose == 0)
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


/*
** check if a child-node already exists,
** if not, create a new one
*/
inline searchnode * ai_player::nodechild (searchnode * node, int nr)
{
    searchnode * newnode;

    if (nr < node->childlist.size())
	return (node->childlist[nr]);


#if 0
/* just for testing */
/* check if the previous nr exists, it should */
if ((nr > 1) && (llitembynr (node->childlist, nr-1) == NULL))
{
    printf ("\nERROR: ai_player::nodechild\n\n");
    exit (0);
}
#endif 

    /* create a new node */
    newnode = new searchnode;
    newnode->gameposition = NULL;
    newnode->finished = -1;
    newnode->depth = node->depth;
    newnode->value = 0;
    newnode->upper = 1001;
    newnode->lower = -1001;
    newnode->type = node->type;
//    newnode->movelist = NULL;
//    newnode->childlist = NULL;

    node->childlist.push_back (newnode);
    
    return (newnode);
}


inline int ai_player::outoftime ()
{
    struct timeval tv;
    struct timezone tz;
    float timedif;

    if (maxtime == 0.0)
	return (0);

    gettimeofday (&tv, &tz);

    timedif = (tv.tv_sec - basetime.tv_sec) +
	(float) (tv.tv_usec - basetime.tv_usec) / 1000000;

    if ((maxtime - timedif) < 0.0)
    {
//cout << "hard timeout\n";
	return (1);
    }

    return (0);
}


inline int ai_player::halfoutoftime ()
{
    struct timeval tv;
    struct timezone tz;
    float timedif;

    if (maxtime == 0.0)
	return (0);

    gettimeofday (&tv, &tz);

    timedif = (tv.tv_sec - basetime.tv_sec) +
	(float) (tv.tv_usec - basetime.tv_usec) / 1000000;

    if ((maxtime/2 - timedif) < 0.0)
    {
//cout << "soft timeout\n";
	return (1);
    }

    return (0);
}
