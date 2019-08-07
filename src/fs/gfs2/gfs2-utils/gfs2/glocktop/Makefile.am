MAINTAINERCLEANFILES = Makefile.in

sbin_PROGRAMS = \
	glocktop

glocktop_SOURCES = \
	glocktop.c

glocktop_CFLAGS = \
	$(ncurses_CFLAGS)

glocktop_LDFLAGS = \
	$(ncurses_LIBS) \
	$(uuid_LIBS)

glocktop_CPPFLAGS = \
	-D_FILE_OFFSET_BITS=64 \
	-D_LARGEFILE64_SOURCE \
	-D_GNU_SOURCE \
	-I$(top_srcdir)/gfs2/include \
	-I$(top_srcdir)/gfs2/libgfs2

glocktop_LDADD = \
	$(top_builddir)/gfs2/libgfs2/libgfs2.la
