/*
** $Id: configfile.h,v 1.2 1998/11/01 18:43:49 kurt Exp $
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

#ifndef _CONFIGFILE_H_
#define _CONFIGFILE_H_ 1

#include "linklist.h"
#include <malloc.h>

#ifndef __cplusplus
listheader * readconfigfile (char * fname);
int writeconfigfile (char * fname, listheader * configlist);
int findconfigvalue (listheader * clist, char * name, char colour, int defval);
void clearconfiglist (listheader * configlist);
void changeconfigvalue (listheader * clist, char * name, int newval);
#else
extern "C" listheader * readconfigfile (char * fname);
extern "C" int writeconfigfile (char * fname, listheader * configlist);
extern "C" int findconfigvalue (listheader * clist, char * name, 
				char colour, int defval);
extern "C" void clearconfiglist (listheader * configlist);
extern "C"void changeconfigvalue (listheader * clist, char * name, int newval);
#endif

#endif 
