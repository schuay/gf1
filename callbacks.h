/*
** $Id: callbacks.h,v 1.14 2000/02/28 19:57:53 kurt Exp $
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

#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_ 1

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <time.h>
#include "gipf_ui.H"
#include "thegame.h"

void show_about (int flag);
void remove_about (void * aboutwindow);

void gf1_alert (char * message);
int gf1_question (char * line1, char * line2);
int gf1_questionsave (char * line1, char * line2);

void changeinterface (int flag, gamestruct * curgame);

enum { /* interface types */
    INTERFACE_PLAY,
    INTERFACE_EDIT
};

void setlostcounters (void);

extern int interrupt_computer;
void stop_thinking (Fl_Button * stopbutton, void * data);
Fl_Window * create_thinkwindow (void);
void timertostr (float newtimer, char * tempstr);
Fl_Window * gf1_help (void);
void showhelpsection (void);
void setcurrentfilename (char * str);
char * getcurrentfilename (void);
char * file_chooser (char * windowtitle, char * pattern, char * path);

#endif
