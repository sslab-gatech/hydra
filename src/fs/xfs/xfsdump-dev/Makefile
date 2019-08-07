#
# Copyright (c) 2000-2006 Silicon Graphics, Inc.  All Rights Reserved.
#

ifeq ("$(origin V)", "command line")
  BUILD_VERBOSE = $(V)
endif
ifndef BUILD_VERBOSE
  BUILD_VERBOSE = 0
endif

ifeq ($(BUILD_VERBOSE),1)
  Q =
else
  Q = @
endif

MAKEOPTS = --no-print-directory Q=$(Q)

TOPDIR = .
HAVE_BUILDDEFS = $(shell test -f $(TOPDIR)/include/builddefs && echo yes || echo no)

ifeq ($(HAVE_BUILDDEFS), yes)
include $(TOPDIR)/include/builddefs
endif

SRCDIR = $(PKG_NAME)-$(PKG_VERSION)
SRCTAR = $(PKG_NAME)-$(PKG_VERSION).tar.gz

CONFIGURE = aclocal.m4 configure config.guess config.sub install-sh ltmain.sh
LSRCFILES = configure.ac release.sh README VERSION $(CONFIGURE)
SRCTARINC = m4/libtool.m4 m4/lt~obsolete.m4 m4/ltoptions.m4 m4/ltsugar.m4 \
            m4/ltversion.m4 .gitcensus $(CONFIGURE)

LDIRT = config.log .ltdep .dep config.status config.cache confdefs.h \
	conftest* built .census install.* install-dev.* *.gz \
	autom4te.cache/* libtool include/builddefs include/config.h

ifeq ($(HAVE_BUILDDEFS), yes)
LDIRDIRT = $(SRCDIR)
LDIRT += $(SRCTAR)
endif

LIB_SUBDIRS = librmt
TOOL_SUBDIRS = common inventory invutil dump restore m4 man doc po debian

SUBDIRS = include $(LIB_SUBDIRS) $(TOOL_SUBDIRS)

default: include/builddefs include/config.h
ifeq ($(HAVE_BUILDDEFS), no)
	$(Q)$(MAKE) $(MAKEOPTS) -C . $@
else
	$(Q)$(MAKE) $(MAKEOPTS) $(SUBDIRS)
endif

# tool/lib dependencies
$(LIB_SUBDIRS) $(TOOL_SUBDIRS): include
invutil dump restore: librmt

ifeq ($(HAVE_BUILDDEFS), yes)
include $(BUILDRULES)
else
clean:	# if configure hasn't run, nothing to clean
endif

# Recent versions of libtool require the -i option for copying auxiliary
# files (config.sub, config.guess, install-sh, ltmain.sh), while older
# versions will copy those files anyway, and don't understand -i.
LIBTOOLIZE_INSTALL = `libtoolize -n -i >/dev/null 2>/dev/null && echo -i`

configure:
	libtoolize -c $(LIBTOOLIZE_INSTALL) -f
	cp include/install-sh .
	aclocal -I m4
	autoconf

include/builddefs: configure
	./configure $$LOCAL_CONFIGURE_OPTIONS

include/config.h: include/builddefs
## Recover from the removal of $@
	@if test -f $@; then :; else \
		rm -f include/builddefs; \
		$(MAKE) $(AM_MAKEFLAGS) include/builddefs; \
	fi

install: default $(addsuffix -install,$(SUBDIRS))
	$(INSTALL) -m 755 -d $(PKG_DOC_DIR)
	$(INSTALL) -m 644 README $(PKG_DOC_DIR)

install-dev: default $(addsuffix -install-dev,$(SUBDIRS))

%-install:
	$(Q)$(MAKE) -C $* install

%-install-dev:
	$(Q)$(MAKE) -C $* install-dev

distclean: clean
	rm -f $(LDIRT)

realclean: distclean
	rm -f $(CONFIGURE) .gitcensus

#
# All this gunk is to allow for a make dist on an unconfigured tree
#
dist: include/builddefs include/config.h default
ifeq ($(HAVE_BUILDDEFS), no)
	$(Q)$(MAKE) $(MAKEOPTS) -C . $@
else
	$(Q)$(MAKE) $(MAKEOPTS) $(SRCTAR)
endif

deb: include/builddefs include/config.h
ifeq ($(HAVE_BUILDDEFS), no)
	$(MAKE) $(MAKEOPTS) -C . $@
else
	$(Q)$(MAKE) $(MAKEOPTS) $(SRCDIR)
	$(Q)cd $(SRCDIR) && dpkg-buildpackage
endif

$(SRCDIR) : $(_FORCE) $(SRCTAR)
	rm -fr $@
	$(Q)$(TAR) -zxvf $(SRCTAR)

$(SRCTAR) : default $(SRCTARINC) .gitcensus
	$(Q)$(TAR) --transform "s,^,$(SRCDIR)/," -zcf $(SRCDIR).tar.gz \
	  `cat .gitcensus` $(SRCTARINC)
	echo Wrote: $@

.gitcensus: $(_FORCE)
	$(Q) if test -d .git; then \
	  git ls-files > .gitcensus && echo "new .gitcensus"; \
	fi
