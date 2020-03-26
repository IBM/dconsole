# myincludesubdir = $(pkgincludedir)/dconsole
# 
# pkginclude_HEADERS = $(top_srcdir)/src/dcnsl.h $(top_srcdir)/src/ddiag.h
pkginclude_HEADERS = dcnsl.h ddiag.h pfsd_list.h

bin_PROGRAMS = dconsole
BUILT_SOURCES = dconsole.h 
dconsole_SOURCES = banner.c dconsole.y dcnsl_a.l ddispatcher.c  dhelp.h console.c  ddiag.h  dcnsl.h dhelp.c dsrand.c

AM_LDFLAGS = -ludev -ljson-c -lreadline -ldl
AM_YFLAGS = -d
AM_LFLAGS = -L -I -i
