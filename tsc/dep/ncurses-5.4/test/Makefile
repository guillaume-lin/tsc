# $Id: Makefile.in,v 1.77 2003/12/07 01:36:16 tom Exp $
##############################################################################
# Copyright (c) 1998-2002,2003 Free Software Foundation, Inc.                #
#                                                                            #
# Permission is hereby granted, free of charge, to any person obtaining a    #
# copy of this software and associated documentation files (the "Software"), #
# to deal in the Software without restriction, including without limitation  #
# the rights to use, copy, modify, merge, publish, distribute, distribute    #
# with modifications, sublicense, and/or sell copies of the Software, and to #
# permit persons to whom the Software is furnished to do so, subject to the  #
# following conditions:                                                      #
#                                                                            #
# The above copyright notice and this permission notice shall be included in #
# all copies or substantial portions of the Software.                        #
#                                                                            #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    #
# THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    #
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        #
# DEALINGS IN THE SOFTWARE.                                                  #
#                                                                            #
# Except as contained in this notice, the name(s) of the above copyright     #
# holders shall not be used in advertising or otherwise to promote the sale, #
# use or other dealings in this Software without prior written               #
# authorization.                                                             #
##############################################################################
#
# Author: Thomas E. Dickey 1996-2002
#
# Makefile for ncurses tests.

# turn off _all_ suffix rules; we'll generate our own
.SUFFIXES:

SHELL		= /bin/sh



x		= 
o		= .o

MODEL		= ../objects
srcdir		= .
prefix		= /usr
exec_prefix	= ${prefix}
libdir		= ${exec_prefix}/lib
includedir	= ${prefix}/include

LIBTOOL		= 
LIBTOOL_CLEAN	= 
LIBTOOL_COMPILE	= 
LIBTOOL_LINK	= 

CC		= gcc
CPP		= gcc -E

CFLAGS		= -O2
CPPFLAGS	=  -I../test -I$(srcdir) -DHAVE_CONFIG_H -I. -I../include  -D_GNU_SOURCE -DNDEBUG

CCFLAGS		= $(CPPFLAGS) $(CFLAGS)

CFLAGS_LIBTOOL	= $(CCFLAGS)
CFLAGS_NORMAL	= $(CCFLAGS)
CFLAGS_DEBUG	= $(CCFLAGS) -g -DTRACE
CFLAGS_PROFILE	= $(CCFLAGS) -pg
CFLAGS_SHARED	= $(CCFLAGS) -fPIC

CFLAGS_DEFAULT	= $(CFLAGS_NORMAL)

REL_VERSION	= 5.4
ABI_VERSION	= 5
LOCAL_LIBS	= ../lib/libform.a ../lib/libmenu.a ../lib/libpanel.a ../lib/libncurses.a 
MATH_LIB	= -lm

LD		= ld
LINK		=  $(LIBTOOL_LINK) $(CC) $(CFLAGS)

LDFLAGS		=  -L../lib -lform -lmenu -lpanel -lncurses     

LDFLAGS_LIBTOOL	= $(LDFLAGS) $(CFLAGS_LIBTOOL)
LDFLAGS_NORMAL	= $(LDFLAGS) $(CFLAGS_NORMAL)
LDFLAGS_DEBUG	= $(LDFLAGS) $(CFLAGS_DEBUG)
LDFLAGS_PROFILE	= $(LDFLAGS) $(CFLAGS_PROFILE)
LDFLAGS_SHARED	= $(LDFLAGS) $(CFLAGS_SHARED) 

LDFLAGS_DEFAULT	= $(LDFLAGS_NORMAL)

LINT		= 
LINT_OPTS	= 
LINT_LIBS	= -lform -lmenu -lpanel -lncurses 

TESTS = \
	background$x \
	blue$x \
	bs$x \
	cardfile$x \
	color_set$x \
	demo_defkey$x \
	demo_forms$x \
	demo_keyok$x \
	demo_menus$x \
	demo_panels$x \
	ditto$x \
	dots$x \
	filter$x \
	firework$x \
	firstlast$x \
	gdc$x \
	hanoi$x \
	hashtest$x \
	ins_wide$x \
	inserts$x \
	keynames$x \
	knight$x \
	lrtest$x \
	ncurses$x \
	newdemo$x \
	railroad$x \
	rain$x \
	tclock$x \
	testaddch$x \
	testcurs$x \
	testscanw$x \
	view$x \
	worm$x \
	xmas$x

all: $(TESTS)

sources:

background$x: $(MODEL)/background$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/background$o $(LDFLAGS_DEFAULT)

blue$x: $(MODEL)/blue$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/blue$o $(LDFLAGS_DEFAULT)

bs$x: $(MODEL)/bs$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/bs$o $(LDFLAGS_DEFAULT)

cardfile$x: $(MODEL)/cardfile$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/cardfile$o $(LDFLAGS_DEFAULT)

color_set$x: $(MODEL)/color_set$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/color_set$o $(LDFLAGS_DEFAULT)

demo_defkey$x: $(MODEL)/demo_defkey$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/demo_defkey$o $(LDFLAGS_DEFAULT)

demo_forms$x: $(MODEL)/demo_forms$o $(MODEL)/edit_field$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/demo_forms$o $(MODEL)/edit_field$o $(LDFLAGS_DEFAULT)

demo_keyok$x: $(MODEL)/demo_keyok$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/demo_keyok$o $(LDFLAGS_DEFAULT)

demo_menus$x: $(MODEL)/demo_menus$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/demo_menus$o $(LDFLAGS_DEFAULT)

demo_panels$x: $(MODEL)/demo_panels$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/demo_panels$o $(LDFLAGS_DEFAULT)

ditto$x: $(MODEL)/ditto$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/ditto$o $(LDFLAGS_DEFAULT)

dots$x: $(MODEL)/dots$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/dots$o $(LDFLAGS_DEFAULT)

filter$x: $(MODEL)/filter$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/filter$o $(LDFLAGS_DEFAULT)

firework$x: $(MODEL)/firework$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/firework$o $(LDFLAGS_DEFAULT)

firstlast$x: $(MODEL)/firstlast$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/firstlast$o $(LDFLAGS_DEFAULT)

gdc$x: $(MODEL)/gdc$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/gdc$o $(LDFLAGS_DEFAULT)

hanoi$x: $(MODEL)/hanoi$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/hanoi$o $(LDFLAGS_DEFAULT)

hashtest$x: $(MODEL)/hashtest$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/hashtest$o $(LDFLAGS_DEFAULT)

inserts$x: $(MODEL)/inserts$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/inserts$o $(LDFLAGS_DEFAULT)

ins_wide$x: $(MODEL)/ins_wide$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/ins_wide$o $(LDFLAGS_DEFAULT)

keynames$x: $(MODEL)/keynames$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/keynames$o $(LDFLAGS_DEFAULT)

knight$x: $(MODEL)/knight$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/knight$o $(LDFLAGS_DEFAULT)

lrtest$x: $(MODEL)/lrtest$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/lrtest$o $(LDFLAGS_DEFAULT)

ncurses$x: $(MODEL)/ncurses$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/ncurses$o $(LDFLAGS_DEFAULT)

newdemo$x: $(MODEL)/newdemo$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/newdemo$o $(LDFLAGS_DEFAULT)

rain$x: $(MODEL)/rain$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/rain$o $(LDFLAGS_DEFAULT)

railroad$x: $(MODEL)/railroad$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/railroad$o $(LDFLAGS_DEFAULT)

tclock$x: $(MODEL)/tclock$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/tclock$o $(LDFLAGS_DEFAULT) $(MATH_LIB)

testaddch$x: $(MODEL)/testaddch$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/testaddch$o $(LDFLAGS_DEFAULT)

testcurs$x: $(MODEL)/testcurs$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/testcurs$o $(LDFLAGS_DEFAULT)

testscanw$x: $(MODEL)/testscanw$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/testscanw$o $(LDFLAGS_DEFAULT)

view$x: $(MODEL)/view$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/view$o $(LDFLAGS_DEFAULT)

worm$x: $(MODEL)/worm$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/worm$o $(LDFLAGS_DEFAULT)

xmas$x: $(MODEL)/xmas$o $(LOCAL_LIBS)
	 $(LINK) -o $@ $(MODEL)/xmas$o $(LDFLAGS_DEFAULT)

libs \
install \
install.libs \
install.test:

uninstall:
uninstall.libs:
uninstall.test:

mostlyclean ::
	-rm -f core tags TAGS *~ *.bak *.i *.ln *.atac trace

clean :: mostlyclean
	-sh -c "if test -n '$x' ; then $(MAKE) clean x=''; fi"
	-rm -rf *$o screendump *.lis $(TESTS) .libs

distclean :: clean
	-rm -f Makefile ncurses_cfg.h config.*

realclean :: distclean

lint:
	sh -c 'for N in $(TESTS); do echo LINT:$$N; $(LINT) $(LINT_OPTS) $(CPPFLAGS) $(srcdir)/$$N.c $(LINT_LIBS); done'

# generated by mk-0th.awk

.SUFFIXES: .c .cc .h .i .ii
.c.i :
	$(CPP) $(CPPFLAGS) $< >$@
.cc.ii :
	$(CPP) $(CPPFLAGS) $< >$@
.h.i :
	$(CPP) $(CPPFLAGS) $< >$@

C_SRC = \
	$(srcdir)/background.c \
	$(srcdir)/blue.c \
	$(srcdir)/bs.c \
	$(srcdir)/cardfile.c \
	$(srcdir)/color_set.c \
	$(srcdir)/demo_defkey.c \
	$(srcdir)/demo_keyok.c \
	$(srcdir)/demo_forms.c \
	$(srcdir)/demo_menus.c \
	$(srcdir)/demo_panels.c \
	$(srcdir)/ditto.c \
	$(srcdir)/dots.c \
	$(srcdir)/edit_field.c \
	$(srcdir)/filter.c \
	$(srcdir)/firework.c \
	$(srcdir)/firstlast.c \
	$(srcdir)/gdc.c \
	$(srcdir)/hanoi.c \
	$(srcdir)/hashtest.c \
	$(srcdir)/ins_wide.c \
	$(srcdir)/inserts.c \
	$(srcdir)/keynames.c \
	$(srcdir)/knight.c \
	$(srcdir)/lrtest.c \
	$(srcdir)/ncurses.c \
	$(srcdir)/newdemo.c \
	$(srcdir)/railroad.c \
	$(srcdir)/rain.c \
	$(srcdir)/tclock.c \
	$(srcdir)/testaddch.c \
	$(srcdir)/testcurs.c \
	$(srcdir)/testscanw.c \
	$(srcdir)/view.c \
	$(srcdir)/worm.c \
	$(srcdir)/xmas.c

lintlib :
	@echo no action needed

# generated by mk-1st.awk (subset=termlib+ext_tinfo+base+ext_funcs)

NORMAL_OBJS = \
	../objects/background$o \
	../objects/blue$o \
	../objects/bs$o \
	../objects/cardfile$o \
	../objects/color_set$o \
	../objects/demo_defkey$o \
	../objects/demo_keyok$o \
	../objects/demo_forms$o \
	../objects/demo_menus$o \
	../objects/demo_panels$o \
	../objects/ditto$o \
	../objects/dots$o \
	../objects/edit_field$o \
	../objects/filter$o \
	../objects/firework$o \
	../objects/firstlast$o \
	../objects/gdc$o \
	../objects/hanoi$o \
	../objects/hashtest$o \
	../objects/ins_wide$o \
	../objects/inserts$o \
	../objects/keynames$o \
	../objects/knight$o \
	../objects/lrtest$o \
	../objects/ncurses$o \
	../objects/newdemo$o \
	../objects/railroad$o \
	../objects/rain$o \
	../objects/tclock$o \
	../objects/testaddch$o \
	../objects/testcurs$o \
	../objects/testscanw$o \
	../objects/view$o \
	../objects/worm$o \
	../objects/xmas$o

$(NORMAL_OBJS) : ../include/ncurses_cfg.h ./test.priv.h

mostlyclean::
	-rm -f $(NORMAL_OBJS)

clean ::
	-rm -f $(NORMAL_OBJS)

# generated by mk-2nd.awk


../objects/background$o :	$(srcdir)/background.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/background.c

../objects/blue$o :	$(srcdir)/blue.c \
			../include/term.h
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/blue.c

../objects/bs$o :	$(srcdir)/bs.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/bs.c

../objects/cardfile$o :	$(srcdir)/cardfile.c \
			../include/panel.h \
			../include/form.h
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/cardfile.c

../objects/color_set$o :	$(srcdir)/color_set.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/color_set.c

../objects/demo_defkey$o :	$(srcdir)/demo_defkey.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/demo_defkey.c

../objects/demo_keyok$o :	$(srcdir)/demo_keyok.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/demo_keyok.c

../objects/demo_forms$o :	$(srcdir)/demo_forms.c \
			../include/form.h
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/demo_forms.c

../objects/demo_menus$o :	$(srcdir)/demo_menus.c \
			../include/menu.h
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/demo_menus.c

../objects/demo_panels$o :	$(srcdir)/demo_panels.c \
			../include/panel.h
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/demo_panels.c

../objects/ditto$o :	$(srcdir)/ditto.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/ditto.c

../objects/dots$o :	$(srcdir)/dots.c \
			../include/term.h
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/dots.c

../objects/edit_field$o :	$(srcdir)/edit_field.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/edit_field.c

../objects/filter$o :	$(srcdir)/filter.c \
			../include/form.h
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/filter.c

../objects/firework$o :	$(srcdir)/firework.c \
			../include/term.h
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/firework.c

../objects/firstlast$o :	$(srcdir)/firstlast.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/firstlast.c

../objects/gdc$o :	$(srcdir)/gdc.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/gdc.c

../objects/hanoi$o :	$(srcdir)/hanoi.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/hanoi.c

../objects/hashtest$o :	$(srcdir)/hashtest.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/hashtest.c

../objects/ins_wide$o :	$(srcdir)/ins_wide.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/ins_wide.c

../objects/inserts$o :	$(srcdir)/inserts.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/inserts.c

../objects/keynames$o :	$(srcdir)/keynames.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/keynames.c

../objects/knight$o :	$(srcdir)/knight.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/knight.c

../objects/lrtest$o :	$(srcdir)/lrtest.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/lrtest.c

../objects/ncurses$o :	$(srcdir)/ncurses.c \
			../include/panel.h \
			../include/menu.h \
			../include/form.h
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/ncurses.c

../objects/newdemo$o :	$(srcdir)/newdemo.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/newdemo.c

../objects/railroad$o :	$(srcdir)/railroad.c \
			../include/termcap.h
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/railroad.c

../objects/rain$o :	$(srcdir)/rain.c \
			../include/term.h
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/rain.c

../objects/tclock$o :	$(srcdir)/tclock.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/tclock.c

../objects/testaddch$o :	$(srcdir)/testaddch.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/testaddch.c

../objects/testcurs$o :	$(srcdir)/testcurs.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/testcurs.c

../objects/testscanw$o :	$(srcdir)/testscanw.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/testscanw.c

../objects/view$o :	$(srcdir)/view.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/view.c

../objects/worm$o :	$(srcdir)/worm.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/worm.c

../objects/xmas$o :	$(srcdir)/xmas.c
	cd ../objects; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_NORMAL) -c ../test/xmas.c


# generated by mk-1st.awk (subset=termlib+ext_tinfo+base+ext_funcs)

DEBUG_OBJS = \
	../obj_g/background$o \
	../obj_g/blue$o \
	../obj_g/bs$o \
	../obj_g/cardfile$o \
	../obj_g/color_set$o \
	../obj_g/demo_defkey$o \
	../obj_g/demo_keyok$o \
	../obj_g/demo_forms$o \
	../obj_g/demo_menus$o \
	../obj_g/demo_panels$o \
	../obj_g/ditto$o \
	../obj_g/dots$o \
	../obj_g/edit_field$o \
	../obj_g/filter$o \
	../obj_g/firework$o \
	../obj_g/firstlast$o \
	../obj_g/gdc$o \
	../obj_g/hanoi$o \
	../obj_g/hashtest$o \
	../obj_g/ins_wide$o \
	../obj_g/inserts$o \
	../obj_g/keynames$o \
	../obj_g/knight$o \
	../obj_g/lrtest$o \
	../obj_g/ncurses$o \
	../obj_g/newdemo$o \
	../obj_g/railroad$o \
	../obj_g/rain$o \
	../obj_g/tclock$o \
	../obj_g/testaddch$o \
	../obj_g/testcurs$o \
	../obj_g/testscanw$o \
	../obj_g/view$o \
	../obj_g/worm$o \
	../obj_g/xmas$o

$(DEBUG_OBJS) : ../include/ncurses_cfg.h ./test.priv.h

mostlyclean::
	-rm -f $(DEBUG_OBJS)

clean ::
	-rm -f $(DEBUG_OBJS)

# generated by mk-2nd.awk


../obj_g/background$o :	$(srcdir)/background.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/background.c

../obj_g/blue$o :	$(srcdir)/blue.c \
			../include/term.h
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/blue.c

../obj_g/bs$o :	$(srcdir)/bs.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/bs.c

../obj_g/cardfile$o :	$(srcdir)/cardfile.c \
			../include/panel.h \
			../include/form.h
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/cardfile.c

../obj_g/color_set$o :	$(srcdir)/color_set.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/color_set.c

../obj_g/demo_defkey$o :	$(srcdir)/demo_defkey.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/demo_defkey.c

../obj_g/demo_keyok$o :	$(srcdir)/demo_keyok.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/demo_keyok.c

../obj_g/demo_forms$o :	$(srcdir)/demo_forms.c \
			../include/form.h
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/demo_forms.c

../obj_g/demo_menus$o :	$(srcdir)/demo_menus.c \
			../include/menu.h
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/demo_menus.c

../obj_g/demo_panels$o :	$(srcdir)/demo_panels.c \
			../include/panel.h
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/demo_panels.c

../obj_g/ditto$o :	$(srcdir)/ditto.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/ditto.c

../obj_g/dots$o :	$(srcdir)/dots.c \
			../include/term.h
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/dots.c

../obj_g/edit_field$o :	$(srcdir)/edit_field.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/edit_field.c

../obj_g/filter$o :	$(srcdir)/filter.c \
			../include/form.h
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/filter.c

../obj_g/firework$o :	$(srcdir)/firework.c \
			../include/term.h
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/firework.c

../obj_g/firstlast$o :	$(srcdir)/firstlast.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/firstlast.c

../obj_g/gdc$o :	$(srcdir)/gdc.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/gdc.c

../obj_g/hanoi$o :	$(srcdir)/hanoi.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/hanoi.c

../obj_g/hashtest$o :	$(srcdir)/hashtest.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/hashtest.c

../obj_g/ins_wide$o :	$(srcdir)/ins_wide.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/ins_wide.c

../obj_g/inserts$o :	$(srcdir)/inserts.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/inserts.c

../obj_g/keynames$o :	$(srcdir)/keynames.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/keynames.c

../obj_g/knight$o :	$(srcdir)/knight.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/knight.c

../obj_g/lrtest$o :	$(srcdir)/lrtest.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/lrtest.c

../obj_g/ncurses$o :	$(srcdir)/ncurses.c \
			../include/panel.h \
			../include/menu.h \
			../include/form.h
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/ncurses.c

../obj_g/newdemo$o :	$(srcdir)/newdemo.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/newdemo.c

../obj_g/railroad$o :	$(srcdir)/railroad.c \
			../include/termcap.h
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/railroad.c

../obj_g/rain$o :	$(srcdir)/rain.c \
			../include/term.h
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/rain.c

../obj_g/tclock$o :	$(srcdir)/tclock.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/tclock.c

../obj_g/testaddch$o :	$(srcdir)/testaddch.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/testaddch.c

../obj_g/testcurs$o :	$(srcdir)/testcurs.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/testcurs.c

../obj_g/testscanw$o :	$(srcdir)/testscanw.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/testscanw.c

../obj_g/view$o :	$(srcdir)/view.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/view.c

../obj_g/worm$o :	$(srcdir)/worm.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/worm.c

../obj_g/xmas$o :	$(srcdir)/xmas.c
	cd ../obj_g; $(LIBTOOL_COMPILE) $(CC) $(CFLAGS_DEBUG) -c ../test/xmas.c

depend : $(AUTO_SRC)
	makedepend -- $(CPPFLAGS) -- $(C_SRC)

# DO NOT DELETE THIS LINE -- make depend depends on it.
