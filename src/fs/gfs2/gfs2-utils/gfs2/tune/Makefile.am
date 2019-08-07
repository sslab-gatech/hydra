MAINTAINERCLEANFILES = Makefile.in

sbin_PROGRAMS = tunegfs2

noinst_HEADERS = tunegfs2.h

tunegfs2_SOURCES = \
	main.c \
	super.c
tunegfs2_CPPFLAGS = \
	-D_FILE_OFFSET_BITS=64 \
	-I$(top_srcdir)/gfs2/include \
	-I$(top_srcdir)/group/include
tunegfs2_CFLAGS = \
	$(uuid_CFLAGS)
tunegfs2_LDFLAGS = \
	$(uuid_LIBS)
