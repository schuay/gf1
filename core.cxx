/*
** $Id: core.cxx,v 1.42 2000/02/28 19:52:14 kurt Exp $
**
** the core routines for the program
** compile it with C++ but most of it will probably be C
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

char versiontext[] = "Gipf for one  v 1.03 (28/02/2000)";

#include <stdio.h>
#include <stdlib.h>

#include <FL/Fl_Menu_Button.H>

#include "fl_board.h"
#include "gipf_ui.H"
#include "callbacks.h"
#include "setup_win.h"
#include "thegame.h"
#include "configfile.h"

//#define GIPF_ICON 1
#ifdef GIPF_ICON
#include <FL/x.H>
#include "i_blauw.xpm"
#endif

#ifdef MSWIN // for the icon
#define GF1_ICON 345
#include <FL/x.H>
#endif

int stop_program = 0; /* set by the callback from the main window */

board * loadboard (void);
void saveboard (board * boardptr);
void savelog (gamelog * logptr);


int main (int argc, char ** argv)
{
    int theend = 0,
	newval;
    Fl_Window * mainwin,
	* helpwin;
    configvalues gf1_config;
    gamestruct * currentgame = NULL,
	* newgame = NULL;
#ifdef GIPF_ICON
    Pixmap p;
#endif

    // new random seed
    srand (time (NULL));

    /* read configuration */
    retrieveconfig (& gf1_config);

    /* create the main window */
    mainwin = create_mainwindow();

#ifdef GIPF_ICON
    /* set the icon */
    p = XCreateBitmapFromData(fl_display, DefaultRootWindow(fl_display),
			      (char *) i_blauw_xpm, 64, 64);
    mainwin->icon((char *)p);
#endif

#ifdef MSWIN
    if (fl_display == 0)
	fl_display = GetModuleHandle (NULL);

    mainwin->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(GF1_ICON)));
#endif

    /* setup the gameboard */
    gameboard->setoutputwidgets(whitepieces, lostwhite, drawwhite,
				blackpieces, lostblack, drawblack,
				text_from, text_to);
    gameboard->setexecbutton (button_execute);
    gameboard->setshowposname (gf1_config.showposname);

    /* create helpwindow but don't show it */
    helpwin = gf1_help ();

    /* set interface-items */
    changeinterface (INTERFACE_PLAY, NULL);

    /* show the main window */
    Fl::visual (FL_DOUBLE|FL_INDEX);
    Fl::background (0xb0, 0xb0, 0xb0);
    mainwin->show();
//    mainwin->show(argc, argv);   // don't do this, uses default user
                                   // colors on windows

    /*
    ** on windows95 the window doesn't stay on for 5 seconds
    ** if I don't add the following line
    */
    Fl::wait();
    /* show the about-window, it will go away after 5 seconds */
    show_about (1);

    while (!theend)
    {
	/* The program must be stopped when wait() returns 0 */
	if (Fl::wait() == 0)
	{
	    break;
	}

	Fl_Widget *x;
	while ((x = Fl::readqueue())) 
	{
	    if (x == gf1_window)
	    {
//		printf ("mainwin event\n");
	    }
	    else if (x == menubar)
	    {
		const Fl_Menu * z;
		Fl_Menu_Bar * y = (Fl_Menu_Bar *) x;

		z = y->mvalue ();
		if (z == menu_new)
		{
		    newgame = show_new (currentgame);
		    if (newgame != NULL)
		    {
			stoptimer (currentgame);
			delete_game (currentgame);
			currentgame = newgame;
			currentgame->config = & gf1_config;

			gameboard->setboard (currentgame->boards[0]);

			setupmove (currentgame);
		    }
		}
		else if (z == menu_loadgame)
		{
		    newgame = loadgame ();
		    if (newgame != NULL)
		    {
			stoptimer (currentgame);
			delete_game (currentgame);
			currentgame = newgame;
			currentgame->config = & gf1_config;

			gameboard->setboard (newgame->boards[0]);

			setupmove (currentgame);
		    }
		}
		else if (z == menu_savegame)
		{
		    savegame (currentgame);
		}
		else if (z == menu_savelog)
		{
		    if (currentgame != NULL)
			savelog (currentgame->movelog);
		    else
			savelog (NULL);
		}
		else if (z == menu_setup)
		{
		    if (show_setup (&gf1_config))
		    {
			writeconfig (&gf1_config);
			gameboard->setshowposname (gf1_config.showposname);
		    }
		}
		else if (z == menu_exit)
		{
		    theend = 1;
		}
		else if (z == menu_edit)
		{
		    board * tempboard;

		    stoptimer (currentgame);
		    if (currentgame == NULL)
		    {
			tempboard = b_new (T_TOURNAMENT);
			gameboard->setboard (tempboard);
			b_del (tempboard);
		    }
		    else
		    {   /* convert board to tournament configuration */
			tempboard = b_copy (gameboard->getboard ());
			if (b_white_gipf (gameboard->getboard ()) == -1)
			{
			    tempboard->gipfwhite = 0;
			    tempboard->gipfblack = 0;
			    tempboard->white += 3;
			    tempboard->black += 3;
			}
			tempboard->movecounter = 0;
			gameboard->setboard (tempboard);
			b_del (tempboard);
		    }
		    changeinterface (INTERFACE_EDIT, currentgame);

		    setlostcounters ();
		}
		else if (z == menu_start)
		{
		    newgame = show_start (gameboard->getboard ());
		    if (newgame != NULL)
		    {
			delete_game (currentgame);
			currentgame = newgame;
			currentgame->config = & gf1_config;

			gameboard->setboard (currentgame->boards[0]);

			setupmove (currentgame);
		    }
		}
		else if (z == menu_compumove)
		{
		    newgame = show_onemove (gameboard->getboard (), 
					    & gf1_config);
		    if (newgame != NULL)
		    {
			delete_game (currentgame);
			currentgame = newgame;
		    }
		}
		else if (z == menu_clear)
		{
		    board * tempboard;

		    tempboard = b_new (T_TOURNAMENT);
		    gameboard->setboard (tempboard);
		    b_del (tempboard);
		    setlostcounters ();
		}
		else if (z == menu_restore)
		{
		    changeinterface (INTERFACE_PLAY, currentgame);
		    if (currentgame == NULL)
		    {
			gameboard->setboard (NULL);
		    }
		    else
		    {
			gameboard->setboard (currentgame->boards[0]);

			setupmove (currentgame);
		    }
		}
		else if (z == menu_loadboard)
		{
		    board * tempboard = NULL;
		    
		    if ((tempboard = loadboard ()) != NULL)
		    {
			gameboard->setboard (tempboard);
			b_del (tempboard);

			setlostcounters ();
		    }
		}
		else if (z == menu_saveboard)
		{
		    saveboard (gameboard->getboard());
		}
		else if (z == menu_makedrawing)
		{
		    show_makegif (gameboard->getboard ());
		}
		else if (z == menu_undo)
		{
		    if ((currentgame != NULL) &&
			(currentgame->boards[2] != NULL))
		    {
			b_del (currentgame->boards[0]);
			b_del (currentgame->boards[1]);
			currentgame->boards[0] = currentgame->boards[2];
			currentgame->boards[1] = NULL;
			currentgame->boards[2] = NULL;
			currentgame->movecounter -= 
			    remlastmove (currentgame->movelog);
			currentgame->movecounter -= 
			    remlastmove (currentgame->movelog);

			setupmove (currentgame);
			gameboard->setboard (currentgame->boards[0]);
		    }
		}
		else if (z == menu_help)
		{
		    helpwin->show();
		}
		else if (z == menu_about)
		{
		    show_about (0);
		}
	    }
	    else if (x == choice_piecetype)
	    {
		const Fl_Menu * z;
		Fl_Choice * y = (Fl_Choice *) x;

		z = y->mvalue ();
		if (z == choice_white)
		{
		    gameboard->seteditpiece ('o');
		}
		else if (z == choice_whitegipf)
		{
		    gameboard->seteditpiece ('O');
		}
		else if (z == choice_black)
		{
		    gameboard->seteditpiece ('x');
		}
		else if (z == choice_blackgipf)
		{
		    gameboard->seteditpiece ('X');
		}
	    }
	    else if (x == menu_piecetype)
	    {
		const Fl_Menu * z;
		Fl_Menu_Button * y = (Fl_Menu_Button *) x;

		z = y->mvalue ();
		if (z == menu_white)
		{
		    choice_piecetype->value (0);
		    gameboard->seteditpiece ('o');
		}
		else if (z == menu_whitegipf)
		{
		    choice_piecetype->value (1);
		    gameboard->seteditpiece ('O');
		}
		else if (z == menu_black)
		{
		    choice_piecetype->value (2);
		    gameboard->seteditpiece ('x');
		}
		else if (z == menu_blackgipf)
		{
		    choice_piecetype->value (3);
		    gameboard->seteditpiece ('X');
		}
	    }
	    else if (x == count_lostwhite)
	    {
		newval = gameboard->setlostwhite 
		    ((int) count_lostwhite->value());
		/* just in case the new value is not valid */
		count_lostwhite->value ((double) newval);
	    }
	    else if (x == count_lostblack)
	    {
		newval = gameboard->setlostblack 
		    ((int) count_lostblack->value());
		/* just in case the new value is not valid */
		count_lostblack->value ((double) newval);
	    }
	    else if (x == button_execute)
	    {
		if (currentgame != NULL)
		{
		    if (humanmove (currentgame) != 0)
			gf1_alert (
			    "invalid move, check the from- and to-position !");

		    setupmove (currentgame);
		}
	    }
	}

	if (stop_program)
	{
	    theend = 1;
	}
    }

    stoptimer (currentgame);
    delete_game (currentgame);
    delete helpwin;
    delete mainwin;
    clearconfiglist (gf1_config.configlist);

    return (0);
}


/* 
** load a board from a file
**
** return NULL if nothing loaded
*/
board * loadboard (void)
{
    char * filename;
    board * newboard;
    xmlite_entity * root;
    xmlite_parser * theparser;

    // ask for filename
    if ((filename = file_chooser ("load board", "*.brd", NULL)) == NULL)
    {
	return (NULL);
    }

    theparser = new xmlite_parser (filename);
    root = theparser->parse();
    delete (theparser);
    if (root == NULL)
    {
	gf1_alert (" ERROR: wrong inputfile format");
	return (NULL);
    }

    newboard = b_from_xml (root);
    delete (root);

    return (newboard);
}


/* 
** save a board to a file
*/
void saveboard (board * boardptr)
{
    char * filename;
    xmlite_entity * root;

    if (boardptr == NULL)
    {
	gf1_alert (" no current board to be saved");
	return;
    }

    // ask for filename
    if ((filename = file_chooser ("save board", "*.brd", "./board.brd")) 
	== NULL)
    {
	return;
    }

    root = b_to_xml (boardptr);
    root->writetofile (filename);
    delete (root);

    return;
}


/* 
** save a gamelog to a file
*/
void savelog (gamelog * logptr)
{
    char * filename;
    FILE * fp;

    if (logptr == NULL)
    {
	gf1_alert (" no gamelog to be saved");
	return;
    }

    // ask for filename
    if ((filename = file_chooser ("save board", "*.log", "./gamelog.log")) 
	== NULL)
    {
	return;
    }

    if ((fp = fopen (filename, "w")) == NULL)
    {
	gf1_alert (" ERROR: Can't create the file");
	return;
    }

    logtofile (logptr, fp);

    fclose (fp);

    return;
}

extern "C" void checkwindowevents (void);

void checkwindowevents (void)
{
    Fl::check ();

    return;
}

