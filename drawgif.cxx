/*
** $Id: drawgif.cxx,v 1.7 2000/01/25 19:16:34 kurt Exp $
**
** member-functions for the drawgif-class
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

#ifdef HAVEGD

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "drawgif.h"

extern "C" {
#include <gd.h>
#include <gdfontg.h>
#include <gdfontl.h>
#include <gdfontmb.h>
#include <gdfonts.h>
#include <gdfontt.h>
}

#ifndef min
#define min(x,y)       (x < y ? x : y)
#endif

#ifndef max
#define max(x,y)       (x > y ? x : y)
#endif

#define round(x)       (int)(x + 0.5)
#define MY_PI          3.14159

// x and y-position of each first point of a column
// based on a square of 2.5 by 2.5
double gif_rowbase[9][2] = {
    {.384, 2}, // row a
    {.6005, 2.125}, // row b
    {.817, 2.25}, // row c
    {1.0335, 2.375}, // row d
    {1.25, 2.5}, // row e
    {1.4665, 2.375}, // row f
    {1.683, 2.25}, // row g
    {1.8995, 2.125}, // row h
    {2.116, 2}};// row i

/* lines on the board */
const position gif_linetable[21][2] = {
    {{0, 4}, {5, 8}},    {{0, 3}, {6, 7}},    {{0, 2}, {7, 6}},
    {{0, 1}, {8, 5}},    {{1, 1}, {8, 4}},    {{2, 1}, {8, 3}},
    {{3, 1}, {8, 2}},    {{0, 2}, {5, 1}},    {{0, 3}, {6, 1}},
    {{0, 4}, {7, 1}},    {{0, 5}, {8, 1}},    {{1, 6}, {8, 2}},
    {{2, 7}, {8, 3}},    {{3, 8}, {8, 4}},    {{1, 6}, {1, 1}},
    {{2, 7}, {2, 1}},    {{3, 8}, {3, 1}},    {{4, 9}, {4, 1}},
    {{5, 8}, {5, 1}},    {{6, 7}, {6, 1}},    {{7, 6}, {7, 1}}
};

/* possible from-points */
const position gif_fromtable[24] = {
    {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {1, 6},
    {2, 7}, {3, 8}, {4, 9}, {5, 8}, {6, 7}, {7, 6},
    {8, 5}, {8, 4}, {8, 3}, {8, 2}, {8, 1}, {7, 1},
    {6, 1}, {5, 1}, {4, 1}, {3, 1}, {2, 1}, {1, 1},
};

/* white hexagon */
const position gif_hexagon[7] = {
    {1, 2}, {1, 5}, {4, 8}, {7, 5}, {7, 2}, {4, 2}, {1, 2}
};


drawgif::drawgif ()
{
    filen = NULL;
    size = 300;
    colour = 1;
    gipfb = NULL;
    extratext = NULL;

    return; 
}


drawgif::~drawgif ()
{
    char * ptr;

    if (filen != NULL)
	free (filen);

    if (gipfb != NULL)
	b_del (gipfb);

    if (extratext != NULL)
    {
	while ((ptr = (char *) llrembynr (extratext, 1)) != NULL)
	    free (ptr);

	free (extratext);
    }

    return;
}


void drawgif::filename (const char * fn)
{
    if (filen != NULL)
	free (filen);

    filen = strdup (fn);

    return;
}


void drawgif::gifboard (board * gb)
{
    if (gipfb != NULL)
	b_del (gipfb);

    gipfb = b_copy (gb);

    return;
}


void drawgif::addtext (char * text)
{
    char * tempstr,
	* kar1,
	* kar2;

    if (extratext == NULL)
    {
	extratext = (listheader *) malloc (sizeof (listheader));
	newlist (extratext);
    }

    tempstr = strdup (text);
    kar1 = tempstr;

    // split on '\n'
    while ((kar2 = strchr (kar1, '\n')) != NULL)
    {
	kar2[0] = '\0';
	pushll (extratext, strdup (kar1));
	kar1 = kar2 + 1;
    }
    pushll (extratext, strdup (kar1));

    free (tempstr);

    return;
}


int drawgif::draw ()
{
    int black,
	white,
	yellow,
	lgray,
	counter,
	offset,
	gray,
	red,
	notused,
	texth,
	pieceh,
	poffset,
	i, j;
    gdImagePtr image;
    FILE * imagefp;
    FILE * fp;
    double base8;
    int diam,
	x,y,
	x1, y1, x2, y2,
	nr,
	diam2;
    char tempstr[20];
    gdFontPtr thefont = gdFontTiny;
    gdPoint hexa[7];
    listheader * extralist;
    int extralines = 0,
	maxchar;
    char * strptr;
    gdPoint rotarrow[7];
    double sina,
	cosa;
    position * temppos;

    /* choose font according to drawing-size */
    if (size < 200)
	thefont = gdFontTiny;
    else if (size < 250)
	thefont = gdFontSmall;
    else if (size < 350)
	thefont = gdFontMediumBold;
    else if (size < 450)
	thefont = gdFontLarge;
    else
	thefont = gdFontGiant;

    /*
    ** find out how many extra lines have to be displayed
    ** and split lines that are to long to fit
    */
    extralist = (listheader *) malloc (sizeof (listheader));
    newlist (extralist);
    maxchar = size / thefont->w - 2;
    counter = 1;
    while ((strptr = (char *) llitembynr (extratext, counter)) != NULL)
    {
	counter++;
	extralines += splitline (extralist, strptr, maxchar);
    }
    
    /* create image */
    base = size / 2.5;
    image = gdImageCreate(size, size + thefont->h *
			  (extralines == 0 ? 0 : extralines + 1));
    gdImageInterlace(image, 1);

    /* all colors have been chosen to be the ones used by Netscape
       when used in 256 color mode */
    
    /* Allocate the color lgray (red, green and blue).
       Since this is the first color in a new image, it will
       be the background color. */
    if (!colour)
    {
	white = gdImageColorAllocate(image, 255, 255, 255);        
	black = gdImageColorAllocate(image, 0, 0, 0);      

	/*lgray = white;*/
    }
    else
	lgray = gdImageColorAllocate(image, 204, 204, 204);
    
    /* Allocate the color black (red, green and blue). */
    if (!colour)
    {
	yellow = white;
	gray = black;
	red = black;
    }
    else
    {
	white = gdImageColorAllocate(image, 255, 255, 255);        
	black = gdImageColorAllocate(image, 0, 0, 0);      
	yellow = gdImageColorAllocate(image, 255, 255, 204);
	gray = gdImageColorAllocate (image, 153, 153, 153);
	red = gdImageColorAllocate (image, 255, 0, 0);
    }
    
    /* the 'notused' color can be used for drawing the outline of a figure
       you want to fill later */
    notused = gdImageColorAllocate (image, 1, 1, 1);
    
    /* draw rectangle around image */
    gdImageRectangle (image, 0, 0, size-1, size-1, black);

    /* draw hexagon */
    for (i = 0; i < 6; i++)
    {
	pos2coor (&gif_hexagon[i], hexa[i].x, hexa[i].y);
    }
    gdImageFilledPolygon(image, hexa, 6, white);
    
    /* draw from-points */
    diam = round (base / 12);
    for (i = 0; i < 24; i++)
    {
	pos2coor (&gif_fromtable[i], x, y);
	gdImageArc(image,
		   x, y,
		   diam, diam,
		   0, 360, black);
	gdImageFillToBorder(image,
			    x, y,
			    black, black);
    }

    /* draw lines */
    for (i = 0 ; i < 21; i++)
    {
	pos2coor (&gif_linetable[i][0], x1, y1);
	pos2coor (&gif_linetable[i][1], x2, y2);
	gdImageLine (image, 
		     x1, y1, x2, y2,
		     black);
    }
    
    /* draw pieces */
    /* calculate size of piece */
    diam = round (base / 6);
    diam2 = round (base / 10);

    temppos = (position *) malloc (sizeof (position));

    for (i = 1; i < 8; i++)
	for (j = 2; j <= b_colsize (i); j++)
	{
	    posp_col (temppos) = i;
	    posp_row (temppos) = j;

	    if (b_ppiece (gipfb, temppos) != '.')
	    {
		/* calculate position of center of the piece */
		pos2coor (temppos, x, y);

		gdImageArc (image, x, y, diam, diam, 0, 360, notused);
		if ((b_ppiece (gipfb, temppos) == 'o') || 
		    (b_ppiece (gipfb, temppos) == 'O'))
		{
		    gdImageFillToBorder (image, x, y, notused, yellow);
		}
		else
		{
		    gdImageFillToBorder (image, x, y, notused, black);
		}
		gdImageArc (image, x, y, diam, diam, 0, 360, black);

		if ((b_ppiece (gipfb, temppos) == 'X') || 
		    (b_ppiece (gipfb, temppos) == 'O'))
		{
		    if (!colour)
		    {
			if (b_ppiece (gipfb, temppos) == 'O')
			    gray = black;
			else
			    gray = white;
		    }
	    
		    gdImageArc (image, x, y, diam2, diam2, 0, 360, gray);
		    if (size >= 250)
			gdImageArc (image, x, y, diam2-1, diam2-1, 0, 360, 
				    gray);
		}
	    }
	}

    free (temppos);

#if 0
    /* draw arrows */
    base8 = base / 8;
    while ((arr = (arrowitem *) llrembynr (arrowlist, 1)) != NULL)
    {
	/* calculate position */
	x = round (rowbase[arr->col][0] * base);
	y = round ((rowbase[arr->col][1] - arr->row * .25) * base);

	cosa = cos ((double) arr->angle * MY_PI / 180);
	sina = sin ((double) arr->angle * MY_PI / 180);
	for (i = 0; i < 8; i++)
	{   /* the 8th point is the center and used by the fill */
 	    rotarrow[i].x = x +
		round (base8 * (arrow[i][0] * cosa - arrow[i][1] * sina));
 	    rotarrow[i].y = y -
		round (base8 * (arrow[i][0] * sina + arrow[i][1] * cosa));
	}
	
	gdImagePolygon (image, rotarrow, 7, notused);
	gdImageFillToBorder (image, rotarrow[7].x, rotarrow[7].y,
			     notused, red);
	gdImagePolygon (image, rotarrow, 7, black);

	free (arr);
    }
    free (arrowlist);
#endif
    
    /* draw position-names */
    for (i = 0; i < 9; i++)
    {
	sprintf (tempstr, "%c1", i+'a');
	x = round (gif_rowbase[i][0] * base) - thefont->w;
	y = round ((gif_rowbase[i][1] - .125) * base) - 3;

	gdImageString(image, thefont, x, y, tempstr, black);

	nr = (i < 5 ? i + 5 : 13 - i);
	sprintf (tempstr, "%c%d", i+'a', nr);
	y = size - round ((gif_rowbase[i][1] - .125) * base) - thefont->h + 3;

	gdImageString(image, thefont, x, y, tempstr, black);
    }
    
    /* draw piles of available pieces */
    pieceh = round (base / 30);
    poffset = round (base / 20);
    diam = round (base / 6);
    texth = thefont->h;

    /* white */
    sprintf (tempstr, "white: %d", b_white (gipfb));
    gdImageString (image, thefont, diam/4, size-texth*2-3, tempstr, black);

    offset = size - texth*2 - 3;
    for (i = 1; i <= b_white (gipfb); i++)
    {
	gdImageRectangle (image,
			  diam/2, offset - poffset * i,
			  round (diam * 1.5), offset - poffset * i + pieceh,
			  black);
	gdImageFillToBorder (image, diam/2+1,
			     offset - poffset * i + 1,
			     black, yellow);
    }
    sprintf (tempstr, "lost : %d", b_white_lost (gipfb));
    gdImageString (image, thefont, diam/4, size-texth-3, tempstr, black);
    
    /* black */
    sprintf (tempstr, "%d :black", b_black (gipfb));
    gdImageString (image, thefont,
		   size - diam/4 - thefont->w * strlen (tempstr),
		   size-texth*2-3, tempstr, black);

    offset = size - texth*2 - 3;
    for (i = 1; i <= b_black (gipfb); i++)
    {
	gdImageRectangle (image,
			  round (size - diam * 1.5), offset - poffset * i,
			  size - diam/2, offset - poffset * i + pieceh,
			  black);
	gdImageFillToBorder (image, size - diam/2 - 1,
			     offset - poffset * i + 1,
			     black, black);
    }
    sprintf (tempstr, "%d : lost", b_black_lost (gipfb));
    gdImageString (image, thefont,
		   size - diam/4 - thefont->w * strlen (tempstr),
		   size-texth-3, tempstr, black);

    /*
    ** draw gipf-logo
    ** (if the file is available)
    */
    if ((imagefp = fopen ("small_logo.png", "rb")) != NULL)
    {
	gdImagePtr loadimage;

	loadimage = gdImageCreateFromPng (imagefp);
	fclose (imagefp);
	if (loadimage != NULL)
	{
	    gdImageCopy(image, loadimage, 1, 1, 0, 0,
			loadimage->sx, loadimage->sy);
			 

	    gdImageDestroy (loadimage);
	}
    }

    /* draw extra lines of text */
    if (extralines > 0)
    {
	gdImageFilledRectangle (image, 0, size, size-1,
				size + thefont->h * (extralines + 1) - 1,
				white);
	gdImageRectangle (image, 0, size, size-1,
			  size + thefont->h * (extralines + 1) - 1,
			  black);
	i = 0;
	while ((strptr = (char *) llrembynr (extralist, 1)) != NULL)
	{
	    gdImageString (image, thefont, thefont->w,
		           size + thefont->h / 2 + thefont->h * i,
			   strptr, black);
	    free (strptr);
	    i++;
	}
    }
    free (extralist);

    gdImageColorDeallocate(image, notused);
    
    /* open file for output */
    fp = fopen(filen, "wb");
    /* Output the image to the disk file. */
    gdImagePng(image, fp);    
    /* Close the file. */
    fclose(fp);

    /* Destroy the image in memory. */
    gdImageDestroy(image);
    
    
    return (1);
}


/*
** the 'string' will be copied and added to 'llist'
** if 'string' is longer then 'maxchar', it will be splitted
** into several strings and added to 'llist'
**
** returns: nr of strings added to 'llist'
*/
int drawgif::splitline (listheader * llist, char * string, int maxchar)
{
    char * newstring,
	* kar,
	* kar2,
	* tempstr,
	savechar = 0;
    int counter = 0;

    tempstr = strdup (string);
    kar = tempstr;
    while (strlen(kar) > maxchar)
    {
	kar2 = kar + maxchar;
	while ((kar2 > kar) && (!isspace ((int) kar2[0])))
	{
	    kar2--;
	}
	if (kar2 == kar)
	{   /* string too long to be splitted, do the dirty split */
	    savechar = kar[maxchar];
	    kar2 = kar + maxchar;
	}
	kar2[0] = '\0';
	newstring = (char *) malloc (strlen (kar) + 1);
	strcpy (newstring, kar);
	pushll (llist, (void *) newstring);
	counter++;

	if (savechar != 0)
	{
	    kar[maxchar] = savechar;
	    savechar = 0;
	    kar = kar2;
	}
	else
	{
	    kar = kar2 + 1;
	}
    }

    /* last piece */
    newstring = (char *) malloc (strlen (kar) + 1);
    strcpy (newstring, kar);
    pushll (llist, (void *) newstring);
    counter++;

    free (tempstr);

    return (counter);
}


/*
** calculate coordinates starting from a board-position
*/
void drawgif::pos2coor (const position *pos, int& x, int& y)
{
    x = round (gif_rowbase[pos->col][0] * base);
    y = round ((gif_rowbase[pos->col][1] - pos->row * .25) * base);

    return;
}

#endif
