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
 * complete command string, to know how to parse. This file's purpose is
 * to contain that command string. It also abstracts the option letters,
 * facilitating easy changes.
 */

#define GETOPT_CMDSTRING	"ab:c:d:ef:hl:mop:qs:t:v:z:AB:CDEFG:H:I:JKL:M:NO:PRSTUVWY:"

#define GETOPT_DUMPASOFFLINE	'a'	/* dump DMF dualstate files as offline */
#define	GETOPT_BLOCKSIZE	'b'	/* blocksize for rmt */
#define	GETOPT_ALERTPROG	'c'	/* Media Change Alert prog(content.c) */
#define	GETOPT_FILESZ		'd'	/* Media file size to use in Mb */
#define GETOPT_EXCLUDEFILES	'e'	/* allow files to be excluded */
#define	GETOPT_DUMPDEST		'f'	/* dump dest. file (drive.c) */
/*				'g'	*/
#define	GETOPT_HELP		'h'	/* display version and usage */
/*				'i'	*/
/*				'j'	*/
/*				'k'	*/
#define	GETOPT_LEVEL		'l'	/* dump level (content_inode.c) */
#define GETOPT_MINRMT		'm'	/* use minimal rmt protocol */
/*				'n'	*/
#define GETOPT_OVERWRITE	'o'	/* overwrite data on tape */
#define GETOPT_PROGRESS		'p'	/* interval between progress reports */
#define	GETOPT_QIC		'q'	/* option to tell dump it's a QIC tape */
/*				'r'	*/
#define	GETOPT_SUBTREE		's'	/* subtree dump (content_inode.c) */
#define GETOPT_DUMPTIME		't'	/* use mtime of file as dump time */
/*				'u' */
#define	GETOPT_VERBOSITY	'v'	/* verbosity level (0 to 4 ) */
/*				'w' */
/*				'x'	   used in irix for xvm snapshot */
/*				'y' */
#define GETOPT_MAXDUMPFILESIZE	'z'	/* prune files over specified size */
#define	GETOPT_NOEXTATTR	'A'	/* do not dump ext. file attributes */
#define	GETOPT_BASED		'B'	/* specify session to base increment */
#define GETOPT_RECCHKSUM	'C'	/* use record checksums */
#define GETOPT_NOUNCHANGEDDIRS	'D'	/* skip unchanged directories */
#define	GETOPT_ERASE		'E'	/* pre-erase media */
#define GETOPT_FORCE		'F'	/* don't prompt (getopt.c) */
#define GETOPT_MINSTACKSZ	'G'	/* minimum stack size (bytes) */
#define GETOPT_MAXSTACKSZ	'H'	/* maximum stack size (bytes) */
#define GETOPT_INVPRINT         'I'     /* just display the inventory */
#define	GETOPT_NOINVUPDATE	'J'	/* do not update the dump inventory */
#define GETOPT_FMT2COMPAT	'K'	/* use dump format 2 for compat with old restore */
#define	GETOPT_DUMPLABEL	'L'	/* dump session label (global.c) */
#define	GETOPT_MEDIALABEL	'M'	/* media object label (media.c) */
#define	GETOPT_TIMESTAMP	'N'	/* show timestamps in log msgs */
#define	GETOPT_OPTFILE		'O'	/* specifycmd line options file */
#define	GETOPT_RINGPIN		'P'	/* pin down I/O buffer ring */
/*				'Q'	*/
#define	GETOPT_RESUME		'R'	/* resume intr dump (content_inode.c) */
#define	GETOPT_SINGLEMFILE	'S'	/* obsolete - now the default */
#define	GETOPT_NOTIMEOUTS	'T'	/* don't timeout dialogs */
#define	GETOPT_UNLOAD		'U'	/* unload media when change needed */
#define	GETOPT_SHOWLOGSS	'V'	/* show subsystem of log messages */
#define	GETOPT_SHOWLOGLEVEL	'W'	/* show level of log messages */
/*				'X'	*/
#define	GETOPT_RINGLEN		'Y'	/* specify I/O buffer ring length */
/*				'Z'	   used on irix for miniroot */

#endif /* GETOPT_H */
