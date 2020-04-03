# myincludesubdir = $(pkgincludedir)/dconsole
# 
# pkginclude_HEADERS = $(top_srcdir)/src/dcnsl.h $(top_srcdir)/src/ddiag.h
pkginclude_HEADERS = dcnsl.h ddiag.h dcnsl_list.h

bin_PROGRAMS = dconsole
BUILT_SOURCES = dconsole.h 
dconsole_SOURCES = banner.c dconsole.y dcnsl_a.l ddispatcher.c  dhelp.h console.c  ddiag.h  dcnsl.h dhelp.c dsrand.c dlogger.c

AM_LDFLAGS = -ludev -ljson-c -lreadline -lglib-2.0 -ldl
AM_YFLAGS = -d
AM_LFLAGS = -L -I -i
dconsole_LDFLAGS = $(GLIB_LIBS)
dconsole_CFLAGS = $(GLIB_CFLAGS)
