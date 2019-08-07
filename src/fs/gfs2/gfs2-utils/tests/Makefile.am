TESTSCRIPTS = \
	rgrifieldscheck.sh \
	rgskipcheck.sh

EXTRA_DIST = \
	$(TESTSUITE_AT) \
	$(TESTSCRIPTS) \
	package.m4 \
	atlocal \
	atconfig

DISTCLEANFILES = \
	atlocal \
	atconfig

CLEANFILES = testvol

noinst_PROGRAMS = nukerg

nukerg_SOURCES = nukerg.c
nukerg_CPPFLAGS = \
	-D_FILE_OFFSET_BITS=64 \
	-D_LARGEFILE64_SOURCE \
	-D_GNU_SOURCE
nukerg_CFLAGS = \
	-I$(top_srcdir)/gfs2/libgfs2 \
	-I$(top_srcdir)/gfs2/include
nukerg_LDADD = \
	$(top_builddir)/gfs2/libgfs2/libgfs2.la
nukerg_LDFLAGS = \
	$(uuid_LIBS)

if HAVE_CHECK
UNIT_TESTS = \
	check_meta \
	check_rgrp
UNIT_SOURCES = \
	$(top_srcdir)/gfs2/libgfs2/libgfs2.h
UNIT_CFLAGS = \
	-I$(top_srcdir)/gfs2/libgfs2 \
        -I$(top_srcdir)/gfs2/include \
	@check_CFLAGS@
UNIT_LDADD = \
	$(top_builddir)/gfs2/libgfs2/libgfs2.la \
	@check_LIBS@
UNIT_CPPFLAGS = \
	-D_FILE_OFFSET_BITS=64 \
	-D_LARGEFILE64_SOURCE \
	-D_GNU_SOURCE

check_PROGRAMS = $(UNIT_TESTS)

check_meta_SOURCES = \
	$(UNIT_SOURCES) \
	check_meta.c
check_meta_CFLAGS = $(UNIT_CFLAGS)
check_meta_LDADD = $(UNIT_LDADD)
check_meta_LDFLAGS = $(uuid_LIBS)
check_meta_CPPFLAGS = $(UNIT_CPPFLAGS)

check_rgrp_SOURCES = \
	$(UNIT_SOURCES) \
	check_rgrp.c
check_rgrp_CFLAGS = $(UNIT_CFLAGS)
check_rgrp_LDADD = $(UNIT_LDADD)
check_rgrp_LDFLAGS = $(uuid_LIBS)
check_rgrp_CPPFLAGS = $(UNIT_CPPFLAGS)
endif

# The `:;' works around a Bash 3.2 bug when the output is not writable.
package.m4: $(top_srcdir)/configure.ac
	:;{ \
		echo '# Signature of the current package.' && \
		echo 'm4_define([AT_PACKAGE_NAME],' && \
		echo '  [$(PACKAGE_NAME)])' && \
		echo 'm4_define([AT_PACKAGE_TARNAME],' && \
		echo '  [$(PACKAGE_TARNAME)])' && \
		echo 'm4_define([AT_PACKAGE_VERSION],' && \
		echo '  [$(PACKAGE_VERSION)])' && \
		echo 'm4_define([AT_PACKAGE_STRING],' && \
		echo '  [$(PACKAGE_STRING)])' && \
		echo 'm4_define([AT_PACKAGE_BUGREPORT],' && \
		echo '  [$(PACKAGE_BUGREPORT)])'; \
		echo 'm4_define([AT_PACKAGE_URL],' && \
		echo '  [$(PACKAGE_URL)])'; \
	} >'$(srcdir)/package.m4'

TESTSUITE_AT = \
	testsuite.at \
	mkfs.at \
	fsck.at \
	edit.at \
	libgfs2.at

if HAVE_CHECK
ENABLE_UNIT=yes
endif

TESTSUITE = testsuite

check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' ENABLE_UNIT_TESTS=$(ENABLE_UNIT) $(TOPTS)

installcheck-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' AUTOTEST_PATH='$(sbindir):gfs2/libgfs2:tests' \
	                        ENABLE_UNIT_TESTS=$(ENABLE_UNIT) $(TOPTS)

clean-local:
	test ! -f '$(TESTSUITE)' || $(SHELL) '$(TESTSUITE)' --clean
	rm -f '$(TESTSUITE)'

atconfig: $(top_builddir)/config.status
	cd $(top_builddir) && ./config.status tests/$@

AUTOM4TE = $(SHELL) $(top_srcdir)/missing --run autom4te
AUTOTEST = $(AUTOM4TE) --language=autotest

$(TESTSUITE): $(TESTSUITE_AT) package.m4 $(UNIT_TESTS)
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@
