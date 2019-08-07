#
# Copyright (c) 2000-2001 Silicon Graphics, Inc.
#

TOPDIR = ..
include $(TOPDIR)/include/builddefs

LTLIBRARY = librmt.la
LTLDFLAGS =	# empty, forces a static-only library build

HFILES = rmtlib.h
CFILES = \
    isrmt.c      rmtclose.c    rmtdev.c    rmtisatty.c  rmtread.c   \
    rmtabort.c   rmtcommand.c  rmtfstat.c  rmtlseek.c   rmtstatus.c \
    rmtaccess.c  rmtcreat.c    rmtioctl.c  rmtopen.c    rmtwrite.c \
    rmtmsg.c

default: ltdepend $(LTLIBRARY)

include $(BUILDRULES)

install install-dev: default

-include .ltdep
