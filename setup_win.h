/*
** $Id: setup_win.h,v 1.4 1998/11/01 16:29:00 kurt Exp $
**
** contains everything concerning the setupwindow
** also includes reading and writing from the configuration-file
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

#ifndef _SETUP_WIN_H_
#define _SETUP_WIN_H_ 1

#include "linklist.h"

typedef struct {
    listheader * configlist;
    int searchdepth;
    int animate;
    int waitcomputer;
    int waitremove;
    int showposname;
} configvalues;

int show_setup (configvalues * config);
void retrieveconfig (configvalues * config);
void writeconfig (configvalues * config);

#endif
