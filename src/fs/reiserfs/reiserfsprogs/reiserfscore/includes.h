/*
 * Copyright 2000-2004 by Hans Reiser, licensing governed by
 * reiserfsprogs/README
 */

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include "io.h"
#include "misc.h"
#include "reiserfs_lib.h"
#include "reiserfs_err.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <asm/types.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys/vfs.h>
#include <time.h>
