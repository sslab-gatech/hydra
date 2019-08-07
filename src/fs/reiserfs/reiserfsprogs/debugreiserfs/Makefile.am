sbin_PROGRAMS = debugreiserfs

debugreiserfs_SOURCES = debugreiserfs.c pack.c unpack.c stat.c corruption.c scan.c recover.c debugreiserfs.h
man_MANS = debugreiserfs.8
EXTRA_DIST = $(man_MANS)

LDADD = $(top_builddir)/reiserfscore/libreiserfscore.la

install-exec-hook:
	$(LN_S) debugreiserfs $(DESTDIR)$(sbindir)/debugfs.reiserfs
	$(INSTALL) -m 755 -d $(DESTDIR)$(mandir)/man8
	$(LN_S) debugreiserfs.8 $(DESTDIR)$(mandir)/man8/debugfs.reiserfs.8
