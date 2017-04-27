/*
** $Id: callbacks.cxx,v 1.24 2000/02/28 19:57:42 kurt Exp $
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
#include <time.h>
#include <FL/Fl.H>
#include <FL/fl_ask.H>

#ifdef NEWFILECHOOSER
#include <FileChooser.h>
#else
#include <FL/fl_file_chooser.H>
#endif

#include "callbacks.h"
#include "thegame.h"
#include "helptext.h"

extern char versiontext[];
int interrupt_computer = 0;
char currentfilename[100] = "";

void show_about (int flag)
{
    Fl_Window * aboutw;

    aboutw = make_aboutwindow ();
    box_versiontext->label (versiontext);
    aboutw->show();

    if (flag)
    {
	Fl::add_timeout (5.0, remove_about, (void *) aboutw);
	return;
    }

    return;
}

void remove_about (void * aboutwindow)
{
    Fl_Window * aboutw = (Fl_Window *) aboutwindow;

    Fl::remove_timeout (remove_about, aboutwindow);
    delete aboutw;

    return;
}


void gf1_alert (char * message)
{
    Fl_Widget * icon;

    icon = fl_message_icon ();
    icon->box (FL_ENGRAVED_BOX);
    icon->color (FL_GRAY);

    fl_alert (message);

    return;
}


/*
** returns:
**    1 : yes was pressed
**    0 : no was pressed
**
** position of the window will be just under the gameboard
*/
int gf1_question (char * line1, char * line2)
{
    Fl_Window * questionw;
    int theend = 0,
	value;
    Fl_Widget * mainwin;

    questionw = make_questionwindow ();
    mainwin = gameboard->parent ();
    questionw->position (mainwin->x() + gameboard->x(),
			 mainwin->y() + mainwin->h() - questionw->h());
    qtext1->label (line1);
    qtext2->label (line2);

    questionw->show();

    while (!theend)
    {
	Fl::wait();    

	Fl_Widget *x;
	while ((x = Fl::readqueue())) 
	{
	    if (x == button_qyes)
	    {
		value = 1;
		theend = 1;
	    }
	    else if (x == button_qno)
	    {
		value = 0;
		theend = 1;
	    }
	}
    }

    delete questionw;

    return (value);
}


/*
** returns:
**    2 : save was pressed
**    1 : yes was pressed
**    0 : no was pressed
**
** position of the window will be just under the gameboard
*/
int gf1_questionsave (char * line1, char * line2)
{
    Fl_Window * questionw;
    int theend = 0,
	value;
    Fl_Widget * mainwin;

    questionw = make_questionsavewindow ();
    mainwin = gameboard->parent ();
    questionw->position (mainwin->x() + gameboard->x(),
			 mainwin->y() + mainwin->h() - questionw->h());
    qstext1->label (line1);
    qstext2->label (line2);

    questionw->show();

    while (!theend)
    {
	Fl::wait();    

	Fl_Widget *x;
	while ((x = Fl::readqueue())) 
	{
	    if (x == button_qsyes)
	    {
		value = 1;
		theend = 1;
	    }
	    else if (x == button_qsno)
	    {
		value = 0;
		theend = 1;
	    }
	    else if (x == button_qssave)
	    {
		value = 2;
		theend = 1;
	    }
	}
    }

    delete questionw;

    return (value);
}


void changeinterface (int flag, gamestruct * curgame)
{
    listheader * loglines;
    char * line,
	tempstr[100];

    if (flag == INTERFACE_PLAY)
    {
	/* interface items for playing */
	count_lostwhite->hide();
	count_lostblack->hide();
	choice_piecetype->hide();
	menu_piecetype->hide();

	text_to->show();
	text_from->show();
	button_execute->show();

	/* the browser widget for logging */
	logbrowser->clear();
	if (curgame != NULL)
	{
	    logbrowser->color (FL_WHITE);
	    loglines = logtobrowser (curgame->movelog);
	    while ((line = (char *) llrembynr (loglines, 1)) != NULL)
	    {
		logbrowser->add (line);
		free (line);
	    }
	    free (loglines);
	}
	else
	    logbrowser->color (FL_GRAY);
	logbrowser->bottomline (logbrowser->size());

	/* player names and timers */
	if (curgame != NULL)
	{
	    whitename->value (curgame->player[0].name);
	    blackname->value (curgame->player[1].name);
	    if (curgame->player[0].fulltime == -1)
	    {
		whitetimer->value ("-:--:--");
		blacktimer->value ("-:--:--");
	    }
	    else
	    {
		timertostr (curgame->player[0].timer, tempstr);
		whitetimer->value (tempstr);
		timertostr (curgame->player[1].timer, tempstr);
		blacktimer->value (tempstr);
	    }
	    /* background color for player-names shows whose turn it is */
	    if (curgame->state == GAME_GO)
	    {
		if (b_next_piece (curgame->boards[0]) == 'o')
		{
		    whitename->color (FL_RED);
		    blackname->color (FL_GRAY);
		}
		else
		{
		    whitename->color (FL_GRAY);
		    blackname->color (FL_RED);
		}
	    }
	    else
	    {
		whitename->color (FL_GRAY);
		blackname->color (FL_GRAY);
	    }
	    whitename->redraw ();
	    blackname->redraw ();
	}
	else
	{
	    whitename->value ("");
	    blackname->value ("");
	    whitetimer->value ("");
	    blacktimer->value ("");
	    whitename->color (FL_GRAY);
	    blackname->color (FL_GRAY);
	    whitename->redraw ();
	    blackname->redraw ();
	}

	/* menu items for playing */
	menu_new->activate();
	menu_loadgame->activate();
	if (curgame != NULL)
	{
	    menu_savegame->activate();
	    menu_savelog->activate();
	    /*
	    ** show the undo-button when there is a board to restore and
	    ** when we are not playing a timed game
	    */
	    if ((curgame->boards[2] != NULL) &&
		(curgame->player[0].fulltime == -1) &
		(curgame->state == GAME_GO))
		menu_undo->activate();
	    else
		menu_undo->deactivate();
	}
	else
	{
	    menu_savegame->deactivate();
	    menu_savelog->deactivate();
	    menu_undo->deactivate();
	}
	menu_edit->activate();
	menu_start->deactivate();
	menu_compumove->deactivate();
	menu_clear->deactivate();
	menu_restore->deactivate();
	menu_loadboard->deactivate();

	if (curgame == NULL)
	    menu_saveboard->deactivate();
	else
	    menu_saveboard->activate();
#ifdef HAVEGD
	if (curgame == NULL)
	    menu_makedrawing->deactivate();
	else
	    menu_makedrawing->activate();
#else
	menu_makedrawing->deactivate();
#endif

	/* state of the gameboard */
	if ((curgame == NULL) ||
	    (curgame->state == GAME_STOP))
	{
	    gameboard->setstate (BOARD_NONE);
	}
	else
	{
	    gameboard->setstate (BOARD_PLAY);
	}
    }
    else
    {
	/* interface items for editing */
	text_to->hide();
	text_from->hide();
	button_execute->hide();

	count_lostwhite->show();
	count_lostblack->show();
	choice_piecetype->show();
	menu_piecetype->show();

	/* the browser widget for logging */
	logbrowser->clear();
	logbrowser->color (FL_GRAY);

	/* player names and timers */
	whitename->value ("");
	blackname->value ("");
	whitetimer->value ("");
	blacktimer->value ("");
	whitename->color (FL_GRAY);
	blackname->color (FL_GRAY);
	whitename->redraw ();
	blackname->redraw ();

	/* menu items for editing */
	menu_new->deactivate();
	menu_loadgame->deactivate();
	menu_savegame->deactivate();
	menu_savelog->deactivate();
	menu_edit->deactivate();
	menu_start->activate();
	menu_compumove->activate();
	menu_clear->activate();
	menu_restore->activate();
	menu_loadboard->activate();
	menu_saveboard->activate();
	menu_undo->deactivate();

#ifdef HAVEGD
	menu_makedrawing->activate();
#else
	menu_makedrawing->deactivate();
#endif

	/* state of the gameboard */
	gameboard->setstate (BOARD_EDIT);
    }

    /* always allowed menu items */
    menu_setup->activate();
    menu_exit->activate();
    menu_help->activate();
    menu_about->activate();

    /* problems with the UNDO-button if I don't do this */
    menubar->redraw ();

    return;
}


void setlostcounters (void)
{
    board * tempboard;

    tempboard = gameboard->getboard ();

    if (tempboard == NULL)
    {
	count_lostwhite->value (0);
	count_lostblack->value (0);
    }
    else
    {
	count_lostwhite->value (b_white_lost (tempboard));
	count_lostblack->value (b_black_lost (tempboard));
    }

    return;
}


void stop_thinking (Fl_Button * stopbutton, void * data)
{
    interrupt_computer = 1;

    return;
}


Fl_Window * create_thinkwindow (void)
{
    Fl_Window * thinkwindow;
    Fl_Widget * mainwin;

    thinkwindow = make_thinkwindow ();
    mainwin = logbrowser->parent ();
    thinkwindow->position (logbrowser->x() + mainwin->x() + 10,
			   logbrowser->y() + mainwin->y() + 10);
    thinkwindow->show ();
    Fl::check();

    return (thinkwindow);
}


void timertostr (float newtimer, char * tempstr)
{
    int thetimer = (int) newtimer;

    if (newtimer <= 0.0)
    {
	sprintf (tempstr, "YOULOSE");
    }
    else
    {
	sprintf (tempstr, "%d:%02d:%02d", 
		 thetimer / 3600,
		 (thetimer % 3600) / 60,
		 thetimer % 60);
    }

    return;
}


Fl_Window * gf1_help (void)
{
    int i;
    Fl_Window * helpw;

    helpw = make_helpwindow ();

    for (i = 0; i < NRHELPLINES; i++)
    {
	browser_help->add (helplines[i]);
    }
    browser_help->topline (1);

    for (i = 0; i < NRSECTIONLINES; i++)
    {
	browser_sections->add (sectionlines[i].line);
    }
    browser_sections->topline (1);

    return (helpw);
}

void showhelpsection (void)
{
    int val = browser_sections->value ();
    if (val != 0)
    {
	browser_help->topline (sectionlines[val-1].offset);
    }

    return;
}


void setcurrentfilename (char * str)
{
    char tempstr[100],
	* pos;

    strcpy (currentfilename, str);
    if (currentfilename[0] == '\0')
    {
	sprintf (tempstr, "Gipf for One");
    }
    else
    {
#ifdef MSWIN
	if ((pos = strrchr (currentfilename, '\\')) != NULL)
#else
	if ((pos = strrchr (currentfilename, '/')) != NULL)
#endif
	    pos++;
	else
	    pos = currentfilename;
	sprintf (tempstr, "Gipf for One (%s)", pos);
    }

    gf1_window->label (tempstr);
    return;
}

char * getcurrentfilename (void)
{
    return (currentfilename);
}

char * file_chooser (char * windowtitle, char * pattern, char * path)
{
    char * filename = NULL;

#ifdef NEWFILECHOOSER
    FileIcon::load_system_icons();

    FileChooser fc(path, pattern, FileChooser::CREATE, windowtitle);

    fc.show();
    while (fc.visible())
	Fl::wait();

    if (fc.count())
	filename = fc.value();
#else
    filename = fl_file_chooser (windowtitle, pattern, path);
#endif

    return (filename);
}
