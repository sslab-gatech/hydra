MAINTAINERCLEANFILES = Makefile.in

sbin_PROGRAMS = gfs2_convert

gfs2_convert_CPPFLAGS = \
	-D_FILE_OFFSET_BITS=64 \
	-I$(top_srcdir)/gfs2/include \
	-I$(top_srcdir)/gfs2/libgfs2

gfs2_convert_LDADD = $(top_builddir)/gfs2/libgfs2/libgfs2.la
gfs2_convert_LDFLAGS = $(uuid_LIBS)
