#
# Copyright (c) 2017 Oracle.  All Rights Reserved.
#

TOPDIR = ..
include $(TOPDIR)/include/builddefs

LTLIBRARY = libfrog.la
LT_CURRENT = 0
LT_REVISION = 0
LT_AGE = 0

CFILES = \
avl64.c \
convert.c \
list_sort.c \
paths.c \
projects.c \
ptvar.c \
radix-tree.c \
topology.c \
util.c \
workqueue.c

ifeq ($(HAVE_GETMNTENT),yes)
LCFLAGS += -DHAVE_GETMNTENT
endif

ifeq ($(HAVE_GETMNTINFO),yes)
LCFLAGS += -DHAVE_GETMNTINFO
endif

default: ltdepend $(LTLIBRARY)

include $(BUILDRULES)

install install-dev: default

-include .ltdep
