/*
** $Id: setup_win.cxx,v 1.5 1998/11/01 16:28:38 kurt Exp $
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

#include "setup_win.h"
#include "gipf_ui.H"
#include "configfile.h"

/*
** returns:
**    1: user pressed OK, values may be changed
**    0: user pressed CANCEL, no values changed
*/
int show_setup (configvalues * config)
{
    Fl_Window * setupw;

    setupw = make_setupwindow ();
    slider_computer->value ((double) config->waitcomputer);
    slider_remove->value ((double) config->waitremove);
    check_posname->value (config->showposname);

    switch (config->searchdepth)
    {
    case 1:
	level1->setonly();
	break;
    case 2:
	level2->setonly();
	break;
    case 3:
	level3->setonly();
	break;
    case 4:
	level4->setonly();
	break;
    case 5:
	level5->setonly();
	break;
    case 6:
	level6->setonly();
	break;
    case 7:
	level7->setonly();
	break;
    case 8:
	level8->setonly();
	break;
    }

    switch (config->animate)
    {
    case 0:
	ani_none->setonly();
	break;
    case 1:
	ani_slow->setonly();
	break;
    case 2:
	ani_medium->setonly();
	break;
    case 3:
	ani_fast->setonly();
	break;
    }

    setupw->show();

    while (1)
    {
	Fl::wait();    

	Fl_Widget *x;
	while ((x = Fl::readqueue())) 
	{
	    if (x == setup_ok)
	    {
		/* get values */
		config->waitcomputer = (int) slider_computer->value ();
		config->waitremove = (int) slider_remove->value ();
		config->showposname = check_posname->value ();

		if (level1->value()) config->searchdepth = 1;
		else if (level2->value()) config->searchdepth = 2;
		else if (level3->value()) config->searchdepth = 3;
		else if (level4->value()) config->searchdepth = 4;
		else if (level5->value()) config->searchdepth = 5;
		else if (level6->value()) config->searchdepth = 6;
		else if (level7->value()) config->searchdepth = 7;
		else if (level8->value()) config->searchdepth = 8;

		if (ani_none->value()) config->animate = 0;
		else if (ani_slow->value()) config->animate = 1;
		else if (ani_medium->value()) config->animate = 2;
		else if (ani_fast->value()) config->animate = 3;

		delete setupw;
		return (1);
	    }
	    else if (x == setup_cancel)
	    {
		delete setupw;
		return (0);
	    }
	}
    }

    return(0);
}


void retrieveconfig (configvalues * config)
{
    config->configlist = readconfigfile ("gf1.cfg");

    config->searchdepth = findconfigvalue (config->configlist, "searchdepth", 
					   ' ', 3);
    config->animate = findconfigvalue (config->configlist, "animate", ' ', 2);
    config->waitcomputer = findconfigvalue (config->configlist, 
					    "waitcomputer", ' ', 2);
    config->waitremove = findconfigvalue (config->configlist, "waitremove", 
					  ' ', 3);
    config->showposname = findconfigvalue (config->configlist, "showposname",
					   ' ', 1);
    return;
}

void writeconfig (configvalues * config)
{
    changeconfigvalue (config->configlist, "searchdepth", config->searchdepth);
    changeconfigvalue (config->configlist, "animate", config->animate);
    changeconfigvalue (config->configlist, "waitcomputer", 
		       config->waitcomputer);
    changeconfigvalue (config->configlist, "waitremove", config->waitremove);
    changeconfigvalue (config->configlist, "showposname", config->showposname);

    writeconfigfile ("gf1.cfg", config->configlist);

    return;
}

