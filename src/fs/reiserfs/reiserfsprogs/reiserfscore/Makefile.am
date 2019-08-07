lib_LTLIBRARIES = libreiserfscore.la

reiserfsdir = $(includedir)/reiserfs
reiserfs_HEADERS = reiserfs_err.h

reiserfs_err.c reiserfs_err.h: reiserfs_err.et
	$(RM) -f reisefs_err.c reiserfs_err.h
	compile_et reiserfs_err.et
reiserfs_err.h: reiserfs_err.c

BUILT_SOURCES = reiserfs_err.h reiserfs_err.c
CLEANFILES = reiserfs_err.h reiserfs_err.c
EXTRA_DIST = reiserfs_err.et

libreiserfscore_la_SOURCES = do_balan.c fix_node.c hashes.c ibalance.c \
			     lbalance.c prints.c stree.c node_formats.c \
			     reiserfslib.c bitmap.c journal.c xattr.c \
			     includes.h reiserfs_err.c
libreiserfscore_la_LIBADD = ../lib/libmisc.la -lcom_err

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = reiserfscore.pc
