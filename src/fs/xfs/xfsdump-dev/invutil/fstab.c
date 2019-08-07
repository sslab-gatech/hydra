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

#include <xfs/xfs.h>
#include <xfs/jdm.h>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <ncurses.h>
#include <sys/mman.h>
#include <string.h>
#include <uuid/uuid.h>

#include "types.h"
#include "mlog.h"
#include "inv_priv.h"
#include "getopt.h"

#include "invutil.h"
#include "cmenu.h"
#include "list.h"
#include "invidx.h"
#include "fstab.h"

fstab_fileinfo_t *fstab_file;
int fstab_numfiles;

menu_ops_t fstab_ops = {
    NULL,
    NULL,
    NULL, /*fstab_saveall,*/
    fstab_select,
    NULL,
    NULL,
    NULL,
    NULL,
    fstab_highlight,
    menu_unhighlight,
    fstab_commit,
    fstab_prune,
};

/*ARGSUSED*/
int
fstab_commit(WINDOW *win, node_t *current, node_t *list)
{
    int i;
    int fidx;
    node_t *n;
    data_t *d;
    invt_fstab_t *fstabentry;
    int fstabentry_idx;

    n = current;
    if(n == NULL || n->data == NULL)
	return 0;

    d = ((data_t *)(n->data));
    fidx = d->file_idx;
    fstabentry = fstab_file[fidx].data[d->data_idx];

    if(d->deleted == BOOL_TRUE && d->imported == BOOL_FALSE) {
	for(i = 0; i < d->nbr_children; i++) {
	    invidx_commit(win, d->children[i], list);
	}
	mark_all_children_commited(current);

	fstabentry_idx = (int)(((long)fstabentry - (long)fstab_file[fidx].mapaddr - sizeof(invt_counter_t)) / sizeof(invt_fstab_t));

	if ( fstab_file[fidx].counter->ic_curnum > 1 ) {
	    memmove(fstabentry,
		    fstabentry + 1,
		    (sizeof(invt_fstab_t) * (fstab_file[fidx].counter->ic_curnum - fstabentry_idx - 1)));
	    
	    fstab_file[fidx].data[d->data_idx] = NULL;

	    for(i = d->data_idx + 1; i < fstab_file[fidx].numrecords; i++) {
		if(fstab_file[fidx].data[i] != NULL)
		    fstab_file[fidx].data[i]--;
	    }
	}
	fstab_file[fidx].counter->ic_curnum--;
    }
    else if(d->imported == BOOL_TRUE) {
	invt_fstab_t *dest;

	for(i = 0; i < d->nbr_children; i++) {
	    invidx_commit(win, d->children[i], list);
	}
	mark_all_children_commited(current);

	if(find_matching_fstab(0, fstabentry) >= 0) {
	    return 0;
	}

	if(fstab_file[0].counter->ic_curnum < fstab_file[0].nEntries) {
	    dest = fstab_file[0].data[fstab_file[0].counter->ic_curnum - 1] + 1;
	}
	else {
	    /* expand mmap by one */
	    dest = remmap_fstab(0, 1);
	}

	/* copy new data to mmap */
	memmove(dest, fstabentry, sizeof(*fstabentry));
	
	/* increment counter */
	fstab_file[0].counter->ic_curnum++;
    }

    return 0;
}

/*ARGSUSED*/
int
fstab_select(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return 0;
    }
    d = (data_t *)(current->data);
	
    if(d->expanded == TRUE) {
	return menu_collapse(win, current, list);
    }

    return menu_expand(win, current, list);
}

/*ARGSUSED*/
int
fstab_highlight(WINDOW *win, node_t *current, node_t *list)
{
    static char txt[256];
    char uuidstr[UUID_STR_LEN + 1];
    data_t *d;
    invt_fstab_t *fstabentry;

    if(current == NULL || current->data == NULL) {
	return 0;
    }
    d = (data_t *)(current->data);
    fstabentry = fstab_file[d->file_idx].data[d->data_idx];

    wclear(infowin);

    put_info_header("fstab entry");

    snprintf(txt, sizeof(txt), "device: %s", fstabentry->ft_devpath);
    put_info_line(1, txt);

    uuid_unparse(fstabentry->ft_uuid, uuidstr);
    snprintf(txt, sizeof(txt), "uuid:   %s", uuidstr);
    put_info_line(2, txt);

    return FALSE;
}

/*ARGSUSED*/
int
fstab_prune(char *mountpt, uuid_t *uuidp, time32_t prunetime, node_t *node, node_t *list)
{
    data_t		*d;
    invt_fstab_t	*fstabentry;

    if(node == NULL || node->data == NULL) {
	return BOOL_FALSE;
    }

    d = (data_t *)(node->data);
    fstabentry = fstab_file[d->file_idx].data[d->data_idx];

    if(mountpt != NULL && mntpnt_equal(mountpt, fstabentry->ft_mountpt)) {
	return BOOL_TRUE;
    }
    if((!uuid_is_null(*uuidp))
       && (uuid_compare(*uuidp, fstabentry->ft_uuid) == 0)) {
	return BOOL_TRUE;
    }

    return BOOL_FALSE;
}

int
add_fstab_data(int fidx, invt_fstab_t *data)
{
    if(fstab_file[fidx].numrecords == 0) {
	fstab_file[fidx].data = malloc(sizeof(*fstab_file[fidx].data));
	if(fstab_file[fidx].data == NULL) {
	    fprintf(stderr, "%s: internal memory error: fstab data allocation\n", g_programName);
	    exit(1);
	}
    }
    else {
	fstab_file[fidx].data = realloc(fstab_file[fidx].data,
					sizeof(*fstab_file[fidx].data)
					* (fstab_file[fidx].numrecords + 1));
	if(fstab_file[fidx].data == NULL) {
	    fprintf(stderr, "%s: internal memory error: fstab data reallocation\n", g_programName);
	    exit(1);
	}
    }
    fstab_file[fidx].data[fstab_file[fidx].numrecords] = data;
    fstab_file[fidx].numrecords++;

    return fstab_file[fidx].numrecords - 1;
}

node_t *
generate_fstab_menu(char * inv_path, node_t *startnode, int level, char *fstabname)
{
    int		i;
    char	*invname;
    char	*txt;
    int		idx;
    int		len;
    node_t	*list = NULL;
    node_t	*n;
    invt_fstab_t *fstab_entry;

    if((idx = open_fstab(fstabname)) < 0) {
	return NULL;
    }
    fstab_entry = (invt_fstab_t *)(fstab_file[idx].mapaddr + sizeof(invt_counter_t));

    n = startnode;
    for(i=0; i < fstab_file[idx].counter->ic_curnum; i++) {
	len = strlen(fstab_entry[i].ft_mountpt) + strlen(fstab_entry[i].ft_devpath) + 40;
	txt = malloc(len);
	if(txt == NULL) {
	    fprintf(stderr, "%s: internal memory error: fstab_text\n", g_programName);
	    exit(1);
	}
	snprintf(txt, len, "    fs: %s", fstab_entry[i].ft_mountpt);

	n = list_add(n, node_create(BOOL_FALSE,	/* hidden */
				    BOOL_FALSE,	/* expanded */
				    level,	/* level */
				    BOOL_FALSE,	/* deleted */
				    idx,	/* file_idx */
				    txt,	/* text */
				    &(fstab_ops), /* menu ops */
				    NULL,	/* parent */
				    NULL,	/* children */
				    0,		/* nbr_children */
				    i));	/* data_idx */
	if(i == 0) {
	    list = n;
	}
	if(n == NULL) {
	    fprintf(stderr, "%s: internal memory error: list_add: node_create: fstab\n", g_programName);
	    exit(1);
	}

	add_fstab_data(idx, &(fstab_entry[i]));
	invname = GetNameOfInvIndex(inv_path, fstab_entry[i].ft_uuid);
	n = generate_invidx_menu(inv_path, n, level + 1, invname);
	free(invname);
    }

    return list;
}

int
add_fstab(char *name, int fd, int nEntries, char *mapaddr, invt_counter_t *counter)
{
    static int highwatermark = 10;

    if(fstab_numfiles == 0) {
	fstab_file = malloc(sizeof(*fstab_file) * highwatermark);
	if(fstab_file == NULL) {
	    fprintf(stderr, "%s: internal memory error: malloc fstab_file\n", g_programName);
	    exit(1);
	}
    }
    else {
	if(fstab_numfiles >= highwatermark - 1) {
	    highwatermark += highwatermark;

	    fstab_file = realloc(fstab_file, sizeof(*fstab_file) * highwatermark);
	    if(fstab_file == NULL) {
		fprintf(stderr, "%s: internal memory error: realloc fstab\n", g_programName);
		exit(1);
	    }
	}
    }

    fstab_file[fstab_numfiles].fd	= fd;
    fstab_file[fstab_numfiles].name	= name;
    fstab_file[fstab_numfiles].nEntries = nEntries;
    fstab_file[fstab_numfiles].mapaddr	= mapaddr;
    fstab_file[fstab_numfiles].counter	= counter;
    fstab_file[fstab_numfiles].data	= NULL;
    fstab_file[fstab_numfiles].numrecords = 0;

    fstab_numfiles++;

    return fstab_numfiles - 1;
}

int
fstab_equal(invt_fstab_t *f1, invt_fstab_t *f2)
{
    if(strcmp(f1->ft_mountpt, f2->ft_mountpt) != 0)
	return BOOL_FALSE;

    if(strcmp(f1->ft_devpath, f2->ft_devpath) != 0)
	return BOOL_FALSE;

    if(uuid_compare(f1->ft_uuid, f2->ft_uuid) != 0)
	return BOOL_FALSE;

    return BOOL_TRUE;
}

int
find_matching_fstab(int fidx, invt_fstab_t *fstabentry)
{
    int i;

    for(i = 0; i < fstab_file[fidx].numrecords; i++) {
	if(fstab_equal(fstab_file[fidx].data[i], fstabentry) == BOOL_TRUE) {
	    return i;
	}
    }

    return -1;
}

void *
remmap_fstab(int fidx, int num)
{
    int		i;
    int		nEntries;
    char	*mapaddr;
    invt_fstab_t *fstab_entry;

    nEntries = fstab_file[fidx].counter->ic_curnum;

    munmap( fstab_file[fidx].mapaddr,
	    (nEntries * sizeof(invt_fstab_t)) + sizeof(invt_counter_t));

    /* need to lseek on the file to grow it to the right size - no autogrow on linux */
    lseek(fstab_file[fidx].fd, (num * sizeof(invt_fstab_t)) - 1, SEEK_END);
    write_n_bytes(fstab_file[fidx].fd, "", 1, fstab_file[fidx].name);

    mapaddr = mmap_n_bytes(fstab_file[fidx].fd,
			   (nEntries + 1) * (num * sizeof(invt_fstab_t))
			   + sizeof(invt_counter_t),
			   BOOL_FALSE,
			   fstab_file[fidx].name);

    fstab_file[fidx].nEntries += num;
    fstab_file[fidx].mapaddr = mapaddr;
    fstab_file[fidx].counter = (invt_counter_t *)mapaddr;

    /* regenerate the data index for the file given the new mapping */
    fstab_entry = (invt_fstab_t *)(fstab_file[fidx].mapaddr + sizeof(invt_counter_t));
    for(i=0; i < nEntries; i++) {
    	if(i >= fstab_file[fidx].numrecords) {
	    add_fstab_data(fidx, &(fstab_entry[i]));
	}
	else {
	    fstab_file[fidx].data[i] = &(fstab_entry[i]);
	}
    }

    return &(fstab_entry[i]);
}

int
open_fstab(char *fstabname)
{
    int			fd;
    int			nEntries;
    char		*mapaddr;
    char		*name;
    invt_counter_t	cnt;

    fd = open_and_lock( fstabname, FILE_WRITE, wait_for_locks );
    if (fd < 0) {
	fprintf( stderr, "%s: abnormal termination\n", g_programName );
	exit(1);
    }

    name = strdup(fstabname);
    if(name == NULL) {
	fprintf(stderr, "%s: internal memory error:  strdup fstabname\n", g_programName);
	exit(1);
    }

    read_n_bytes(fd, &cnt, sizeof(invt_counter_t), fstabname);

    nEntries = cnt.ic_curnum;

    mapaddr = mmap_n_bytes(fd,
			   nEntries * sizeof(invt_fstab_t) + sizeof(invt_counter_t),
			   BOOL_FALSE,
			   fstabname);

    return add_fstab(name, fd, nEntries, mapaddr, (invt_counter_t *)mapaddr);
}

int
close_fstab(int fidx)
{
    int fstabentries;

    fstabentries = fstab_file[fidx].counter->ic_curnum;

    munmap( fstab_file[fidx].mapaddr,
	    (fstab_file[fidx].counter->ic_curnum * sizeof(invt_fstab_t)) + sizeof(invt_counter_t));

    if ((fstabentries != 0)  && (fstabentries < fstab_file[fidx].nEntries)) {
	ftruncate(fstab_file[fidx].fd,
		  sizeof(invt_counter_t) + (sizeof(invt_fstab_t) * fstabentries));
    }

    close(fstab_file[fidx].fd);

    if (fstabentries == 0) {
	unlink( fstab_file[fidx].name );
    }

    free(fstab_file[fidx].name);
    free(fstab_file[fidx].data);

    return 0;
}

int
close_all_fstab()
{
    int i;

    for(i = 0; i < fstab_numfiles; i++) {
	close_fstab(i);
    }

    free(fstab_file);
    fstab_file = NULL;
    return 0;
}
