/*
 * Copyright (c) 2000-2001 Silicon Graphics, Inc.
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

/* getopt.h	common getopt  command string
 *
 * several modules parse the command line looking for arguments specific to
 * that module. Unfortunately, each of the getopt(3) calls needs the
 * complete command string, to know how to parse. This file's sole
 * purpose is to contain that command string.
 */

#define GETOPT_CMDSTRING	"a:b:c:def:himn:op:qrs:tv:wABCDEFG:H:I:JKL:M:NO:PQRS:TUVWX:Y:"

#define GETOPT_WORKSPACE	'a'	/* workspace dir (content.c) */
#define GETOPT_BLOCKSIZE        'b'     /* blocksize for rmt */
#define GETOPT_ALERTPROG        'c'     /* Media Change Alert prog(content.c) */
#define GETOPT_PV766024		'd'	/* DRIVE_ERROR_MEDIA for pv#766024 */
#define	GETOPT_EXISTING		'e'	/* don't overwrite existing files */
#define	GETOPT_DUMPDEST		'f'	/* dump src. file (drive.c) */
/*				'g' */
#define	GETOPT_HELP		'h'	/* display version and usage */
#define	GETOPT_INTERACTIVE	'i'	/* interactive subtree selection */
/*				'j' */
/*				'k' */
/*				'l' */
#define GETOPT_MINRMT		'm'	/* use minimal rmt protocol */
#define	GETOPT_NEWER		'n'	/* only restore files newer than arg */
#define	GETOPT_OWNER		'o'	/* restore owner/grp even if not root */
#define GETOPT_PROGRESS		'p'	/* interval between progress reports */
#define	GETOPT_QIC		'q'	/* option to tell dump it's a QIC tape */
#define	GETOPT_CUMULATIVE	'r'	/* accumulating restore (content.c) */
#define	GETOPT_SUBTREE		's'	/* subtree restore (content.c) */
#define	GETOPT_TOC		't'	/* display contents only (content.c) */
/*				'u' */
#define	GETOPT_VERBOSITY	'v'	/* verbosity level (0 to 4 ) */
#define	GETOPT_SMALLWINDOW	'w'	/* use a small window for dir entries */
/*				'x' */
/*				'y' */
/*				'z' */
#define	GETOPT_NOEXTATTR	'A'	/* do not restore ext. file attr. */
#define GETOPT_ROOTPERM		'B'	/* restore ownership and permissions for root directory */
#define GETOPT_RECCHKSUM	'C'	/* use record checksums */
#define GETOPT_SETDM		'D'	/* set DMAPI event mask and state */
#define	GETOPT_CHANGED		'E'	/* overwrite if missing or old */
#define GETOPT_FORCE		'F'	/* don't prompt (getopt.c) */
#define GETOPT_MINSTACKSZ	'G'	/* minimum stack size (bytes) */
#define GETOPT_MAXSTACKSZ	'H'	/* maximum stack size (bytes) */
#define GETOPT_INVPRINT         'I'     /* just display the inventory */
#define	GETOPT_NOINVUPDATE	'J'	/* do not update the dump inventory */
#define GETOPT_FMT2COMPAT	'K'	/* force use format 2 gen numbers */
#define	GETOPT_DUMPLABEL	'L'	/* dump session label (global.c) */
#define	GETOPT_MEDIALABEL	'M'	/* media object label (media.c) */
#define	GETOPT_TIMESTAMP	'N'	/* show timestamps in log msgs */
#define	GETOPT_OPTFILE		'O'	/* specifycmd line options file */
#define	GETOPT_RINGPIN		'P'	/* pin down I/O buffer ring */
#define GETOPT_SESSCPLT		'Q'	/* force completion of intr. session */
#define	GETOPT_RESUME		'R'	/* resume intr rest (content.c) */
#define	GETOPT_SESSIONID	'S'	/* dump session uuid (global.c) */
#define	GETOPT_NOTIMEOUTS	'T'	/* don't timeout dialogs */
#define	GETOPT_UNLOAD		'U'	/* unload media when change needed */
#define	GETOPT_SHOWLOGSS	'V'	/* show subsystem of log messages */
#define	GETOPT_SHOWLOGLEVEL	'W'	/* show level of log messages */
#define	GETOPT_NOSUBTREE	'X'	/* subtree to exclude */
#define	GETOPT_RINGLEN		'Y'	/* specify I/O buffer ring length */
/*				'Z'	   used on irix for miniroot */

#endif /* GETOPT_H */
