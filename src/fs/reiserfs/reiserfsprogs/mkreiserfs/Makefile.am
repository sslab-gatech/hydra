sbin_PROGRAMS = mkreiserfs

mkreiserfs_SOURCES = mkreiserfs.c
man_MANS = mkreiserfs.8
EXTRA_DIST = $(man_MANS)

LDADD = $(top_builddir)/reiserfscore/libreiserfscore.la

install-exec-hook:
	$(LN_S) mkreiserfs $(DESTDIR)$(sbindir)/mkfs.reiserfs
	$(INSTALL) -m 755 -d $(DESTDIR)$(mandir)/man8
	$(LN_S) mkreiserfs.8 $(DESTDIR)$(mandir)/man8/mkfs.reiserfs.8
