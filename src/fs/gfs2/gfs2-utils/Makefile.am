EXTRA_DIST = autogen.sh README

AUTOMAKE_OPTIONS = foreign

MAINTAINERCLEANFILES = \
	Makefile.in \
	aclocal.m4 \
	configure \
	depcomp \
	config.guess \
	config.sub \
	missing \
	install-sh \
	autoheader \
	automake \
	autoconf \
	libtool \
	libtoolize \
	ltmain.sh \
	compile \
	make/clusterautoconfig.h.in \
	make/clusterautoconfig.h.in~

noinst_HEADERS = make/copyright.cf

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	po \
	gfs2 \
	doc \
	tests

maintainer-clean-local:
	rm -rf m4
