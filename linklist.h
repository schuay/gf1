/*
**++
**  FACILITY:  linklist.h
**
**  MODULE DESCRIPTION:
**
**      header file for the functions from linklist.c
**
**  AUTHORS:
**
**      Kurt Van den Branden
**
**  CREATION DATE:  22/01/97
**
**--
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

#ifndef _LINKLIST_H_
#define _LINKLIST_H_ 1

/*
** use the following define to include some improvements for making
** some functions faster (llitembynr en pushll)
*/
#define FASTLLIST 1

typedef struct _listitem {
    void	        * itemptr; /* pointer to user data structure */
    struct _listitem	* next;
} listitem;

/*
** header for a list, this is what the user will use to call
** the functions
*/
typedef struct {
    listitem	* firstitem;
#ifdef FASTLLIST
    int curnr;
    listitem * curptr;
    listitem * lastptr;
#endif
} listheader;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FASTLLIST
int newlist (listheader * head);
#else
#define newlist(x)     x->firstitem = NULL;
#endif
    
int insertll (listheader * head, void * newitem, int (* compfunction)());
void * searchll (listheader * head, void * item, int (* compfunction)());
int emptyll (listheader * head, void (* delfunction)(void *));
int doll (listheader * head, void (* dofunction)());
void * getnextll (listheader * head, void * item);
listheader * copyll (listheader * head, void * (* copyfunction)());
void * llitembynr (listheader * head, int nr);
void * llrembynr (listheader * head, int nr);
int pushll (listheader * head, void * newitem);
int unshiftll (listheader * head, void * newitem);
void * llremitem (listheader * head, void * newitem, int (* compfunction)());
int findlastll (listheader * head, int (* checkfunction)());
int lllength (listheader * head);
    
#ifdef __cplusplus
}
#endif

#endif
