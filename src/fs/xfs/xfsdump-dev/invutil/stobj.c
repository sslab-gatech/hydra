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
#include <ncurses.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <uuid/uuid.h>

#include "types.h"
#include "mlog.h"
#include "inv_priv.h"
#include "getopt.h"

#include "invutil.h"
#include "timeutil.h"
#include "cmenu.h"
#include "list.h"
#include "stobj.h"

stobj_fileinfo_t *stobj_file;
int stobj_numfiles;

menu_ops_t stobjsess_ops = {
    NULL,
    stobj_undelete,
    NULL,
    stobj_select,
    NULL,
    NULL,
    NULL,
    NULL,
    stobjsess_highlight,
    menu_unhighlight,
    stobjsess_commit,
    stobj_prune,
};

menu_ops_t stobjstrm_ops = {
    stobjstrm_delete,
    stobj_undelete,
    NULL,
    stobj_select,
    NULL,
    NULL,
    NULL,
    NULL,
    stobjstrm_highlight,
    menu_unhighlight,
    stobjstrm_commit,
    NULL,
};

menu_ops_t stobjmed_ops = {
    stobjmed_delete,
    NULL,
    NULL,
    stobj_select,
    NULL,
    NULL,
    NULL,
    NULL,
    stobjmed_highlight,
    menu_unhighlight,
    stobjmed_commit,
    NULL,
};

/*ARGSUSED*/
int
stobjmed_commit(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return 0;
    }
    d = (data_t *)(current->data);

    return stobjstrm_commit(win, d->parent, list);
}

/*ARGSUSED*/
int
stobjstrm_commit(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return 0;
    }
    d = (data_t *)(current->data);

    return stobjsess_commit(win, d->parent, list);
}

/*ARGSUSED*/
int
stobjmed_delete(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return 0;
    }
    d = (data_t *)(current->data);

    return stobjstrm_delete(win, d->parent, list);
}

/*ARGSUSED*/
int
stobjstrm_delete(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return 0;
    }
    d = (data_t *)(current->data);

    return menu_delete(win, d->parent, list);
}

/*ARGSUSED*/
int
stobjsess_commit(WINDOW *win, node_t *current, node_t *list)
{
    data_t *d;
    invt_seshdr_t *stobjhdr;

    if(current == NULL || current->data == NULL) {
	return 0;
    }
    d = (data_t *)(current->data);
    stobjhdr = ((stobjsess_t *)(stobj_file[d->file_idx].data[d->data_idx]))->header;

    mark_all_children_commited(current);

    if(d->deleted == BOOL_TRUE) {
	stobjhdr->sh_pruned = 1;
    }
    else {
	stobjhdr->sh_pruned = 0;
    }

    return 0;
}

/*ARGSUSED*/
int
stobjsess_highlight(WINDOW *win, node_t *current, node_t *list)
{
    static char txt[256];
    char uuidstr[UUID_STR_LEN + 1];
    data_t *d;
    invt_seshdr_t *stobjhdr;
    invt_session_t *stobjses;

    if(current == NULL || current->data == NULL) {
	return 0;
    }
    d = (data_t *)(current->data);
    stobjhdr = ((stobjsess_t *)(stobj_file[d->file_idx].data[d->data_idx]))->header;
    stobjses = ((stobjsess_t *)(stobj_file[d->file_idx].data[d->data_idx]))->session;

    wclear(infowin);

    put_info_header("session entry");

    snprintf(txt, sizeof(txt), "pruned:  %s, flags: %#x, time: %s",
	    (stobjhdr->sh_pruned == BOOL_TRUE) ? "yes" : "no",
	    stobjhdr->sh_flag,
	    ctime32(&(stobjhdr->sh_time)));
    txt[strlen(txt)-1] = '\0';
    put_info_line(1, txt);

    uuid_unparse(stobjses->s_fsid, uuidstr);
    snprintf(txt, sizeof(txt), "mountpt: %s, fsid: %s", stobjses->s_mountpt, uuidstr);
    put_info_line(2, txt);

    uuid_unparse(stobjses->s_sesid, uuidstr);
    snprintf(txt, sizeof(txt), "device:  %s, sesid: %s", stobjses->s_devpath, uuidstr);
    put_info_line(3, txt);

    return FALSE;
}

/*ARGSUSED*/
int
stobjstrm_highlight(WINDOW *win, node_t *current, node_t *list)
{
    static char txt[256];
    data_t *d;
    invt_stream_t *stobjstrm;

    if(current == NULL || current->data == NULL) {
	return 0;
    }
    d = (data_t *)(current->data);
    stobjstrm = (invt_stream_t *)(stobj_file[d->file_idx].data[d->data_idx]);

    wclear(infowin);

    put_info_header("session stream");

    snprintf(txt, sizeof(txt), "interrupted: %s, cmdarg: %s",
	    (stobjstrm->st_interrupted == BOOL_TRUE) ? "yes" : "no",
	    stobjstrm->st_cmdarg);
    put_info_line(1, txt);

    snprintf(txt, sizeof(txt), "start ino: %llu, offset %lld",
	    (unsigned long long) stobjstrm->st_startino.ino,
	    (long long) stobjstrm->st_startino.offset);
    put_info_line(2, txt);

    snprintf(txt, sizeof(txt), "  end ino: %llu, offset %lld",
	    (unsigned long long) stobjstrm->st_endino.ino,
	    (long long) stobjstrm->st_endino.offset);
    put_info_line(3, txt);

    return FALSE;
}

/*ARGSUSED*/
int
stobjmed_highlight(WINDOW *win, node_t *current, node_t *list)
{
    static char txt[256];
    char uuidstr[UUID_STR_LEN + 1];
    data_t *d;
    invt_mediafile_t *stobjmed;

    if(current == NULL || current->data == NULL) {
	return 0;
    }
    d = (data_t *)(current->data);
    stobjmed = (invt_mediafile_t *)(stobj_file[d->file_idx].data[d->data_idx]);

    wclear(infowin);

    put_info_header("session media file");

    uuid_unparse(stobjmed->mf_moid, uuidstr);
    snprintf(txt, sizeof(txt), "flags: %#x, id: %s", stobjmed->mf_flag, uuidstr);
    put_info_line(1, txt);

    snprintf(txt, sizeof(txt), "start ino: %llu, offset %lld",
	    (unsigned long long) stobjmed->mf_startino.ino,
	    (long long) stobjmed->mf_startino.offset);
    put_info_line(2, txt);

    snprintf(txt, sizeof(txt), "  end ino: %llu, offset %lld",
	    (unsigned long long) stobjmed->mf_endino.ino,
	    (long long) stobjmed->mf_endino.offset);
    put_info_line(3, txt);

    return FALSE;
}

/*ARGSUSED*/
int
stobj_select(WINDOW *win, node_t *current, node_t *list)
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
stobj_prune(char *mountpt, uuid_t *uuidp, time32_t prunetime, node_t *node, node_t *list)
{
    data_t *d;
    stobjsess_t *stobj;
    invt_seshdr_t *stobj_header;
    invt_session_t *stobj_session;

    if(node == NULL || node->data == NULL) {
	return BOOL_FALSE;
    }
    d = (data_t *)(node->data);
    stobj = stobj_file[d->file_idx].data[d->data_idx];
    stobj_header = stobj->header;
    stobj_session = stobj->session;

    if(stobj_header == NULL || stobj_session == NULL) {
	return BOOL_FALSE;
    }
    if(stobj_header->sh_pruned) {
	return BOOL_TRUE;
    }
    if(stobj_header->sh_time < prunetime) {
	if(mountpt != NULL && mntpnt_equal(mountpt, stobj_session->s_mountpt)) {
	    return BOOL_TRUE;
	}
	if((!uuid_is_null(*uuidp))
	   && (uuid_compare(*uuidp, stobj_session->s_fsid) == 0)) {
	    return BOOL_TRUE;
	}
    }

    return BOOL_FALSE;
}

/*ARGSUSED*/
int
stobj_undelete(WINDOW *win, node_t *current, node_t *list)
{
    node_t *n;
    data_t *d;

    if(current == NULL || current->data == NULL) {
	return BOOL_FALSE;
    }

    list_undelete(current, list);

    d = ((data_t *)(current->data));
    n = current->next;
    while(n != NULL && n->data != NULL && ((data_t *)(n->data))->level > d->level) {
	((data_t *)(n->data))->deleted = BOOL_FALSE;
	((data_t *)(n->data))->text[0] = ' ';
	n = n->next;
    }

    redraw_screen = BOOL_TRUE;

    return BOOL_FALSE;
}

int
add_stobj_data(int fidx, void *data)
{
    if(stobj_file[fidx].numrecords == 0) {
	stobj_file[fidx].data = malloc(sizeof(*stobj_file[fidx].data));
	if(stobj_file[fidx].data == NULL) {
	    fprintf(stderr, "%s: internal memory error: stobj data allocation\n",
		    g_programName);
	    exit(1);
	}
    }
    else {
	stobj_file[fidx].data = realloc(stobj_file[fidx].data,
					sizeof(*stobj_file[fidx].data)
					* (stobj_file[fidx].numrecords + 1));
	if(stobj_file[fidx].data == NULL) {
	    fprintf(stderr, "%s: internal memory error: stobj data reallocation\n",
		    g_programName);
	    exit(1);
	}
    }
    stobj_file[fidx].data[stobj_file[fidx].numrecords] = data;
    stobj_file[fidx].numrecords++;

    return 0;
}

node_t *
generate_stobj_menu(node_t *startnode, int level, char *StObjFileName)
{
    int		i;
    int		j;
    int		k;
    int		idx;
    int		data_idx;
    int		len;
    char	*txt;
    node_t	*n;
    node_t	*parent_stream;
    node_t	*parent_session;

    stobjsess_t		*session;
    invt_seshdr_t	*StObjhdr;
    invt_session_t	*StObjses;
    invt_stream_t	*StObjstrm;
    invt_mediafile_t	*StObjmed;

    if((idx = open_stobj(StObjFileName)) < 0) {
	return startnode;
    }

    StObjhdr = (invt_seshdr_t *)( stobj_file[idx].mapaddr + sizeof(invt_sescounter_t));
    StObjses = (invt_session_t *)(stobj_file[idx].mapaddr + StObjhdr->sh_sess_off);

    data_idx = 0;
    n = startnode;
    for (i=0; i < stobj_file[idx].counter->ic_curnum; ) {
	session = malloc(sizeof(*session));
	if(session == NULL) {
	    fprintf(stderr, "%s: internal memory error: session malloc\n", g_programName);
	    exit(1);
	}
	session->header = StObjhdr;
	session->session = StObjses;

	len = 60+strlen(session->session->s_label);
	txt = malloc(len);
	if(txt == NULL) {
	    fprintf(stderr, "%s: internal memory error: invidx_text\n", g_programName);
	    exit(1);
	}
	snprintf(txt, len, "        session level: %d label: %s",
		session->header->sh_level,
		session->session->s_label);

	n = list_add(n, node_create(BOOL_TRUE,	/* hidden */
				    BOOL_FALSE,	/* expanded */
				    level,	/* level */
				    session->header->sh_pruned,	/* deleted */
				    idx,	/* file_idx */
				    txt,	/* text */
				    &(stobjsess_ops), /* menu ops */
				    startnode,	/* parent */
				    NULL,	/* children */
				    0,		/* nbr_children */
				    data_idx));
	if(n == NULL) {
	    fprintf(stderr, "%s: internal memory error: list_add: node_create: session\n", g_programName);
	    exit(1);
	}

	data_idx++;
	add_stobj_data(idx, session);

	parent_session = n;
	for ( j = 0; j < (int) StObjses->s_cur_nstreams; j++ ) {
	    StObjstrm = (invt_stream_t *)(stobj_file[idx].mapaddr + 
					  StObjhdr->sh_streams_off +
					  (j * sizeof(invt_stream_t)));

	    len = strlen(StObjstrm->st_cmdarg) + 33;
	    txt = malloc(len);
	    if(txt == NULL) {
		fprintf(stderr, "%s: internal memory error: invidx_text\n", g_programName);
		exit(1);
	    }
	    snprintf(txt, len, "          stream: drive path: %s", StObjstrm->st_cmdarg);

	    n = list_add(n, node_create(BOOL_TRUE,	/* hidden */
					BOOL_FALSE,	/* expanded */
					level + 1,	/* level */
					session->header->sh_pruned,	/* deleted */
					idx,		/* file_idx */
					txt,		/* text */
					&(stobjstrm_ops), /* menu ops */
					parent_session,	/* parent */
					NULL,		/* children */
					0,		/* nbr_children */
					data_idx));
	    if(n == NULL) {
		fprintf(stderr, "%s: internal memory error: list_add: node_create: StObjstrm\n", g_programName);
		exit(1);
	    }

	    data_idx++;
	    add_stobj_data(idx, StObjstrm);

	    parent_stream = n;
	    for ( k = 0; k < StObjstrm->st_nmediafiles; k++) {
		StObjmed = (invt_mediafile_t *)(stobj_file[idx].mapaddr + 
						StObjstrm->st_firstmfile +
						(k * sizeof(invt_mediafile_t)));
		
		len = strlen(StObjmed->mf_label) + 26;
		txt = malloc(len);
		if(txt == NULL) {
		    fprintf(stderr, "%s: internal memory error: invidx_text\n", g_programName);
		    exit(1);
		}
		snprintf(txt, len, "            media file: %s", StObjmed->mf_label);

		n = list_add(n, node_create(BOOL_TRUE,	/* hidden */
					    BOOL_FALSE,	/* expanded */
					    level + 2,	/* level */
					    session->header->sh_pruned,	/* deleted */
					    idx,	/* file_idx */
					    txt,	/* text */
					    &(stobjmed_ops), /* menu ops */
					    parent_stream, /* parent */
					    NULL,	/* children */
					    0,		/* nbr_children */
					    data_idx));
		if(n == NULL) {
		    fprintf(stderr, "%s: internal memory error: list_add: node_create: StObjmed\n", g_programName);
		    exit(1);
		}

		data_idx++;
		add_stobj_data(idx, StObjmed);
	    }
	}

	i++;

	StObjhdr = (invt_seshdr_t *)( stobj_file[idx].mapaddr + sizeof(invt_sescounter_t) +
				      (i * sizeof(invt_seshdr_t)) );
	StObjses = (invt_session_t *)(stobj_file[idx].mapaddr + StObjhdr->sh_sess_off);
    }

    return n;
}

int
add_stobj(char *name, int fd, off_t size, char *mapaddr, invt_sescounter_t *counter)
{
    static int highwatermark = 20;

    if(stobj_numfiles == 0) {
	stobj_file     = malloc(sizeof(*stobj_file) * highwatermark);
	if(stobj_file == NULL) {
	    fprintf(stderr, "%s: internal memory error: malloc stobj_file\n", g_programName);
	    exit(1);
	}
    }
    else {
	if(stobj_numfiles >= highwatermark - 1) {
	    highwatermark += highwatermark;

	    stobj_file = realloc(stobj_file, sizeof(*stobj_file) * highwatermark);
	    if(stobj_file == NULL) {
		fprintf(stderr, "%s: internal memory error: realloc stobj_file\n", g_programName);
		exit(1);
	    }
	}
    }

    stobj_file[stobj_numfiles].size     = size;
    stobj_file[stobj_numfiles].mapaddr  = mapaddr;
    stobj_file[stobj_numfiles].fd       = fd;
    stobj_file[stobj_numfiles].counter  = counter;
    stobj_file[stobj_numfiles].name     = name;
    stobj_file[stobj_numfiles].data     = NULL;
    stobj_file[stobj_numfiles].numrecords = 0;

    stobj_numfiles++;

    return stobj_numfiles - 1;
}

int
open_stobj(char *StObjFileName)
{
    int			fd;
    struct stat		sb;
    off_t		size;
    char		*mapaddr;
    invt_sescounter_t	cnt;
    char		*name;

    errno=0;
    fd = open_and_lock( StObjFileName, FILE_WRITE, wait_for_locks );
    if (fd < 0) {
	return fd;
    }

    read_n_bytes(fd, &cnt, sizeof(invt_sescounter_t), StObjFileName);
    lseek( fd, 0, SEEK_SET );
    errno = 0;
    if (fstat(fd, &sb) < 0) {
	fprintf(stderr, "Could not get stat info on %s\n", StObjFileName);
	perror("fstat");
	close(fd);
	return -1;
    }
    size = sb.st_size;
    mapaddr = mmap_n_bytes(fd, size, BOOL_FALSE, StObjFileName);

    name = strdup(StObjFileName);
    if(name == NULL) {
	fprintf(stderr, "%s: internal memory error: strdup stobj_name\n", g_programName);
	exit(1);
    }

    return add_stobj(name, fd, size, mapaddr, (invt_sescounter_t *)mapaddr);
}

int
close_stobj_file(int fidx, int unlink_ok)
{
    if(fidx >= stobj_numfiles || stobj_file[fidx].fd < 0)
	return 0;

    munmap( stobj_file[fidx].mapaddr, stobj_file[fidx].size);
    close( stobj_file[fidx].fd );
    stobj_file[fidx].fd = -1;

    if(unlink_ok == BOOL_TRUE) {
	unlink(stobj_file[fidx].name);
    }

    free( stobj_file[fidx].name );
    free( stobj_file[fidx].data );

    stobj_file[fidx].name = NULL;
    stobj_file[fidx].data = NULL;

    return 0;
}

int
close_all_stobj()
{
    int i;
    int j;
    invt_seshdr_t *StObjhdr;
    int unlink_ok;

    for(i = 0; i < stobj_numfiles; i++) {

	if(stobj_file[i].fd < 0)
	    continue;

	unlink_ok = BOOL_TRUE;
	StObjhdr = (invt_seshdr_t *)( stobj_file[i].mapaddr + sizeof(invt_sescounter_t));
	for(j = 0; j < stobj_file[i].counter->ic_curnum; ) {
	    if(StObjhdr->sh_pruned != 1) {
		unlink_ok = BOOL_FALSE;
		break;
	    }
	    j++;
	    StObjhdr = (invt_seshdr_t *)( stobj_file[i].mapaddr + sizeof(invt_sescounter_t) +
					  (j * sizeof(invt_seshdr_t)) );
	}

	close_stobj_file(i, unlink_ok);
    }

    free(stobj_file);
    stobj_file = NULL;
    return 0;
}
