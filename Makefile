# 
# $Id: Makefile,v 1.38 2000/02/26 21:16:05 kurt Exp $
#

# put the next line in comment if you don't have the gd-library
GDLIB = -DHAVEGD

# put the next line in comment if not compiling for MS Windows
#MSWIN = -DMSWIN

# this uses the new filechooser widget
NEWFILECHOOSER = -DNEWFILECHOOSER
FCDIR = FileChooser-1.4.1

# set your compile-options here
COMPILEFLAGS = -O3
#COMPILEFLAGS = -O0 -g -Wall #-pg

###########################################
# no user servicable parts after this line
###########################################

# are there any extra include paths
ifdef GDLIB
  ifdef MSWIN
    EXTRAINCLUDES = -I./gd -I./libpng -I./zlib
  else
    EXTRAINCLUDES = -I/usr/local/include
  endif
endif

ifdef NEWFILECHOOSER
  EXTRAINCLUDES += -I./$(FCDIR)
endif

# what libraries to link with
ifdef GDLIB
  ifndef MSWIN
    gd_libs =  -L/usr/local/lib -lgd -lpng -lz
  else
    gd_libs =  -L./gd -lgd -L./libpng -lpng -L./zlib -lz
  endif
endif

ifdef MSWIN
  libs = -lgdi32 -luser32 -lfltk -lwsock32 -mwindows $(gd_libs)
else
  libs = -L/usr/X11R6/lib -lX11 -lXext -lm -lfltk $(gd_libs)
#  libs += -L/usr/local/lib /usr/local/lib/ccmalloc.o $(gd_libs) \
#  -lccmalloc -ldl 
endif

DEFINES = -DWINGIPF $(MSWIN) $(GDLIB) $(NEWFILECHOOSER) $(EXTRAINCLUDES)

CFLAGS = $(COMPILEFLAGS) $(DEFINES)
DATE = `date +'%Y%m%d'`
CC = gcc
CXX = g++
FLUID = fluid
PERL = perl
WINDRES = windres

# sources
sources_c = configfile.c linklist.c position.c \
            rem_gipf.c rem_row.c gettimeofday.c
sources_cxx = gipf_ui.cxx callbacks.cxx core.cxx fl_board.cxx fl_pile.cxx \
              setup_win.cxx thegame.cxx win_human.cxx drawgif.cxx \
              ai_player.cxx ai_gipf.cxx xmlite.cxx board2.cxx gamelog2.cxx

ifdef NEWFILECHOOSER
sources_filechooser = $(FCDIR)/FileBrowser.cxx $(FCDIR)/FileChooser.cxx \
                      $(FCDIR)/FileChooser2.cxx $(FCDIR)/FileIcon.cxx \
                      $(FCDIR)/FileInput.cxx
endif

# objectfiles
objects = $(sources_c:.c=.o) $(sources_cxx:.cxx=.o) \
          $(sources_filechooser:.cxx=.o)
ifdef MSWIN
  objects += icons.o
endif

# executable-name
ifdef MSWIN
  program = gf1.exe
else
  program = gf1
endif

.SUFFIXES: .cxx .o
.cxx.o:
	$(CXX) $(CFLAGS) -c -o $@ $*.cxx

.PHONY: all clean realclean fullzip winzip linuxzip doc

all: $(program)

clean:
	- rm $(program) $(objects) 

realclean: clean
	- rm *.d gipf_ui.H gipf_ui.cxx helptext.h gf1_doc.txt gf1_doc*.html

fullzip:
	zip gf1_src_$(DATE) RCS/* * $(FCDIR)/* \
	                     -x core \*.o \*.zip \*~ $(program)

winzip:
	zip gf1_wexe gf1.exe gf1_doc.txt README.txt COPYING.txt HISTORY.txt \
            gf1_doc*.html TODO.txt small_logo.png

linuxzip:
	zip gf1_lexe gf1 gf1_doc.txt README.txt COPYING.txt HISTORY.txt \
            gf1_doc*.html TODO.txt small_logo.png

doc: gf1_doc.html

$(program): $(objects)
	$(CXX) $(CFLAGS) -o $@ $^ $(libs) 

gipf_ui.H gipf_ui.cxx: gipf_ui.fl
	$(FLUID) -c gipf_ui.fl
	mv gipf_ui.C gipf_ui.cxx

helptext.h: gf1_doc.txt
	$(PERL) makehelptext.pl $< $@

gf1_doc.txt: gf1_doc.sgml
	sgml2txt -f $<

gf1_doc.html: gf1_doc.sgml
	sgml2html $<

icons.o: icons.rc win_icon.ico
	$(WINDRES) $< $@


ifndef MSWIN

# some things for automatic dependency generation
%.d: %.c
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $(DEFINES) $< \
		| sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'

%.d: %.cxx
	$(SHELL) -ec '$(CXX) -MM $(CPPFLAGS) $(DEFINES) $< \
		| sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'

include $(sources_c:.c=.d)
include $(sources_cxx:.cxx=.d)

# not all dependencies can be added automatically
# so I add some here myself
win_human.cxx thegame.cxx setup_win.cxx core.cxx: gipf_ui.H
callbacks.cxx: helptext.h

endif
