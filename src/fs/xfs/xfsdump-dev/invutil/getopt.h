/*
 * Copyright (c) 2000-2002 Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write the Free Software Foundation,
 * Inc.,  51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef GETOPT_H
#define GETOPT_H

#define GETOPT_CMDSTRING	"dilnu:wCFM:m:s:"

#define GETOPT_DEBUG		'd'	/* debug */
#define GETOPT_INTERACTIVE	'i'	/* interactive mode */
#define GETOPT_NONINTERACTIVE	'n'	/* non interactive mode - obsoleted by -F */
#define GETOPT_UUID		'u'	/* prune uuid */
#define GETOPT_WAITFORLOCKS	'w'	/* wait for locks */
#define GETOPT_CHECKPRUNEFSTAB	'C'	/* check and prune fstab */
#define GETOPT_FORCE		'F'	/* force - do not ask for confirmation */
#define GETOPT_PRUNEMNT		'M'	/* prune mount point */
#define GETOPT_PRUNEMEDIALABEL	'm'	/* prune media label */
#define GETOPT_PRUNESESSION	's'	/* prune session id */

#endif /* GETOPT_H */
