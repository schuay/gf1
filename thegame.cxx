/*
** $Id: thegame.cxx,v 1.35 2000/02/28 20:01:53 kurt Exp $
**
** functions and structures for a game of gipf
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
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>
#include <FL/fl_draw.H>
#include <FL/Fl.H>

#ifdef MSWIN
#include "gettimeofday.h"
#endif

#include "thegame.h"
#include "gipf_ui.H"
#include "board.h"
#include "callbacks.h"
#include "player_info.h"
#include "drawgif.h"
#include "xmlite.h"

char gametype[4][15] = {"basic", "standard", "tournament", "other game"};
char playertype[2][9] = {"human", "computer"};

/*
** show the window for starting a new game
**
** if the user pressed OK, this will return a structure for starting
** a new game
**
** if you give a parameter different from NULL, then the window will be
** initialised from this game, otherwise, defaults are used
*/
gamestruct * show_new (gamestruct * oldgame)
{
    Fl_Window * neww;
    gamestruct * newgame;

    neww = make_newwindow ();

    // initialize values
    if (oldgame != NULL)
    {
	// gametype
	switch (oldgame->game_type)
	{
	case T_BASIC:
	    radio_basic->setonly();
	    break;
	case T_STANDARD:
	    radio_standard->setonly();
	    break;
	case T_TOURNAMENT:
	    radio_tournament->setonly();
	    break;
	}

	// player names
	whiteplayername->value (oldgame->player[0].name);
	blackplayername->value (oldgame->player[1].name);

	// player type
	if (oldgame->player[0].type)
	    radio_wcompu->setonly();
	else
	    radio_whuman->setonly();

	if (oldgame->player[1].type)
	    radio_bcompu->setonly();
	else
	    radio_bhuman->setonly();

	// timed game
	if (oldgame->player[0].fulltime == -1)
	{
	    toggle_timedgame->clear();
	}
	else
	{
	    toggle_timedgame->set();
	    whitetime->value (oldgame->player[0].fulltime / 60);
	    blacktime->value (oldgame->player[1].fulltime / 60);
	}
    }
    else
    {
	radio_tournament->setonly();
	toggle_timedgame->clear();
	radio_whuman->setonly();
	radio_bcompu->setonly();
    }

    // show or hide the time-counters
    if (toggle_timedgame->value())
    {
	whitetime->activate();
	blacktime->activate();
    }
    else
    {
	whitetime->deactivate();
	blacktime->deactivate();
    }

    neww->show();

    while (1)
    {
	Fl::wait();    

	Fl_Widget *x;
	while ((x = Fl::readqueue())) 
	{
	    if (x == toggle_timedgame)
	    {
		if (toggle_timedgame->value())
		{
		    whitetime->activate();
		    blacktime->activate();
		}
		else
		{
		    whitetime->deactivate();
		    blacktime->deactivate();
		}
	    }
	    else if (x == new_ok)
	    {
		newgame = (gamestruct *) malloc (sizeof (gamestruct));
		newgame->state = GAME_GO;
		newgame->movecounter = 0;

		if (radio_whuman->value()) 
		    newgame->player[0].type = 0;
		else
		    newgame->player[0].type = 1;
		if (radio_bhuman->value()) 
		    newgame->player[1].type = 0;
		else
		    newgame->player[1].type = 1;

		strcpy (newgame->player[0].name, whiteplayername->value());
		strcpy (newgame->player[1].name, blackplayername->value());
		
		if (toggle_timedgame->value())
		{
		    newgame->player[0].fulltime = 60 * (int)whitetime->value();
		    newgame->player[0].timer = 
			(float) newgame->player[0].fulltime;
		    newgame->player[1].fulltime = 60 * (int)blacktime->value();
		    newgame->player[1].timer = 
			(float) newgame->player[1].fulltime;
		}
		else
		{
		    newgame->player[0].fulltime = -1; 
		    newgame->player[0].timer = 0.0;
		    newgame->player[1].fulltime = -1; 
		    newgame->player[1].timer = 0.0;
		}

		if (radio_basic->value ()) { newgame->game_type = T_BASIC;}
		else if (radio_standard->value ()) 
		{ newgame->game_type = T_STANDARD;}
		else if (radio_tournament->value ()) 
		{ newgame->game_type = T_TOURNAMENT;}
		else { return (NULL);}

		newgame->boards[0] = b_new (newgame->game_type);
		newgame->boards[1] = NULL;
		newgame->boards[2] = NULL;
		
		// initialize players
		newgame->player[0].self = 
		    (* players[newgame->player[0].type].newfunc)
		    ('o', newgame->game_type);
		newgame->player[1].self = 
		    (* players[newgame->player[1].type].newfunc)
		    ('x', newgame->game_type);

		newgame->movelog = newlog (gametype[newgame->game_type], 
					   newgame->player[0].name, 
					   newgame->player[1].name);

		delete neww;

		setcurrentfilename ("\0");

		return (newgame);
	    }
	    else if (x == new_cancel)
	    {
		delete neww;
		return (NULL);
	    }
	}
    }

    return (NULL);
}


/*
** show the window for starting a game from a gameboard
**
** if the user pressed OK, this will return a structure for starting
** a the game
**
** the parameter is the board to start from
*/
gamestruct * show_start (board * startboard)
{
    Fl_Window * startw;
    gamestruct * newgame;
    board * newboard;

    if ((newboard = verify_board (startboard)) == NULL)
    {
	return (NULL);
    }

    startw = make_startwindow ();

    /* put default settings in window */
    radio_white->setonly ();
    radio_s_whuman->setonly ();
    radio_s_bcompu->setonly ();
    if (newboard->typewhite == 'g')
	toggle_s_whitegipf->set ();
    else
	toggle_s_whitegipf->deactivate ();
    if (newboard->typeblack == 'g')
	toggle_s_blackgipf->set ();
    else
	toggle_s_blackgipf->deactivate ();

    startw->show();

    while (1)
    {
	Fl::wait();    

	Fl_Widget *x;
	while ((x = Fl::readqueue())) 
	{
	    if (x == start_ok)
	    {
		newgame = (gamestruct *) malloc (sizeof (gamestruct));
		newgame->state = GAME_GO;
		newgame->movecounter = 0;

		if (radio_s_whuman->value() != 0) 
		    newgame->player[0].type = 0;
		else  
		    newgame->player[0].type = 1;

		if (radio_s_bhuman->value() != 0) 
		    newgame->player[1].type = 0;
		else  
		    newgame->player[1].type = 1;

		strcpy (newgame->player[0].name, s_whiteplayername->value());
		strcpy (newgame->player[1].name, s_blackplayername->value());
		
		newgame->player[0].fulltime = -1;
		newgame->player[0].timer = 0.0;
		newgame->player[1].fulltime = -1;
		newgame->player[1].timer = 0.0;

		newgame->game_type = T_OTHER;

		if ((newboard->typewhite == 'g') && 
		    (toggle_s_whitegipf->value () == 0))
		    newboard->typewhite = 'n';
		if ((newboard->typeblack == 'g') && 
		    (toggle_s_blackgipf->value () == 0))
		    newboard->typeblack = 'n';

		if (radio_white->value () != 0)
		    newboard->nextpiece = 'o';
		else
		    newboard->nextpiece = 'x';

		newgame->boards[0] = newboard;
		newgame->boards[1] = NULL;
		newgame->boards[2] = NULL;
		
		// initialize players
		newgame->player[0].self = 
		    (* players[newgame->player[0].type].newfunc)
		    ('o', newgame->game_type);
		newgame->player[1].self = 
		    (* players[newgame->player[1].type].newfunc)
		    ('x', newgame->game_type);

		newgame->movelog = newlog (gametype[newgame->game_type], 
					   newgame->player[0].name, 
					   newgame->player[1].name);

		delete (startw);

		setcurrentfilename ("\0");

		return (newgame);
	    }
	    else if (x == start_cancel)
	    {
		b_del (newboard);
		delete (startw);
		return (NULL);
	    }
	}
    }

    return (NULL);
}


/*
** show the window for calculating one move starting from a gameboard
**
** if the user pressed OK, this will return a structure for calculating
** the one move
**
** the parameter is the board to start from
*/
gamestruct * show_onemove (board * startboard, configvalues * conf)
{
    Fl_Window * onemovew;
    gamestruct * newgame;
    board * newboard;
    int newlevel,
	oldlevel;

    if ((newboard = verify_board (startboard)) == NULL)
    {
	return (NULL);
    }

    onemovew = make_onemovewindow ();

    radio_c_white->setonly ();
    c_level3->setonly ();

    onemovew->show();

    while (1)
    {
	Fl::wait();    

	Fl_Widget *x;
	while ((x = Fl::readqueue())) 
	{
	    if (x == one_ok)
	    {
		newgame = (gamestruct *) malloc (sizeof (gamestruct));
		newgame->state = GAME_GO;
		newgame->movecounter = 0;

		if (radio_c_white->value() != 0)
		{
		    newgame->player[0].type = 1;
		    newgame->player[1].type = 0;
		    strcpy (newgame->player[0].name, "Computer");
		    strcpy (newgame->player[1].name, "None");
		    newboard->nextpiece = 'o';
		}
		else  
		{
		    newgame->player[0].type = 0;
		    newgame->player[1].type = 1;
		    strcpy (newgame->player[1].name, "Computer");
		    strcpy (newgame->player[0].name, "None");
		    newboard->nextpiece = 'x';
		}

		newgame->player[0].fulltime = -1;
		newgame->player[0].timer = 0.0;
		newgame->player[1].fulltime = -1;
		newgame->player[1].timer = 0.0;

		newgame->game_type = T_OTHER;

		newgame->boards[0] = newboard;
		newgame->boards[1] = NULL;
		newgame->boards[2] = NULL;
		
		newgame->config = conf;

		// initialize players
		newgame->player[0].self = 
		    (* players[newgame->player[0].type].newfunc)
		    ('o', newgame->game_type);
		newgame->player[1].self = 
		    (* players[newgame->player[1].type].newfunc)
		    ('x', newgame->game_type);

		newgame->movelog = newlog (gametype[newgame->game_type], 
					   newgame->player[0].name, 
					   newgame->player[1].name);

		if      ((c_level1->value ()) != 0) { newlevel = 1; }
		else if ((c_level2->value ()) != 0) { newlevel = 2; }
		else if ((c_level3->value ()) != 0) { newlevel = 3; }
		else if ((c_level4->value ()) != 0) { newlevel = 4; }
		else if ((c_level5->value ()) != 0) { newlevel = 5; }
		else if ((c_level6->value ()) != 0) { newlevel = 6; }
		else if ((c_level7->value ()) != 0) { newlevel = 7; }
		else if ((c_level8->value ()) != 0) { newlevel = 8; }

		oldlevel = conf->searchdepth;
		if (newlevel != oldlevel)
		{
		    conf->searchdepth = newlevel;
		    writeconfig (conf);
		}

		delete (onemovew);

		setcurrentfilename ("\0");

		/* execute the move here */
		gameboard->setboard (newgame->boards[0]);

		setupmove (newgame);

		newgame->state = GAME_STOP;
		changeinterface (INTERFACE_PLAY, newgame);

		if (newlevel != oldlevel)
		{
		    conf->searchdepth = oldlevel;
		    writeconfig (conf);
		}

		return (newgame);
	    }
	    else if (x == one_cancel)
	    {
		b_del (newboard);
		delete (onemovew);
		return (NULL);
	    }
	}
    }

    return (NULL);
}


/* 
** check if a gameboard can be used to start a game from
**
** returns a copy of sboard that is correctly setup
** or NULL if there is a problem
*/
board * verify_board (board * sboard)
{
    board * nboard;
    rem_row * row_ptr;
    int i, j,
	white = 0,
	black = 0;
    position * pos;

    /* check if one of the players has no pieces left */
    if (b_colour (sboard, 'o') == 0)
    {
	gf1_alert ("ERROR: no game possible from this boardsituation, player white has no pieces left.");
	return (NULL);
    }
    if (b_colour (sboard, 'x') == 0)
    {
	gf1_alert ("ERROR: no game possible from this boardsituation, player black has no pieces left.");
	return (NULL);
    }

    /* count pieces on the board */
    pos = new_position ();
    for (i = 1; i < 8; i++)
	for (j = 2; j <= b_colsize (i); j++)
	{
	    posp_col (pos) = i;
	    posp_row (pos) = j;
	    switch (b_ppiece (sboard, pos))
	    {
	    case 'o':
 		white += 1;
		break;
	    case 'O':
 		white += 2;
		break;
	    case 'x':
 		black += 1;
		break;
	    case 'X':
 		black += 2;
		break;
	    }
	}
	   
    /* check if the board is empty */
    if ((white == 0) && (black == 0))
    {
	gf1_alert ("ERROR: no game possible from this boardsituation, the gameboard is empty.");
	return (NULL);
    }

    /* check for four-in-a-row */
    for (i = 0; i < 21; i++)
    {
	if ((row_ptr = b_rowoffour (sboard, i)) != NULL)
	{
	    del_rem_row ((void *) row_ptr);
	    gf1_alert ("ERROR: no game possible from this boardsituation, four-in-a-row found.");
	    return (NULL);
	}
    }

    /* check if this is a basic game */
    if ((b_colour_gipf (sboard, 'o') == 0) &&
	(b_colour_gipf (sboard, 'x') == 0))
    {
	nboard = b_copy (sboard);
	nboard->gipfwhite = -1;
	nboard->gipfblack = -1;
	nboard->typewhite = 'n';
	nboard->typeblack = 'n';

	/* turn this into a real basic game (if possible) */
	if ((b_colour (nboard, 'o') > 3) && (b_colour (sboard, 'x') > 3))
	{
	    nboard->white -= 3;
	    nboard->black -= 3;
	}

	return (nboard);
    }

    /* tournament or standard game, for me it's always tournament */

    /* 
    ** something is wrong when a player has normal pieces, but
    ** no gipf-pieces
    */
    /* check if one of the player has single pieces, but no gipf-pieces */
    if ((b_colour_gipf (sboard, 'o') == 0) && (b_colour (sboard, 'o') < 18))
    {
	gf1_alert ("ERROR: no game possible from this boardsituation, player white has no gipf-pieces.");
	return (0);
    }
    if ((b_colour_gipf (sboard, 'x') == 0) && (b_colour (sboard, 'x') < 18))
    {
	gf1_alert ("ERROR: no game possible from this boardsituation, player black has no gipf-pieces.");
	return (0);
    }

    nboard = b_copy (sboard);
    nboard->typewhite = 'n';
    nboard->typeblack = 'n';
    if ((18 - b_colour_gipf (sboard, 'o') * 2) == b_colour (sboard, 'o'))
    {   /* only gipf-pieces, allow player to add more */
	nboard->typewhite = 'g';
    }
    if ((18 - b_colour_gipf (sboard, 'x') * 2) == b_colour (sboard, 'x'))
    {   /* only gipf-pieces, allow player to add more */
	nboard->typeblack = 'g';
    }
/*
** I don't check here if the players have 18 pieces
** which means that it's not completely correct
*/

    return (nboard);
}


void delete_game (gamestruct * gameptr)
{
    int i;

    if (gameptr == NULL)
	return;

    /* ask the player to cleanup its data */
    if (gameptr->player[0].self != NULL)
    {
	(* players[gameptr->player[0].type].endfunc) (gameptr->player[0].self);
    }
    if (gameptr->player[1].self != NULL)
    {
	(* players[gameptr->player[1].type].endfunc) (gameptr->player[1].self);
    }

    for (i = 0; i < 3; i++)
    {
	b_del (gameptr->boards[i]);
    }

    deletelog (gameptr->movelog);
    return;
}


/* 
** REMARK: this is the old version
**
** load a game from a file
**
** return NULL if nothing loaded
*/
gamestruct * oldloadgame (char * filename)
{
    char buffer[100],
	tempstr[100];
    FILE * fp;
    int i,
	tempnr;
    gamestruct * newgame;

    if ((fp = fopen (filename, "r")) == NULL)
    {
	gf1_alert (" ERROR: Can't open the file");
	return (NULL);
    }

    newgame = (gamestruct *) malloc (sizeof (gamestruct));
    newgame->state = GAME_GO;

    fgets (buffer, 100, fp);
    if (strncmp (buffer, "# saved game for gf1", 20) != 0)
    {
	gf1_alert (" ERROR: wrong inputfile format");
	free (newgame);
	return (NULL);
    }

    fgets (buffer, 100, fp);
    if (sscanf (buffer, "# gametype: %d", &(newgame->game_type)) != 1)
    {
	gf1_alert (" ERROR: wrong inputfile format");
	free (newgame);
	return (NULL);
    }

    for (i = 0; i < 2; i++)
    {
	fgets (buffer, 100, fp);
	if (sscanf (buffer, "# player%d type: %d", 
		    &tempnr, &(newgame->player[i].type)) != 2)
	{
	    gf1_alert (" ERROR: wrong inputfile format");
	    free (newgame);
	    return (NULL);
	}

	fgets (buffer, 100, fp);
	sprintf (tempstr, "# player%d name: ", i);
	if (strncmp (buffer, tempstr, 16) != 0)
	{
	    gf1_alert (" ERROR: wrong inputfile format");
	    free (newgame);
	    return (NULL);
	}
	buffer[strlen(buffer) - 1] = '\0';
	strcpy (newgame->player[i].name, buffer+16);

	fgets (buffer, 100, fp);
	if (sscanf (buffer, "# player%d fulltime: %d", 
		    &tempnr, &(newgame->player[i].fulltime)) != 2)
	{
	    gf1_alert (" ERROR: wrong inputfile format");
	    free (newgame);
	    return (NULL);
	}

	fgets (buffer, 100, fp);
	if (sscanf (buffer, "# player%d timer: %f", 
		    &tempnr, &(newgame->player[i].timer)) != 2)
	{
	    gf1_alert (" ERROR: wrong inputfile format");
	    free (newgame);
	    return (NULL);
	}

	newgame->player[i].self = NULL;
    }

    fgets (buffer, 100, fp);
    if ((newgame->boards[0] = b_from_file (fp)) == NULL)
    {
	gf1_alert (" ERROR: wrong inputfile format");
	free (newgame);
	return (NULL);
    }
    newgame->boards[1] = NULL;
    newgame->boards[2] = NULL;

    fgets (buffer, 100, fp);
    if ((newgame->movelog = logfromfile (fp)) == NULL)
    {
	gf1_alert (" ERROR: wrong inputfile format");
	free (newgame);
	return (NULL);
    }
    newgame->movecounter = loglength (newgame->movelog);

    fclose (fp);

    // reading was succesfull, init the players
    newgame->player[0].self = 
	(* players[newgame->player[0].type].newfunc) ('o', newgame->game_type);
    newgame->player[1].self = 
	(* players[newgame->player[1].type].newfunc) ('x', newgame->game_type);

    return (newgame);
}


/* 
** load a game from a file
**
** return NULL if nothing loaded
*/
gamestruct * loadgame (void)
{
    char * filename,
	buffer[100],
	tempstr[100];
    FILE * fp;
    int i;
    gamestruct * newgame;
    xmlite_entity * root, *x1;
    xmlite_parser * theparser;

    // ask for filename
    if ((filename = file_chooser ("load game", "*.gf1", NULL)) == NULL)
    {
	return (NULL);
    }

    if ((fp = fopen (filename, "r")) == NULL)
    {
	gf1_alert (" ERROR: Can't open the file");
	return (NULL);
    }

    fgets (buffer, 100, fp);
    fclose (fp);
    if (strncmp (buffer, "# saved game for gf1", 20) == 0)
    {   // old saveformat
	return (oldloadgame (filename));
    }

    theparser = new xmlite_parser (filename);
    root = theparser->parse ();
    delete (theparser);
    if (root == NULL)
    {
	delete (root);
	gf1_alert (" ERROR: Can't parse savefile");
	return (NULL);
    }

    if (root->getname() != "gipfgame")
    {
	delete (root);
	gf1_alert (" ERROR: Not a savefile from gf1");
	return (NULL);
    }

    if (atoi (root->getattribute("formatversion").c_str()) > 1)
    {
	delete (root);
	gf1_alert (" ERROR: Don't know how to handle this fileversion");
	return (NULL);
    }

    newgame = (gamestruct *) malloc (sizeof (gamestruct));
    newgame->state = GAME_GO;

    // gametype
    strcpy (tempstr, root->getattribute("type").c_str());
    newgame->game_type = 3; // put some default value in
    for (i = 0; i < 4; i++)
    {
	if (strcmp (tempstr, gametype[i]) == 0)
	{
	    newgame->game_type = i;
	    break;
	}
    }

    // players
    x1 = root->getcontentbyname ("whiteplayer");
    strcpy (newgame->player[0].name, x1->getvalue().c_str());
    newgame->player[0].type = x1->getattribute ("type") == "human" ? 0 : 1;
    newgame->player[0].fulltime = atoi (x1->getattribute ("fulltime").c_str());
    newgame->player[0].timer = atof (x1->getattribute ("timeleft").c_str());
    newgame->player[0].self = NULL;
    x1 = root->getcontentbyname ("blackplayer");
    strcpy (newgame->player[1].name, x1->getvalue().c_str());
    newgame->player[1].type = x1->getattribute ("type") == "human" ? 0 : 1;
    newgame->player[1].fulltime = atoi (x1->getattribute ("fulltime").c_str());
    newgame->player[1].timer = atof (x1->getattribute ("timeleft").c_str());
    newgame->player[1].self = NULL;

    // the boardsituation
    newgame->boards[0] = b_from_xml (root->getcontentbyname ("board"));
    newgame->boards[1] = NULL;
    newgame->boards[2] = NULL;

    // the log of moves
    newgame->movelog = logfromxml (root->getcontentbyname ("gamelog"));
    newgame->movecounter = loglength (newgame->movelog);

    // reading was succesfull, init the players
    newgame->player[0].self = 
	(* players[newgame->player[0].type].newfunc) ('o', newgame->game_type);
    newgame->player[1].self = 
	(* players[newgame->player[1].type].newfunc) ('x', newgame->game_type);

    delete (root);

    setcurrentfilename (filename);

    return (newgame);
}


/* 
** save a game to a file
*/
void savegame (gamestruct * gameptr)
{
    char * filename;
    xmlite_entity *x1, *x2;
    char tempstr[100],
	* cfile;

    if (gameptr == NULL)
    {
	gf1_alert (" no current game to be saved");
	return;
    }

    // ask for filename
    cfile = getcurrentfilename ();
    if (cfile[0] == '\0')
	strcpy (tempstr, "./savegame.gf1");
    else
	strcpy (tempstr, cfile);
    if ((filename = file_chooser ("save game", "*.gf1", tempstr)) 
	== NULL)
    {
	return;
    }

    // gipfgame
    x1 = new xmlite_entity ("gipfgame");
    x1->addattribute ("type", gametype[gameptr->game_type]);
    x1->addattribute ("formatversion", "1");

    // white player
    x2 = new xmlite_entity ("whiteplayer");
    sprintf (tempstr, "%f", gameptr->player[0].timer);
    x2->addattribute ("timeleft", tempstr);
    sprintf (tempstr, "%d", gameptr->player[0].fulltime);
    x2->addattribute ("fulltime", tempstr);
    x2->addattribute ("type", playertype[gameptr->player[0].type]);
    x2->setvalue (gameptr->player[0].name);
    x1->addcontent (x2);

    // black player
    x2 = new xmlite_entity ("blackplayer");
    sprintf (tempstr, "%f", gameptr->player[1].timer);
    x2->addattribute ("timeleft", tempstr);
    sprintf (tempstr, "%d", gameptr->player[1].fulltime);
    x2->addattribute ("fulltime", tempstr);
    x2->addattribute ("type", playertype[gameptr->player[1].type]);
    x2->setvalue (gameptr->player[1].name);
    x1->addcontent (x2);

    // last gameboard
    x2 = b_to_xml (gameptr->boards[0]);
    x1->addcontent (x2);

    // gamelog
    x2 = logtoxml (gameptr->movelog);
    x1->addcontent (x2);

    x1->writetofile (filename);
    delete (x1);

    setcurrentfilename (filename);

    return;
}


void setupmove (gamestruct * thegame)
{
    char tempstr[100],
	nextpiece,
	winner = ' ';

    if ((thegame == NULL) || (thegame->state == GAME_STOP))
    {
	changeinterface (INTERFACE_PLAY, thegame);
	return;
    }

    while (1)
    {
	Fl::check ();

	if (thegame->player[0].fulltime != -1)
	{
	    if (thegame->player[0].timer <= 0.0)
	    {
		winner = 'x';
		break;
	    }
	    if (thegame->player[1].timer <= 0.0)
	    {
		winner = 'o';
		break;
	    }
	}

	if (interrupt_computer)
	{
	    stoptimer (thegame);
	    interrupt_computer = 0;
	    thegame->state = GAME_STOP;
	    break;
	}
	
	changeinterface (INTERFACE_PLAY, thegame);

	if (b_status (thegame->boards[0]) == S_NORMAL)
	{
	    nextpiece = b_next_piece (thegame->boards[0]);
	    /* check if gipf-pieces left */
	    if (thegame->game_type != T_BASIC)
	    {
		if (((b_move_counter (thegame->boards[0]) > 1) || 
		     (thegame->game_type == T_OTHER) || (nextpiece == 'x')) && 
		    (b_white_gipf (thegame->boards[0]) == 0))
		{
		    winner = 'x';
		    break;
		}
		else if ((b_move_counter (thegame->boards[0]) > 1) && 
			 (b_black_gipf (thegame->boards[0]) == 0))
		{
		    winner = 'o';
		    break;
		}
	    }
	    if (b_colour (thegame->boards[0], nextpiece) == 0)
	    {   /* game finished */
		winner = b_opponent (nextpiece);
		break;
	    }

	    if (thegame->player[pnr(nextpiece)].type == 0)
	    {   /* human player */
		gameboard->setto (NULL);
		gameboard->setfrom (NULL);
		if (b_colour_type (thegame->boards[0], nextpiece) == 'g')
		    gameboard->setgipfpossible (1);
		else
		    gameboard->setgipfpossible (0);

		starttimer (thegame, nextpiece);
		break;
	    }
	    else
	    {   /* computer player */
		fl_cursor (FL_CURSOR_WAIT);
		Fl::check ();
		
		computermove (thegame);

		fl_cursor (FL_CURSOR_DEFAULT);
	    }
	}
	else if (b_status (thegame->boards[0]) == S_REMOVEGIPF)
	{
	    gipf_questions (thegame);
	}
	else if (b_status (thegame->boards[0]) == S_REMOVEROW)
	{
	    row_questions (thegame);
	}
    }

    if (winner != ' ')
    {
	sprintf (tempstr, "Player %s, you have won after %d moves !", 
		 thegame->player[pnr(winner)].name, 
		 b_move_counter (thegame->boards[0]));
	gf1_alert (tempstr);
	thegame->state = GAME_STOP;
    }

    changeinterface (INTERFACE_PLAY, thegame);
    Fl::check ();
    return;
}


void computermove (gamestruct * thegame)
{
    char nextpiece,
	piecetype,
	from[3] = "  ",
	to[3] ="  ",
	piece;
    int playertype;
    board * temp_board,
	* new_board = NULL;
    Fl_Window * thinkwindow;
    
    temp_board = b_copy (thegame->boards[0]);
    nextpiece = b_next_piece (thegame->boards[0]);
    piecetype = b_colour_type (thegame->boards[0], nextpiece);
    playertype = thegame->player[pnr(nextpiece)].type;

    starttimer (thegame, nextpiece);

    thinkwindow = create_thinkwindow ();

    (* players[playertype].movefunc)
	(temp_board, thegame->player[pnr(nextpiece)].self, 
	 thegame->player[pnr(nextpiece)].timer, 
	 &piecetype, from, to);

    Fl::check ();
    delete thinkwindow;

    stoptimer (thegame);
    if (interrupt_computer)
	return;

    if ((b_colour_type (thegame->boards[0], nextpiece) != 'g') &&
	(piecetype == 'g'))
    {
	piecetype = 'n';
    }

    piece = piecetype == 'g' ? b_otherpiece (nextpiece) : nextpiece;

    b_setlog (thegame->boards[0], thegame->movelog);
    new_board = b_move (thegame->boards[0], from, to, piece);
    b_nolog (thegame->boards[0]);
    b_nolog (new_board);

    b_del (temp_board);
    if (new_board == NULL)
    {
	gf1_alert ("the computer-player executed an invalid move !!");

//	b_del (thegame->boards[0]);
//	(* players[thegame->player[0].type].endfunc)(thegame->player[0].self);
//	(* players[thegame->player[1].type].endfunc)(thegame->player[1].self);
//	thegame->boards[0] = NULL;
    }
    else
    {
	b_del (thegame->boards[2]);
	thegame->boards[2] = thegame->boards[1];
	thegame->boards[1] = thegame->boards[0];
	thegame->boards[0] = new_board;

	analysemove (thegame, playertype);

	gameboard->setboard (thegame->boards[0]);
    }

    return;
}


int humanmove (gamestruct * thegame)
{
    char nextpiece,
	piece;
    int playertype;
    board * new_board = NULL;
    char * from,
	* to,
	piecetype;

    if (thegame->state == GAME_STOP)
	return (0);

    from = postostr (gameboard->getfrom ());
    to = postostr (gameboard->getto ());
    if (gameboard->getptype ())
	piecetype = 'g';
    else
	piecetype = 'n';

    nextpiece = b_next_piece (thegame->boards[0]);
    playertype = thegame->player[pnr(nextpiece)].type;

    if ((b_colour_type (thegame->boards[0], nextpiece) != 'g') &&
	(piecetype == 'g'))
    {
	piecetype = 'n';
    }

    piece = piecetype == 'g' ? b_otherpiece (nextpiece) : nextpiece;

    b_setlog (thegame->boards[0], thegame->movelog);
    new_board = b_move (thegame->boards[0], from, to, piece);
    b_nolog (thegame->boards[0]);
    if (new_board != NULL)
	b_nolog (new_board);

    free (from);
    free (to);

    if (new_board == NULL)
    {
	return (-1);
    }

    stoptimer (thegame);

    b_del (thegame->boards[2]);
    thegame->boards[2] = thegame->boards[1];
    thegame->boards[1] = thegame->boards[0];
    thegame->boards[0] = new_board;

    analysemove (thegame, 0);

    gameboard->setto (NULL);
    gameboard->setfrom (NULL);
    gameboard->setboard (thegame->boards[0]);

    return (0);
}


void row_questions (gamestruct * thegame)
{
    board * temp_board,
	* new_board;
    int counter;
    rem_row * row;
    listheader * rowlist;
    char owner,
	response,
	* start,
	* end,
	tempstr1[100],
	tempstr2[100];
    int answer;

    temp_board = b_copy (thegame->boards[0]);
    counter = 1;
    rowlist = b_row_extra (thegame->boards[0]);
    while ((row = (rem_row *) llitembynr (rowlist, counter)) != NULL)
    {
	counter++;
	start = postostr (row->startpos);
	end = postostr (row->endpos);
	owner = row_owner (row);
		
	starttimer (thegame, owner);
	if (thegame->player[pnr(owner)].type == 0)
	{   /* human */
	    gameboard->setrow (row->piecelist);
	    gameboard->setboard (thegame->boards[0]);

	    sprintf (tempstr1, "Player %s, do you want to remove",
		     thegame->player[pnr(owner)].name);
	    sprintf (tempstr2, "the row from %s to %s", start, end);

	    answer = 2;
	    while (answer == 2)
	    {
		answer = gf1_questionsave (tempstr1, tempstr2);

		if (answer == 2)
		    savegame (thegame);
		else if (answer == 1)
		    response = 'y';
		else
		    response = 'n';
	    }

	    gameboard->setrow (NULL);
	}
	else
	{   /* computer */
	    Fl_Window * thinkwindow;
    
	    thinkwindow = create_thinkwindow ();

	    response = 
		(* players[thegame->player[pnr(owner)].type].rowfunc)
		(temp_board, thegame->player[pnr(owner)].self, 
		 thegame->player[pnr(owner)].timer, start, end);

	    delete thinkwindow;
	}
	stoptimer (thegame);
	if (interrupt_computer)
	    return;
		
	free (start);
	free (end);

	if (response == 'y')
	{
	    b_setlog (thegame->boards[0], thegame->movelog);
	    new_board = b_remove_row (thegame->boards[0], counter-1);
	    b_nolog (new_board);
	    b_del (thegame->boards[0]);
	    thegame->boards[0] = new_board;

	    analysemove (thegame, thegame->player[pnr(owner)].type);

	    gameboard->setboard (thegame->boards[0]);

	    break;
	}
    }
    b_del (temp_board);

    return;
}


void gipf_questions (gamestruct * thegame)
{
    int counter;
    rem_gipf * gipf;
    board * nboard,
	* new_board,
	* temp_board;
    listheader * gipflist;
    char owner,
	* strpos,
	response;
    char tempstr1[100],
	tempstr2[100];
    int answer;

    counter = 1;
    gipflist = b_gipf_extra (thegame->boards[0]);
    new_board = thegame->boards[0];

    while ((gipf = (rem_gipf *) llitembynr (gipflist, counter)) != NULL)
    {
	counter++;
	strpos = b_gipf_position (gipf);
	owner = b_gipf_owner (gipf);

	starttimer (thegame, owner);
	if (thegame->player[pnr(owner)].type == 0)
	{   /* human */
	    gameboard->setto (gipf->pos);
	    gameboard->setboard (new_board);

	    sprintf (tempstr1, "Player %s, do you want to remove",
		     thegame->player[pnr(owner)].name);
	    sprintf (tempstr2, "the GIPF at %s", strpos);

	    answer = 2;
	    while (answer == 2)
	    {
		/*
		** only allow saving of the game at the first question
		** for removing a gipf.
		** things get fishy if you would save from here after a gipf
		** has been removed already.
		** (the problem doesn't occur immediatly, but when you load
		**  the game again)
		*/
		if (counter == 2)
		    answer = gf1_questionsave (tempstr1, tempstr2);
		else
		    answer = gf1_question (tempstr1, tempstr2);
		    

		if (answer == 2)
		    savegame (thegame);
		else if (answer == 1)
		    response = 'y';
		else
		    response = 'n';
	    }

	    gameboard->setto (NULL);
	}
	else
	{   /* computer */
	    Fl_Window * thinkwindow;
    
	    thinkwindow = create_thinkwindow ();

	    temp_board = b_copy (new_board);
	    response = (* players[thegame->player[pnr(owner)].type].gipffunc)
		(temp_board, thegame->player[pnr(owner)].self, 
		 thegame->player[pnr(owner)].timer, strpos);
	    b_del (temp_board);

	    delete thinkwindow;
	}
	stoptimer (thegame);
	if (interrupt_computer)
	    return;

	if ((response == 'y') || (response == 'Y'))
	{
	    b_setlog (new_board, thegame->movelog);
	    nboard = b_remove_gipf (new_board, gipf);
	    b_nolog (new_board);
	    b_nolog (nboard);

	    if (new_board != thegame->boards[0])
		b_del (new_board);

	    new_board = nboard;

	    analysemove (thegame, thegame->player[pnr(owner)].type);

	    gameboard->setboard (new_board);
	}
	free (strpos);
    }

    b_setlog (new_board, thegame->movelog);
    nboard = b_checkfour (new_board);
    b_nolog (new_board);
    b_nolog (nboard);
    if (new_board != thegame->boards[0])
	b_del (new_board);
   
    b_del (thegame->boards[0]);
    thegame->boards[0] = nboard;
    
    analysemove (thegame, 1);

    gameboard->setboard (thegame->boards[0]);

    return;
}


//time_t basetime = 0;
struct timeval basetime;
char timercolor;

void starttimer (gamestruct * thegame, char color)
{
    struct timezone tz;

    if (thegame->player[0].fulltime == -1)
    {
	basetime.tv_sec = 0;
	return;
    }

//    basetime = time (NULL);
    gettimeofday (&basetime, &tz);
    timercolor = color;

    Fl::add_timeout (1.0, updatetimer, (void *) thegame);

    return;
}


void stoptimer (gamestruct * thegame)
{
//    time_t newtime;
    struct timezone tz;
    struct timeval tv;
    float timedif;
    char tempstr[20];

    if ((thegame == NULL) || (basetime.tv_sec == 0))
	return;

    Fl::remove_timeout (updatetimer, (void *) thegame);

//    newtime = time (NULL);
    gettimeofday (&tv, &tz);
    timedif = (tv.tv_sec - basetime.tv_sec) +
	(float) (tv.tv_usec - basetime.tv_usec)/1000000;

    if (timercolor == 'o')
    {
	if (interrupt_computer == 0)
	    thegame->player[0].timer -= timedif;
	timertostr (thegame->player[0].timer, tempstr);
	whitetimer->value (tempstr);
    }
    else
    {
	if (interrupt_computer == 0)
	    thegame->player[1].timer -= timedif;
	timertostr (thegame->player[1].timer, tempstr);
	blacktimer->value (tempstr);
    }

    basetime.tv_sec = 0;
    return;
}


void updatetimer (void * data)
{
    gamestruct * thegame = (gamestruct *) data;
//    time_t newtime;
    struct timezone tz;
    struct timeval tv;
    float timedif;
    float newtimer;
    char tempstr[20];

//    newtime = time (NULL);
    gettimeofday (&tv, &tz);
    timedif = (tv.tv_sec - basetime.tv_sec) +
	(float) (tv.tv_usec - basetime.tv_usec)/1000000;

    if (timercolor == 'o')
    {
	newtimer = thegame->player[0].timer - timedif;
	timertostr (newtimer, tempstr);
	whitetimer->value (tempstr);
    }
    else
    {
	newtimer = thegame->player[1].timer - timedif;
	timertostr (newtimer, tempstr);
	blacktimer->value (tempstr);
    }

    if (newtimer > 0.0)
	Fl::add_timeout (1.0, updatetimer, data);
    else
    {   /* timer ran out, show if this is a human move */
	if (timercolor == 'o')
	{
	    if (thegame->player[0].type == 0)
	    {
		thegame->player[0].timer = newtimer;
		
		setupmove (thegame);
	    }
	}
	else if (thegame->player[1].type == 0)
	{
	    thegame->player[1].timer = newtimer;

	    setupmove (thegame);
	}
    }

    return;
}


/*
** analyse the entries added to the gamelog
** since the last time we ran this function
** show what needs to be shown to the user
*/
void analysemove (gamestruct * thegame, int playertype)
{
    int counter = thegame->movecounter + 1;
    logitem * item;

    while ((item = logitemnr (thegame->movelog, counter)) != NULL)
    {
	switch (logitem_type (item))
	{
	case LOGMOVE:
	    showmove (thegame, playertype, item);
	    break;
	case LOGREMGIPF:
	    showremgipf (thegame, playertype, item);
	    break;
	case LOGREMROW:
	    showremrow (thegame, playertype, item);
	    break;
	}

	counter++;
	playertype = 1; /* necessary in case a row is removed automatically */
    }
    thegame->movecounter = counter - 1;

    /* show the correct board */
    gameboard->setboard (thegame->boards[0]);

    return;
}


void showmove (gamestruct * thegame, int playertype, logitem * item)
{
    char start[3] = "  ",
	end[3] = "  ";
    position * startpos,
	* endpos,
	* topos;
    int dir,
	counter;
    listheader * plist;
    struct movepiece {
	char piece;
	position * from;
	position * to;
    } * mpiece;
    board * oldboard,
	* newboard,
	* nboard;

    strncpy (start, logitem_start (item), 2);
    strncpy (end, logitem_end (item), 2);
    startpos = strtopos (start);
    endpos = strtopos (end);
    topos = new_position ();

    /*
    **from the start and the end of the move we can find the to-position 
    ** and the direction of the move
    */
    if (posp_col (startpos) == posp_col (endpos))
    {
	posp_col (topos) = posp_col (startpos);
	if (posp_row (startpos) > posp_row (endpos))
	{
	    posp_row (topos) = posp_row (startpos) - 1;
	    dir = 2;
	}
	else
	{
	    posp_row (topos) = posp_row (startpos) + 1;
	    dir = 3;
	}
    }
    else if (posp_col (startpos) < posp_col (endpos))
    {
	posp_col (topos) = posp_col (startpos) + 1;
	if (posp_row (startpos) >= posp_row (endpos))
	{
	    posp_row (topos) = posp_row (startpos);
	    dir = 4;
	}
	else
	{
	    posp_row (topos) = posp_row (startpos) + 1;
	    dir = 5;
	}
    }
    else
    {
	posp_col (topos) = posp_col (startpos) - 1;
	if (posp_row (startpos) >= posp_row (endpos))
	{
	    posp_row (topos) = posp_row (startpos);
	    dir = 0;
	}
	else
	{
	    posp_row (topos) = posp_row (startpos) + 1;
	    dir = 1;
	}
    }

    /*
    ** reasons to wait before showing the move
    **    - waitcomputer > 0
    **    - move by the computer
    */
    if ((playertype != 0) &&
	(thegame->config->waitcomputer > 0))
    {   /* computerplayer */
	if ((logitem_player (item) == 'O') ||
	    (logitem_player (item) == 'X'))
	{
	    gameboard->setgipfpossible (1);
	}
	else
	{
	    gameboard->setgipfpossible (0);
	}
	gameboard->setfrom (startpos);
	gameboard->setto (topos);

 	/* the first Fl::wait is necessary to reset the start of the timer */
	Fl::wait (0);
	for (float time = thegame->config->waitcomputer * 1.0; time > 0; ) 
	    time = Fl::wait(time);
#if 0
	Fl::check ();
#ifdef MSWIN
	sleep (thegame->config->waitcomputer * 1000);
#else
	sleep (thegame->config->waitcomputer);
#endif
#endif
    }
    gameboard->setto (NULL);
    gameboard->setfrom (NULL);


    /*
    ** make a list of all the pieces that move 
    ** + from and to position
    */
    plist = (listheader *) malloc (sizeof (listheader));
    newlist (plist);
    mpiece = (struct movepiece *) malloc (sizeof (struct movepiece));
    mpiece->piece = logitem_player (item);
    mpiece->from = (position *) copy_position ((void *) startpos);
    mpiece->to = (position *) copy_position ((void *) topos);
    pushll (plist, mpiece);
    while ((posp_col (topos) != posp_col (endpos)) ||
	   (posp_row (topos) != posp_row (endpos)))	
    {
	posp_col (startpos) = posp_col (topos);
	posp_row (startpos) = posp_row (topos);
	posp_col (topos) = 
	    b_buren[posp_col (startpos)][posp_row (startpos)][dir][0];
	posp_row (topos) = 
	    b_buren[posp_col (startpos)][posp_row (startpos)][dir][1];

	mpiece = (struct movepiece *) malloc (sizeof (struct movepiece));
	mpiece->piece = b_ppiece (gameboard->getboard (), startpos);
	mpiece->from = (position *) copy_position ((void *) startpos);
	mpiece->to = (position *) copy_position ((void *) topos);
	pushll (plist, mpiece);
    }

    /*
    ** reason to animate the move
    **    - animate > 0
    */
    if (thegame->config->animate > 0)
    {   /* animate */
	int steps = 10;
	switch (thegame->config->animate)
	{
	case 1: /* slow */
	    steps = 20;
	    break;
	case 2: /* medium */
	    steps = 13;
	    break;
	case 3: /* fast */
	    steps = 8;
	    break;
	}
	gameboard->initanim (steps, plist);

//	Fl::wait (0);
	for (int i = 0; i <= steps; i++)
	{
	    gameboard->animstep (i);
#ifdef MSWIN
	    /* this timing looks a little better on ms windows */
	    for (float time = 0.02; time > 0; ) 
		time = Fl::wait(time);
#else
	    for (float time = 0.03; time > 0; ) 
		time = Fl::wait(time);
#endif
	}
    }

    /* update board */
    nboard = oldboard = gameboard->getboard ();
    counter = 1;
    while ((mpiece = (struct movepiece *) llitembynr (plist, counter)) 
	   != NULL)
    {
	counter++;
	newboard = b_edit_piece (nboard, mpiece->to, mpiece->piece);
	if (nboard != oldboard)
	    b_del (nboard);
	nboard = newboard;
    }
    gameboard->setboard (nboard);
    if (nboard != oldboard)
	b_del (nboard);

    /* cleanup */
    while ((mpiece = (struct movepiece *) llrembynr (plist, 1)) != NULL)
    {
	free (mpiece->from);
	free (mpiece->to);
	free (mpiece);
    }
    free (plist);
    del_position (startpos);
    del_position (endpos);
    del_position (topos);

    return;
}


void showremgipf (gamestruct * thegame, int playertype, logitem * item)
{
    listheader * piecelist;
    char * piece,
	posstr[3] =  "  ";
    position * ppos;
    board * oldboard,
	* newboard;

    piecelist = logitem_plist (item);
    piece = (char *) llitembynr (piecelist, 1);
    strncpy (posstr, piece, 2);
    ppos = strtopos (posstr);

    /* 
    ** reasons for showing the removal of a gipf
    **    - waitremove > 0
    **    - gipf removed by a computer-player
    */
    if ((playertype != 0) &&
	(thegame->config->waitremove > 0))
    {   /* computerplayer */
	gameboard->setfrom (NULL);
	gameboard->setto (ppos);

 	/* the first Fl::wait is necessary to reset the start of the timer */
	Fl::wait (0);
	for (float time = thegame->config->waitremove * 1.0; time > 0; ) 
	    time = Fl::wait(time);
#if 0
	Fl::check ();
#ifdef MSWIN
	sleep (thegame->config->waitremove * 1000);
#else
	sleep (thegame->config->waitremove);
#endif
#endif
    }
    gameboard->setto (NULL);

    /* now we update the gameboard */
    oldboard = gameboard->getboard ();
    newboard = b_edit_piece (oldboard, ppos, '.');
    gameboard->setboard (newboard);
    b_del (newboard);

    del_position (ppos);

    return;
}


void showremrow (gamestruct * thegame, int playertype, logitem * item)
{
    listheader * poslist,
	* piecelist;
    char start[3] =  "  ",
	end[3] = "  ",
	posstr[3] = "  ",
	* ptr;
    int dir,
	counter;
    position * ppos,
	* prevpos,
	* startpos,
	* endpos;
    board * oldboard,
	* newboard,
	* nboard;

    strncpy (start, logitem_start (item), 2);
    strncpy (end, logitem_end (item), 2);
    startpos = strtopos (start);
    endpos = strtopos (end);

    /* find the direction between start and end */
    if (posp_col (startpos) == posp_col (endpos))
    {
	if (posp_row (startpos) > posp_row (endpos))
	    dir = 2;
	else
	    dir = 3;
    }
    else if (posp_col (startpos) < posp_col (endpos))
    {
	if (posp_row (startpos) > posp_row (endpos))
	    dir = 4;
	else if (posp_row (startpos) < posp_row (endpos))
	    dir = 5;
	else
	{
	    if (posp_col (startpos) < 4)
		dir = 4;
	    else
		dir = 5;
	}
    }
    else
    {
	if (posp_row (startpos) > posp_row (endpos))
	    dir = 0;
	else if (posp_row (startpos) < posp_row (endpos))
	    dir = 1;
	else
	{
	    if (posp_col (startpos) > 4)
		dir = 0;
	    else
		dir = 1;
	}
    }


    /* make a list with all positions between start and end */
    piecelist = logitem_plist (item);
    poslist = (listheader *) malloc (sizeof (listheader));
    newlist (poslist);

    pushll (poslist, startpos);

    prevpos = startpos;
    while ((b_buren[posp_col(prevpos)][posp_row(prevpos)][dir][0] 
	    != posp_col(endpos)) || 
	   (b_buren[posp_col(prevpos)][posp_row(prevpos)][dir][1] 
	    != posp_row(endpos)))
    {
	ppos = new_position ();
	posp_col(ppos) = b_buren[posp_col(prevpos)][posp_row(prevpos)][dir][0];
	posp_row(ppos) = b_buren[posp_col(prevpos)][posp_row(prevpos)][dir][1];

	pushll (poslist, ppos);
	prevpos = ppos;
    }
    pushll (poslist, endpos);

    /* 
    ** reasons for showing the removal of a row
    **    - waitremove > 0
    **    - row automatically removed 
    **      or row removed by a computer-player
    */
    if ((playertype != 0) &&
	(thegame->config->waitremove > 0))
    {   /* computerplayer */
	gameboard->setrow (poslist);

 	/* the first Fl::wait is necessary to reset the start of the timer */
	Fl::wait (0);
	for (float time = thegame->config->waitremove * 1.0; time > 0; ) 
	    time = Fl::wait(time);
#if 0
	Fl::check ();
#ifdef MSWIN
	sleep (thegame->config->waitremove * 1000);
#else
	sleep (thegame->config->waitremove);
#endif
#endif
    }
    gameboard->setrow (NULL);

    /* cleanup */
    while ((ppos = (position *) llrembynr (poslist, 1)) != NULL)
    {
	free (ppos);
    }
    free (poslist);

    /* now we update the gameboard */
    poslist = logitem_plist (item);
    oldboard = gameboard->getboard ();
    nboard = oldboard;
    counter = 1;
    while ((ptr = (char *) llitembynr (poslist, counter)) != 0)
    {
	counter++;
	strncpy (posstr, ptr, 2);
	ppos = strtopos (posstr);
	newboard = b_edit_piece (nboard, ppos, '.');
	if (nboard != oldboard)
	    b_del (nboard);
	nboard = newboard;
	free (ppos);
    }
    gameboard->setboard (nboard);
    
    if (nboard != oldboard)
	b_del (nboard);

    return;
}


/*
** this doesn't make gif-files anymore, but png-files
*/
void show_makegif (board * game)
{
#ifdef HAVEGD
    Fl_Window * gifw;
    drawgif drawing;
    char * str;
    int i;
    static int def_size = 300,
	def_colour = 1;

    gifw = make_gifwindow ();

    // set default values
    gif_filename->value ("./game.png");
    gif_size->value (def_size);
    if (def_colour)
	gif_colour->setonly ();
    else
	gif_bw->setonly ();
    gif_extratext->value (NULL);

    gifw->show ();

    while (1)
    {
	Fl::wait();    

	Fl_Widget *x;
	while ((x = Fl::readqueue())) 
	{
	    if (x == gif_choose)
	    {
		if ((str = file_chooser ("Filename", "*.gif", 
					 gif_filename->value ())) != NULL)
		{
		    gif_filename->value (str);
		}
	    }
	    else if (x == gif_ok)
	    {
		drawing.filename (gif_filename->value ());
		def_size = (int) gif_size->value ();
		drawing.gifsize (def_size);
		drawing.gifboard (game);
		if (gif_colour->value ())
		    def_colour = 1;
		else
		    def_colour = 0;
		drawing.gifcolour (def_colour);

		str = gif_extratext->value ();
		// check if the string contains something
		for (i = 0; i < strlen (str); i++)
		{
		    if (! isspace ((int) str[i]))
		    {
			drawing.addtext (str);
			break;
		    }
		}
		drawing.draw ();

		delete gifw;
		return;
	    }
	    else if (x == gif_cancel)
	    {
		delete gifw;
		return;
	    }
	}
    }
#endif

    return;
}
