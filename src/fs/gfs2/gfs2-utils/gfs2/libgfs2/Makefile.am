MAINTAINERCLEANFILES = Makefile.in

CLEANFILES = \
	parser.h \
	parser.c \
	lexer.c \
	lexer.h

BUILT_SOURCES = \
	parser.h \
	lexer.h

AM_LFLAGS = --header-file=lexer.h
AM_YFLAGS = -d

noinst_HEADERS = \
	libgfs2.h \
	crc32c.h \
	lang.h \
	config.h \
	rgrp.h

noinst_LTLIBRARIES = libgfs2.la

noinst_PROGRAMS = gfs2l

libgfs2_la_SOURCES = \
	crc32c.c \
	block_list.c \
	fs_bits.c \
	gfs1.c \
	misc.c \
	rgrp.c \
	super.c \
	buf.c \
	fs_geometry.c \
	gfs2_disk_hash.c \
	ondisk.c \
	config.c \
	device_geometry.c \
	fs_ops.c \
	recovery.c \
	structures.c \
	meta.c \
	lang.c \
	parser.y \
	lexer.l

libgfs2_la_CPPFLAGS = \
	-D_FILE_OFFSET_BITS=64 \
	-D_LARGEFILE64_SOURCE \
	-D_GNU_SOURCE \
	-I$(top_srcdir)/gfs2/include \
	$(uuid_CFLAGS)

gfs2l_SOURCES = gfs2l.c
gfs2l_LDADD = libgfs2.la
gfs2l_LDFLAGS = $(uuid_LIBS) \
            $(uuid_CFLAGS)
gfs2l_CPPFLAGS = \
	-I$(top_srcdir)/gfs2/include \
	-D_FILE_OFFSET_BITS=64 \
	$(uuid_CFLAGS)

# Autotools can't handle header files output by flex so we have to generate it manually
lexer.h: lexer.l
	$(LEX) -o lexer.c $(AM_LFLAGS) $^
