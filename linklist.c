/*
**  FACILITY:  linklist.c
**
**  MODULE DESCRIPTION:
**
**      general routines for linked lists
**	    + newlist: initialize new list
**	    + insertll: insert item in list
**	    + searchll: search for item in list
**	    + emptyll: delete all items from a linked list
**	    + doll: run a function for every item from the linked list
**	    + getnextll: get the next item from the list
**          + copyll: copy a complete linked list
**          + llitembynr: return pointer to the data of item nr of the list
**          + llrembynr: remove item nr from the list and return pointer
**                       to the data
**          + pushll: add item to the end of a linked list
**          + findlastll: find last item in the last that
**                        corresponds with what we want
**	    (+ delll: delete an item from the list)
**
**  AUTHORS:
**
**      Kurt Van den Branden
**
**  CREATION DATE:  22/01/97
**
**  DESIGN ISSUES:
**
**      these functions should be as general as possible
**	you can put any data at the nodes from the list
**	you have to provide your own compare-routine
**
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
**
**  INCLUDE FILES
**
*/

#include <stdio.h>
#include <malloc.h>
#include "linklist.h"

#ifdef FASTLLIST
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      newlist: initialize a new linked list
**
**  FORMAL PARAMETERS:
**
**      head: pointer to header of new list
**
**  RETURN VALUE:
**
**      0: OK
**	-1: error
**
**--
*/
int newlist (listheader * head)
{
    head->firstitem = NULL;
    head->curnr = -1;
    head->curptr = NULL;
    head->lastptr = NULL;
    return (0);
}
#endif

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      insertll: insert item in a linked list
**
**  FORMAL PARAMETERS:
**
**      head: pointer to head of linked list
**	newitem: void-pointer to item that has to be inserted in the list
**	compfunction: pointer to a function that can compare to items
**		      it should return -1, 0, 1. to indicate that the
**		      first item is smaller, equal, larger than the second
**
**  RETURN VALUE:
**
**      0 : OK
**	-1: error
**	1 : item already in the list (0 was returned by compfunction)
**
**--
*/
int insertll (listheader * head, void * newitem, int (* compfunction)())
{
    listitem *curitem, 
	     *previtem, 
	     *dumitem;
    int compresult;

#ifdef FASTLLIST
    head->curnr = -1;
    head->curptr = NULL;
#endif

    if (head->firstitem == NULL)
    {
    	dumitem = (listitem *) malloc (sizeof (listitem));
	dumitem->itemptr = newitem;
	dumitem->next = NULL;
	head->firstitem = dumitem;
#ifdef FASTLLIST
	head->lastptr = dumitem;
#endif
	return (0);
    }

    previtem = NULL;
    curitem = head->firstitem;
    while (curitem != NULL)
    {
	compresult = (* compfunction)(curitem->itemptr, newitem);
    	if (compresult == 0)
	{
	    return (1);
	}
	else if (compresult > 0)
	{
	    dumitem = (listitem *) malloc (sizeof (listitem));
	    dumitem->itemptr = newitem;
	    dumitem->next = NULL;
	    if (previtem == NULL)
	    {
	    	head->firstitem = dumitem;
	    }
	    else
	    {
	    	previtem->next = dumitem;
	    }
	    dumitem->next = curitem;
	    return (0);
	}
	else
	{
	    previtem = curitem;
	    curitem = curitem->next;
	}
    }
    dumitem = (listitem *) malloc (sizeof (listitem));
    dumitem->itemptr = newitem;
    dumitem->next = NULL;
    previtem->next = dumitem;

#ifdef FASTLLIST
    head->lastptr = dumitem;
#endif

    return (0);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      llremitem: delete item from a linked list
**
**  FORMAL PARAMETERS:
**
**      head: pointer to head of linked list
**	newitem: void-pointer to an item that has to be compared with
**               what has to be deleted
**	compfunction: pointer to a function that can compare to items
**		      it should return -1, 0, 1. to indicate that the
**		      first item is smaller, equal, larger than the second
**
**  RETURN VALUE:
**
**      pointer to data (has to be deleted by calling procedure)
**	NULL
**
**--
*/
void * llremitem (listheader * head, void * newitem, int (* compfunction)())
{
    listitem *curitem, 
	     *previtem;
    int compresult;
    void * data;

    if ((head == NULL) || (head->firstitem == NULL))
    {
	return (NULL);
    }

    previtem = NULL;
    curitem = head->firstitem;
    while (curitem != NULL)
    {
	compresult = (* compfunction)(curitem->itemptr, newitem);
    	if (compresult == 0)
	{
	    if (previtem == NULL)
	    {
		head->firstitem = curitem->next;
	    }
	    else
	    {
		previtem->next = curitem->next;
	    }
#ifdef FASTLLIST
	    if (head->lastptr == curitem)
		head->lastptr = previtem;
	    head->curptr = NULL;
	    head->curnr = -1;
#endif	    
	    data = curitem->itemptr;
	    free (curitem);
	    return (data);
	}
	else if (compresult > 0)
	{
	    return (NULL);
	}
	else
	{
	    previtem = curitem;
	    curitem = curitem->next;
	}
    }

    return (NULL);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      pushll: add item to the end of a linked list
**
**  FORMAL PARAMETERS:
**
**      head: pointer to head of linked list
**	newitem: void-pointer to item that has to be inserted in the list
**
**  RETURN VALUE:
**
**      0 : OK
**	-1: error
**
**--
*/
int pushll (listheader * head, void * newitem)
{
    listitem *curitem, 
	     *previtem, 
	     *dumitem;

    dumitem = (listitem *) malloc (sizeof (listitem));
    dumitem->itemptr = newitem;
    dumitem->next = NULL;
    
#ifdef FASTLLIST
    if (head->lastptr == NULL)
	head->firstitem = dumitem;
    else
	head->lastptr->next = dumitem;
    head->lastptr = dumitem;
#else
    if (head->firstitem == NULL)
    {
	head->firstitem = dumitem;
	return (0);
    }

    previtem = NULL;
    curitem = head->firstitem;
    while (curitem != NULL)
    {
	previtem = curitem;
	curitem = curitem->next;
    }
    previtem->next = dumitem;
#endif
    
    return (0);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      unshiftll: add item to the beginning of a linked list
**
**  FORMAL PARAMETERS:
**
**      head: pointer to head of linked list
**	newitem: void-pointer to item that has to be inserted in the list
**
**  RETURN VALUE:
**
**      0 : OK
**	-1: error
**
**--
*/
int unshiftll (listheader * head, void * newitem)
{
    listitem *dumitem;

    dumitem = (listitem *) malloc (sizeof (listitem));
    dumitem->itemptr = newitem;
    dumitem->next = head->firstitem;
    head->firstitem = dumitem;

#ifdef FASTLLIST
    if (head->lastptr == NULL)
	head->lastptr = dumitem;
    head->curptr = NULL;
    head->curnr = -1;
#endif
    
    return (0);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      searchll : find an item in a linked list
**
**  FORMAL PARAMETERS:
**
**      head: pointer to listhead
**	item: pointer to item to search for
**	compfunction: pointer to a function that can compare to items
**		      it should return -1, 0, 1. to indicate that the
**		      first item is smaller, equal, larger than the second
**
**  RETURN VALUE:
**
**      pointer to the found item, or NULL if not found
**
**--
*/
void * searchll (listheader * head, void * item, int (* compfunction)())
{
    listitem * curitem;
    int compresult;

    curitem = head->firstitem;
    while (curitem != NULL)
    {
	compresult = (* compfunction)(curitem->itemptr, item);
    	if (compresult == 0)
	{
	    return (curitem->itemptr);
	}
	else if (compresult > 0)
	{
	    return (NULL);
	}
	curitem = curitem->next;
    }
    return (NULL);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      emptyll: remove all items from a linked list
**
**  FORMAL PARAMETERS:
**
**      head: pointer to the listheader
**	delfunction: pointer to a function that is called to delete each
**		     item from the list, it is called with 1 parameter,
**		     a void-pointer to the item, no return-value is expected
**
**  RETURN VALUE:
**
**      0: OK
**	-1: error
**
**--
*/
int emptyll (listheader * head, void (* delfunction)(void *))
{
    listitem * curitem, 
	     * dumitem;

    if (head == NULL)
    {
	return (0);
    }

    curitem = head->firstitem;
    while (curitem != NULL)
    {
    	(* delfunction)(curitem->itemptr);
	dumitem = curitem->next;
	free (curitem);
	curitem = dumitem;
    }
    head->firstitem = NULL;

#ifdef FASTLLIST
    head->lastptr = NULL;
    head->curptr = NULL;
    head->curnr = -1;
#endif
    
    return (0);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      doll: execute a function for all the items from a linked list
**
**  FORMAL PARAMETERS:
**
**      head: pointer to the listheader
**	dofunction: pointer to a function that is called for each
**		     item from the list, it is called with 1 parameter,
**		     a void-pointer to the item, no return-value is expected
**
**  RETURN VALUE:
**
**      0: OK
**	-1: error
**
**--
*/
int doll (listheader * head, void (* dofunction)())
{
    listitem * curitem;

    curitem = head->firstitem;
    while (curitem != NULL)
    {
    	(* dofunction)(curitem->itemptr);
	curitem = curitem->next;
    }
    return (0);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      llitembynr: return the data for item nr from the list
**
**  FORMAL PARAMETERS:
**
**      head: pointer to the listheader
**      nr : item to return, the first item of the list = 1
**
**  RETURN VALUE:
**
**      pointer to data
**	NULL on error
**
**--
*/
void * llitembynr (listheader * head, int nr)
{
    listitem * curitem;
    int counter;

    if (head == NULL)
    {
	return (NULL);
    }

#ifdef FASTLLIST
    if (head->curnr == (nr - 1))
	curitem = head->curptr->next;
    else if (head->curnr == nr)
	curitem = head->curptr;
    else
    {
#endif
	curitem = head->firstitem;
	counter = 1;
	while ((curitem != NULL) && (counter != nr))
	{
	    counter++;
	    curitem = curitem->next;
	}
#ifdef FASTLLIST
    }
#endif
    
    if (curitem != NULL)
    {
#ifdef FASTLLIST
	head->curnr = nr;
	head->curptr = curitem;
#endif
	return (curitem->itemptr);
    }
    return (NULL);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      llrembynr: remove item nr from the list and return pointer to
**                 the data from this item
**
**  FORMAL PARAMETERS:
**
**      head: pointer to the listheader
**      nr : item to remove, the first item of the list = 1
**
**  RETURN VALUE:
**
**      pointer to data
**	NULL on error
**
**--
*/
void * llrembynr (listheader * head, int nr)
{
    listitem * previtem = NULL,
	* curitem;
    int counter;
    void * data;

    if (head == NULL)
    {
	return (NULL);
    }

#ifdef FASTLLIST
    // use information from the last llitembynr
    if (head->curnr == (nr - 1))
    {
	previtem = head->curptr;
	curitem = head->curptr->next;
    }
    else
    {
#endif
	curitem = head->firstitem;
	counter = 1;
	while ((curitem != NULL) && (counter != nr))
	{
	    counter++;
	    previtem = curitem;
	    curitem = curitem->next;
	}
#ifdef FASTLLIST
    }
#endif

    if (curitem != NULL)
    {
	if (previtem == NULL)
	{
	    head->firstitem = curitem->next;
	}
	else
	{
	    previtem->next = curitem->next;
	}
#ifdef FASTLLIST
	if (curitem == head->lastptr)
	    head->lastptr = previtem;
	head->curptr = NULL;
	head->curnr = -1;
#endif
	
	data = curitem->itemptr;
	free (curitem);
	return (data);
    }

    return (NULL);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      copyll: copy a complete linked list and all the data in it
**
**  FORMAL PARAMETERS:
**
**      head: pointer to the listheader
**	copyfunction: pointer to a function that is called for each
**		      item from the list, it is called with 1 parameter,
**		      a void-pointer to the item. 
**                    as return-value, a void pointer to a copy of the input
**                    item is expected
**
**  RETURN VALUE:
**
**      pointer to the head of the copied list
**
**--
*/
listheader * copyll (listheader * head, void * (* copyfunction)())
{
    listitem * curitem,
	* newitem = NULL,
	* dumitem;
    listheader * newheader;

    if (head == NULL)
    {
	return (NULL);
    }

    curitem = head->firstitem;
    newheader = (listheader *) malloc (sizeof (listheader));
    newheader->firstitem = NULL;
    while (curitem != NULL)
    {
	dumitem = (listitem *) malloc (sizeof (listitem)); 

	dumitem->next = NULL;
    	dumitem->itemptr = (* copyfunction)(curitem->itemptr);
	if (newitem == NULL)
	{
	    newheader->firstitem = dumitem;
	}
	else
	{
	    newitem->next = dumitem;
	}
	newitem = dumitem;

	curitem = curitem->next;
    }

#ifdef FASTLLIST
    newheader->lastptr = newitem;
    newheader->curptr = NULL;
    newheader->curnr = -1;
#endif
    
    return (newheader);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      getnextll
**
**  FORMAL PARAMETERS:
**
**      head: header of the linked list
**	curptr: pointer to the current item in the list
**
**  RETURN VALUE:
**
**      pointer to the next item in the linked list (the item after curptr)
**	the order of the list is the same order then during inclusion
**	if curptr is not found in the list, or there is no next item, the
**	NULL is returned.
**
**--
*/
void * getnextll (listheader * head, void * curptr)
{
    listitem * curitem;

    curitem = head->firstitem;
    while ((curitem != NULL) && (curitem->itemptr != curptr))
    {
	curitem = curitem->next;
    }
    if ((curitem == NULL) || (curitem->next == NULL))
    {
    	return (NULL);
    }

    curitem = curitem->next;
    return (curitem->itemptr);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      findlastll
**
**  FORMAL PARAMETERS:
**
**      head: header of the linked list
**	checkfunction: function to be called with each item from the list
**            and returning 1 or 0 (1: item we are looking for, 0: not)
**
**  RETURN VALUE:
**
**      number of the last item in the list that makes the checkfunction
**      return 1
**
**--
*/
int findlastll (listheader * head, int (* checkfunction)())
{
    listitem * curitem;
    int theitem = 0,
	nr = 1;

    curitem = head->firstitem;
    while (curitem != NULL)
    {
	if ((* checkfunction) (curitem->itemptr) == 1)
	    theitem = nr;

	nr++;
	curitem = curitem->next;
    }

    return (theitem);
}


/*
** returns the number of items in the linked list
*/
int lllength (listheader * head)
{
    listitem * curitem;
    int counter = 0;

    curitem = head->firstitem;
    while (curitem != NULL)
    {
	counter++;
	curitem = curitem->next;
    }

    return (counter);
}
