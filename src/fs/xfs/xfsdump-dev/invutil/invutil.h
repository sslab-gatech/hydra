/*
 * Copyright (c) 2002 Silicon Graphics, Inc.
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
#ifndef INVUTIL_H
#define INVUTIL_H

#define STR_LEN			INV_STRLEN+32
#define GEN_STRLEN		32

/*      open_and_lock() return codes */
#define SYSCALL_FAILED  (-1)
#define LOCK_BUSY       (-2)

typedef enum {
    FILE_READ,
    FILE_WRITE,
    FILE_UNSAFE,
    DIRECTORY_READ,
    DIRECTORY_WRITE,
    DIRECTORY_UNSAFE
} Open_t;

extern char *	g_programName;
extern char *	g_programVersion;
extern char *	inventory_path;
extern bool_t	debug;
extern bool_t	force;
extern bool_t	wait_for_locks;

char *	GetFstabFullPath(char *);
char *	GetNameOfInvIndex (char *, uuid_t);
char *	GetNameOfStobj (char *inv_path, char *filename);
void	CheckAndPruneFstab(
		char *, bool_t, char *, uuid_t *, uuid_t *, time32_t, char *);
int	CheckAndPruneInvIndexFile( bool_t, char *, uuid_t *, time32_t, char *);
int	CheckAndPruneStObjFile( bool_t, char *, uuid_t *, time32_t, char *);
int	uses_specified_mf_label(
		invt_seshdr_t *, invt_session_t *, char	*, char *);
time32_t ParseDate(char *);
void	usage (void);
int	open_and_lock(char *, Open_t, uint);
void	read_n_bytes(int, void *, size_t, char *);
void	write_n_bytes(int, void *, size_t, char *);
void *	mmap_n_bytes(int, size_t, bool_t, char *);
void	ListFstab(void);
int	ListInvIndexFile( char *);
int	ListStObjFile(char *);
int	invutil_interactive(char *, char *, uuid_t *, time32_t timeSecs);
int	mntpnt_equal(char *s1, char *s2);

#endif /* INVUTIL_H */
