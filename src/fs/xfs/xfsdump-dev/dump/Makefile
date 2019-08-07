#
# Copyright (c) 2000-2002 Silicon Graphics, Inc.  All Rights Reserved.
#

TOPDIR = ..
include $(TOPDIR)/include/builddefs

COMMINCL = \
	arch_xlate.h \
	cldmgr.h \
	content.h \
	content_common.h \
	content_inode.h \
	dlog.h \
	drive.h \
	exit.h \
	fs.h \
        getdents.h \
	global.h \
	hsmapi.h \
	lock.h \
	media.h \
	mlog.h \
	openutil.h \
	path.h \
	qlock.h \
	ring.h \
	stream.h \
	timeutil.h \
	ts_mtio.h \
	types.h \
	util.h \
	rec_hdr.h

INVINCL = \
	inventory.h \
	inv_priv.h

INVCOMMON = \
	inv_api.c \
	inv_core.c \
	inv_files.c \
	inv_fstab.c \
	inv_idx.c \
	inv_mgr.c \
	inv_stobj.c

COMMON = \
	arch_xlate.c \
	cldmgr.c \
	content_common.c \
	dlog.c \
	drive.c \
	drive_scsitape.c \
	drive_simple.c \
	drive_minrmt.c \
	fs.c \
	getdents.c \
	global.c \
	hsmapi.c \
	lock.c \
	main.c \
	mlog.c \
	openutil.c \
	qlock.c \
	path.c \
	ring.c \
	stream.c \
	timeutil.c \
	util.c

LOCALS = \
	content.c \
	inomap.c \
	var.c

LOCALINCL = \
	getopt.h \
	inomap.h \
	var.h

LTCOMMAND = xfsdump

CFILES = $(LOCALS)
LCFILES = $(COMMON) $(INVCOMMON)
HFILES = $(LOCALINCL)
LHFILES = $(COMMINCL) $(INVINCL)
LINKS = $(COMMINCL) $(COMMON) $(INVINCL) $(INVCOMMON)
LDIRT = $(LINKS)
LLDLIBS = $(LIBUUID) $(LIBHANDLE) $(LIBATTR) $(LIBRMT) $(LIBPTHREAD)
LTDEPENDENCIES = $(LIBRMT)

LCFLAGS = -DDUMP

default: depend $(LTCOMMAND)

include $(BUILDRULES)

install: default
	$(INSTALL) -m 755 -d $(PKG_ROOT_SBIN_DIR)
	$(LTINSTALL) -m 755 $(LTCOMMAND) $(PKG_ROOT_SBIN_DIR)
	$(INSTALL) -m 755 -d $(PKG_SBIN_DIR)
	$(INSTALL) -S $(PKG_ROOT_SBIN_DIR)/$(LTCOMMAND) $(PKG_SBIN_DIR)/$(LTCOMMAND)
install-dev:

.dep: $(COMMINCL) $(COMMON) $(INVINCL) $(INVCOMMON)

$(COMMINCL) $(COMMON):
	@echo "    [LN]     $@"
	$(Q)$(RM) $@; $(LN_S) ../common/$@ $@

$(INVINCL) $(INVCOMMON):
	@echo "    [LN]     $@"
	$(Q)$(RM) $@; $(LN_S) ../inventory/$@ $@

-include .dep
