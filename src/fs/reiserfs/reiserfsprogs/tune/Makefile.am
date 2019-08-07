sbin_PROGRAMS = reiserfstune

reiserfstune_SOURCES = tune.c tune.h

man_MANS = reiserfstune.8
EXTRA_DIST = $(man_MANS)

LDADD = $(top_builddir)/reiserfscore/libreiserfscore.la

install-exec-hook:
	$(LN_S) reiserfstune $(DESTDIR)$(sbindir)/tunefs.reiserfs
	$(INSTALL) -m 755 -d $(DESTDIR)$(mandir)/man8
	$(LN_S) reiserfstune.8 $(DESTDIR)$(mandir)/man8/tunefs.reiserfs.8
