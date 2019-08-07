MAINTAINERCLEANFILES = Makefile.in

sbin_PROGRAMS = fsck.gfs2

noinst_HEADERS = \
	afterpass1_common.h \
	fsck.h \
	fs_recovery.h \
	inode_hash.h \
	link.h \
	lost_n_found.h \
	metawalk.h \
	util.h

fsck_gfs2_SOURCES = \
	fs_recovery.c \
	initialize.c \
	inode_hash.c \
	link.c \
	lost_n_found.c \
	main.c \
	metawalk.c \
	afterpass1_common.c \
	pass1b.c \
	pass1.c \
	pass2.c \
	pass3.c \
	pass4.c \
	pass5.c \
	rgrepair.c \
	util.c

fsck_gfs2_CPPFLAGS = \
	-D_FILE_OFFSET_BITS=64 \
	-I$(top_srcdir)/gfs2/include \
	-I$(top_srcdir)/gfs2/libgfs2

fsck_gfs2_LDADD = \
	$(top_builddir)/gfs2/libgfs2/libgfs2.la
fsck_gfs2_LDFLAGS = \
	$(uuid_LIBS)
