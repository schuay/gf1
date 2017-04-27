/*
** $Id: configfile.c,v 1.2 1998/11/01 18:43:09 kurt Exp $
**
** functions for reading the gipf-configfile
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
#include "configfile.h"

typedef struct {
    char name[40];
    int value;
} configinfo;

int compconfig (void * data1, void * data2);
void delconfig (void * data);

int writeconfigfile (char * fname, listheader * configlist)
{
    FILE * cfg;
    configinfo * configitem;
    int i;
    
    cfg = fopen (fname, "w");
    if (cfg == NULL)
    {
	return (-1);
    }

    i = 1;
    while ((configitem = (configinfo *) llitembynr (configlist, i)) != NULL)
    {
	fprintf (cfg, "%s = %d\n", configitem->name, configitem->value);
	i++;
    }

    fclose (cfg);

    return (0);
}

listheader * readconfigfile (char * fname)
{
    FILE * cfg;
    listheader * configlist;
    char buffer[200],
	tempstr[100];
    int result,
	value;
    configinfo * configitem;
    
    configlist = (listheader *) malloc (sizeof (listheader));
    newlist (configlist);

    /* read configuration file */
    cfg = fopen (fname, "r");
    if (cfg == NULL)
    {
	return (configlist);
    }

    while (fgets (buffer, 200, cfg) != NULL)
    {
        result = sscanf (buffer, " %s = %d", tempstr, &value);
	if (result == 2)
	{
	    configitem = (configinfo *) malloc (sizeof (configinfo));
	    strcpy (configitem->name, tempstr);
	    configitem->value = value;

	    if (insertll (configlist, (void *) configitem, compconfig) != 0)
	    {   /* item already in the list */
		free (configitem);
	    }
	}
    }

    fclose (cfg);

    return (configlist);
}


void clearconfiglist (listheader * configlist)
{
    emptyll (configlist, delconfig);
    free (configlist);

    return;
}


int compconfig (void * data1, void * data2)
{
    configinfo * item1 = data1,
	* item2 = data2;

    return (strcmp (item1->name, item2->name));
}

void delconfig (void * data)
{
    configinfo * item = data;

    free (item);

    return;
}

/*
** parameters:
**    clist: list with config-info from config-file
**    name: string to look for
**    colour: colour to add to string
**    defval: default value if nothing found in the config-file
**
** returns:
**    value
*/
int findconfigvalue (listheader * clist, char * name, char colour, int defval)
{
    configinfo search_c,
	* config_p,
	* configitem;

    if (clist == NULL)
    {
	return (defval);
    }

    /* search for a specific configuration value for this colour player */
    sprintf (search_c.name, "%s_%c", name, colour);
    if ((config_p = (configinfo *)
	 searchll (clist, (void *) &search_c, compconfig)) != NULL)
    {
	return (config_p->value);
    }

    /* search for a general configuration value for the parameter */
    strcpy (search_c.name, name);
    if ((config_p = (configinfo *)
	 searchll (clist, (void *) &search_c, compconfig)) != NULL)
    {
	return (config_p->value);
    }

    /* if parameter not found, add it to the list with the default value */
    configitem = (configinfo *) malloc (sizeof (configinfo));
    strcpy (configitem->name, name);
    configitem->value = defval;

    if (insertll (clist, (void *) configitem, compconfig) != 0)
    {   /* item already in the list */
	free (configitem);
    }

    return (defval);
}

void changeconfigvalue (listheader * clist, char * name, int newval)
{
    configinfo search_c,
	* config_p,
	* configitem;

    if (clist == NULL)
    {
	return;
    }

    /* search for a general configuration value for the parameter */
    strcpy (search_c.name, name);
    if ((config_p = (configinfo *)
	 searchll (clist, (void *) &search_c, compconfig)) != NULL)
    {
	config_p->value = newval;
	return;
    }

    /* if parameter not found, add it to the list with the new value */
    configitem = (configinfo *) malloc (sizeof (configinfo));
    strcpy (configitem->name, name);
    configitem->value = newval;

    if (insertll (clist, (void *) configitem, compconfig) != 0)
    {   /* item already in the list */
	free (configitem);
    }

    return;
}

