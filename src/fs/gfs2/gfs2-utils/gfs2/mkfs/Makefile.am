MAINTAINERCLEANFILES = Makefile.in

COMMON_CPPFLAGS = \
	-D_FILE_OFFSET_BITS=64 \
	-D_LARGEFILE64_SOURCE \
	-D_GNU_SOURCE \
	-I$(top_srcdir)/gfs2/include \
	-I$(top_srcdir)/gfs2/libgfs2

sbin_PROGRAMS = \
	mkfs.gfs2 \
	gfs2_jadd \
	gfs2_grow

noinst_HEADERS = \
	gfs2_mkfs.h \
	metafs.h

mkfs_gfs2_SOURCES = \
	main_mkfs.c \
	progress.c \
	progress.h

mkfs_gfs2_CPPFLAGS = $(COMMON_CPPFLAGS)
mkfs_gfs2_CFLAGS = \
	$(blkid_CFLAGS) \
	$(uuid_CFLAGS)
mkfs_gfs2_LDFLAGS = \
	$(blkid_LIBS) \
	$(uuid_LIBS)
mkfs_gfs2_LDADD	= $(top_builddir)/gfs2/libgfs2/libgfs2.la

gfs2_grow_SOURCES = \
	main_grow.c \
	metafs.c

gfs2_grow_CPPFLAGS = $(COMMON_CPPFLAGS)
gfs2_grow_CFLAGS = $(blkid_CFLAGS)
gfs2_grow_LDFLAGS = \
	$(blkid_LIBS) \
	$(uuid_LIBS)
gfs2_grow_LDADD = $(top_builddir)/gfs2/libgfs2/libgfs2.la

gfs2_jadd_SOURCES = \
	main_jadd.c \
	metafs.c

gfs2_jadd_CPPFLAGS = $(COMMON_CPPFLAGS)
gfs2_jadd_LDADD = $(top_builddir)/gfs2/libgfs2/libgfs2.la
gfs2_jadd_LDFLAGS = $(uuid_LIBS)

