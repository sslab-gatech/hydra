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

#include <stdio.h>
#include <fcntl.h>
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
#include "cmenu.h"
#include "list.h"
#include "invidx.h"
#include "stobj.h"
#include "timeutil.h"

invidx_fileinfo_t *invidx_file;
int invidx_numfiles;

extern stobj_fileinfo_t *stobj_file;
extern int stobj_numfiles;

static void stobj_makefname_len( char *fname, int fname_len );

menu_ops_t invidx_ops = {
    NULL,
    invidx_undelete,
    NULL,
    invidx_select,
    NULL,
    NULL,
    NULL,
    NULL,
    invidx_highlight,
    menu_unhighlight,
    invidx_commit,
    invidx_prune,
};


/*ARGSUSED*/
int
invidx_commit(WINDOW *win, node_t *current, node_t *list)
{
    int i;
    int idx;
    int fidx;
    node_t *n;
    data_t *d;
    invt_entry_t *inv_entry;

    n = current;
    if(n == NULL || n->data == NULL)
	return 0;

    d = ((data_t *)(n->data));
    fidx = d->file_idx;
    inv_entry = invidx_file[fidx].data[d->data_idx];

    idx = (int)(((long)inv_entry - (long)invidx_file[fidx].mapaddr - sizeof(invt_counter_t)) / sizeof(invt_entry_t));


    if(d->deleted == BOOL_TRUE && d->imported == BOOL_FALSE) {
	/* shortcut - just close and unlink stobj files */
	for(i = 0; i < d->nbr_children; i++) {
	    close_stobj_file(((data_t *)(d->children[i]->data))->file_idx, BOOL_TRUE);
	}
	mark_all_children_commited(current);

	if ( invidx_file[fidx].counter->ic_curnum > 1 ) {
	    memmove(&inv_entry[idx],
		    &inv_entry[idx + 1],
		    (sizeof(invt_entry_t) * (invidx_file[fidx].counter->ic_curnum - idx - 1)));

	    invidx_file[fidx].data[idx]=NULL;
	    for(i = idx + 1; i < invidx_file[fidx].numrecords; i++) {
		if(invidx_file[fidx].data[i] != NULL)
		    invidx_file[fidx].data[i]--;
	    }
	}
	invidx_file[fidx].counter->ic_curnum--;
    }
    else if(d->imported == BOOL_TRUE) {
	int dst_fileidx;
	int dst_invpos;
	int dst_invidx;
	char dst_idxfile[MAXPATHLEN];
	struct stat s;
	char dst_stobjfile[MAXPATHLEN];
	char *stobjfile;
	char cmd[1024];

	snprintf(dst_idxfile, sizeof(dst_idxfile), "%s/%s", inventory_path, basename(invidx_file[fidx].name));

	/* append if we have the target open already, else write to a possibly new file */
	if((dst_fileidx = find_matching_invidxfile(dst_idxfile)) >= 0) {
	    invt_entry_t *dest = NULL;

	    if((dst_invidx = find_overlapping_invidx(dst_fileidx, inv_entry)) >= 0) {
	    	char *stobj_filename = NULL;
		int stobj_fd = -1;
		int stobj_fileidx;
		int last_dst_invidx;
	    	node_t *tmp_parent;
		data_t *tmp_d;
		node_t *dst_n;
		data_t *dst_d;
		invt_seshdr_t *hdr;
		invt_session_t *ses;
		invt_stream_t *strms;
		invt_mediafile_t *mfiles;

		last_dst_invidx = -1;
		i = d->nbr_children - 1;
		do {
		    if(dst_invidx != last_dst_invidx) {
			stobj_fileidx = find_matching_stobjfile(invidx_file[dst_fileidx].data[dst_invidx]->ie_filename);
			stobj_filename = stobj_file[stobj_fileidx].name;
			stobj_fd = stobj_file[stobj_fileidx].fd;
			last_dst_invidx = dst_invidx;
		    }

		    /* for each child of inv_entry, insert into invidx & stobj files */
		    tmp_d = d->children[i]->data;

		    read_stobj_info(stobj_file[tmp_d->file_idx].fd,
		    		i,
				&hdr,
				&ses,
				&strms,
				&mfiles);
		    
		    insert_stobj_into_stobjfile(dst_fileidx,
		    		stobj_filename,
				stobj_fd,
				hdr,
				ses,
				strms,
				mfiles);

		    free(hdr); free(ses); free(strms); free(mfiles);

		    if(--i < 0)
		    	break;
		}
		while((dst_invidx = find_overlapping_invidx(dst_fileidx, inv_entry)) >= 0);

		mark_all_children_commited(current);

		/* Clean up the mess we just created */
		/* find node for dst_fileidx */
		dst_n = find_invidx_node(list, dst_fileidx);
		tmp_parent = ((data_t *)(dst_n->data))->parent;
		while(dst_n != NULL) {
		    node_t *tmp_n1;
		    
		    dst_d = dst_n->data;

		    /* close affected invidx file and stobj files */
		    for(i = 0; i < dst_d->nbr_children; i++) {
		        close_stobj_file(((data_t *)(dst_d->children[i]->data))->file_idx, BOOL_FALSE);
		    }

		    /* free_all_children on that node */
		    free_all_children(dst_n);
		    tmp_n1 = find_invidx_node(dst_n->next, dst_fileidx);
		    node_free(list_del(dst_n));
		    dst_n = tmp_n1;
		}
		close_invidx(dst_fileidx);

		/* generate invidx menu for the parent node */
		dst_n = generate_invidx_menu(inventory_path, tmp_parent, 1, dst_idxfile);

		if(dst_n != tmp_parent) {
		    /* mark all its children commited */
		    mark_all_children_commited(dst_n);
		}

		return 0;
	    }
	    else {
		/* insert new invidx entry, and copy over stobj file */
		if(invidx_file[dst_fileidx].counter->ic_curnum >= invidx_file[dst_fileidx].nEntries) {
		    dest = remmap_invidx(dst_fileidx, 1);
		}

		dst_invpos = find_invidx_insert_pos(dst_fileidx, inv_entry);
		if(dst_invpos >= 0) {
		    memmove(invidx_file[dst_fileidx].data[dst_invpos + 1],
			    invidx_file[dst_fileidx].data[dst_invpos],
			    sizeof(*inv_entry) * (invidx_file[dst_fileidx].counter->ic_curnum - dst_invpos));
		}
		else {
		    dest = invidx_file[dst_fileidx].data[invidx_file[dst_fileidx].counter->ic_curnum - 1] + 1;
		}
		
		/* copy new data to mmap */
		memmove(dest, inv_entry, sizeof(*inv_entry));
	    
		/* increment counter */
		invidx_file[dst_fileidx].counter->ic_curnum++;

		mark_all_children_commited(current);

		/* shortcut - just copy stobj file over */
		stobjfile = get_inventry_stobjfile(d);
		if(stobjfile == NULL) {
		    put_error("Error: unable to work out stobj filename");
		    return 0;
		}

		snprintf(dst_stobjfile, sizeof(dst_stobjfile), "%s/%s", inventory_path, basename(stobjfile));

		if(stat(dst_stobjfile, &s) < 0) {
		    if(errno != ENOENT) {
			put_error("Error: unable to copy stobj file: stat error");
			return 0;
		    }
		}
		else {
		    put_error("Error: unable to copy stobj file: file exists");
		    return 0;
		}
	    
		snprintf(cmd, sizeof(cmd), "cp %s %s", stobjfile, dst_stobjfile);
		if(system(cmd) != 0) {
		    put_error("Error: unable to copy stobj file: cp failed");
		    return 0;
		}
	    
		open_stobj(dst_stobjfile);
		return 0;
	    }
	}
	else {
	    /* no matching inventory index file - just copy entry to a new file */
	    invt_counter_t cnt;
	    int new_fd;

	    memset(&cnt, 0, sizeof(cnt));
	    cnt.ic_maxnum = INVT_MAX_INVINDICES;
	    cnt.ic_curnum = 0;
	    cnt.ic_vernum = INV_VERSION;

	    mark_all_children_commited(current);

	    if(stat(dst_idxfile, &s) < 0) {
		if(errno != ENOENT) {
		    put_error("Error: unable to stat inventory index file");
		    return 0;
		}
	    }
	    else {
		put_error("Error: unable to copy inventory index file: file exists");
		return 0;
	    }

	    new_fd = open(dst_idxfile, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
	    if(new_fd < 0) {
		put_error("Error: unable to open inventory index file");
		return 0;
	    }
	    INVLOCK(new_fd, LOCK_EX);

	    cnt.ic_curnum++;
	    write_n_bytes(new_fd, (char *)&cnt, sizeof(cnt), dst_idxfile);

	    write_n_bytes(new_fd, (char *)inv_entry, sizeof(*inv_entry), dst_idxfile);

	    INVLOCK(new_fd, LOCK_UN);
	    close(new_fd);

	    open_invidx(dst_idxfile);

	    /* shortcut - just copy stobj file over */
	    stobjfile = get_inventry_stobjfile(d);
	    if(stobjfile == NULL) {
		put_error("Error: unable to work out stobj filename");
		return 0;
	    }

	    snprintf(dst_stobjfile, sizeof(dst_stobjfile), "%s/%s", inventory_path, basename(stobjfile));

	    if(stat(dst_stobjfile, &s) < 0) {
		if(errno != ENOENT) {
		    put_error("Error: unable to copy stobj file: stat error");
		    return 0;
		}
	    }
	    else {
		put_error("Error: unable to copy stobj file: file exists");
		return 0;
	    }
	    
	    snprintf(cmd, sizeof(cmd), "cp %s %s", stobjfile, dst_stobjfile);
	    if(system(cmd) != 0) {
		put_error("Error: unable to copy stobj file: cp failed");
		return 0;
	    }
	    
	    open_stobj(dst_stobjfile);
	    return 0;
	}
    }

    return 0;
}

int
create_stobjfile(int invidx_fileidx, char *filename, int fname_len, 
	invt_seshdr_t *hdr, invt_session_t *ses, invt_stream_t *strms,
	invt_mediafile_t *mfiles)
{
    int fd;

    stobj_makefname_len(filename, fname_len);
    fd = stobj_create(filename);

    insert_stobj_into_stobjfile(invidx_fileidx, filename, fd, hdr, ses, strms, mfiles);

    return fd;
}

int
read_stobj_info(int fd, int idx, invt_seshdr_t **out_hdr,
	invt_session_t **out_ses, invt_stream_t **out_strms,
	invt_mediafile_t **out_mfiles)
{
    int i;
    int nmfiles;
    invt_seshdr_t *hdr;
    invt_session_t *ses;
    invt_stream_t *strms;
    invt_mediafile_t *mfiles;

    lseek(fd, STOBJ_OFFSET(idx, 0), SEEK_SET);
    hdr = malloc(sizeof(*hdr));
    read_n_bytes(fd, (char *)hdr, sizeof(*hdr), "stobj file");

    lseek(fd, hdr->sh_sess_off, SEEK_SET);
    ses = malloc(sizeof(*ses));
    read_n_bytes(fd, (char *)ses, sizeof(*ses), "stobj file");

    if(ses->s_cur_nstreams > 0) {
	lseek(fd, hdr->sh_streams_off, SEEK_SET);
	strms = malloc(sizeof(*strms) * ses->s_cur_nstreams);
	read_n_bytes(fd, (char *)strms, sizeof(*strms) * ses->s_cur_nstreams, "stobj file");

	nmfiles = 0;
	for(i = 0; i < ses->s_cur_nstreams; i++) {
	    nmfiles += strms[i].st_nmediafiles;
	}

	if(nmfiles > 0) {
	    mfiles = malloc(sizeof(*mfiles) * nmfiles);
	    read_n_bytes(fd, (char *)mfiles, sizeof(*mfiles) * nmfiles, "stobj file");
	}
	else {
	    mfiles = NULL;
	}
    }    
    else {
	strms = NULL;
	mfiles = NULL;
    }

    *out_hdr = hdr;
    *out_ses = ses;
    *out_strms = strms;
    *out_mfiles = mfiles;
    
    return fd;
}

int
delete_stobj_entries(int fd, int deletepos)
{
    invt_sescounter_t sescnt;
    invt_seshdr_t *hdr;
    invt_session_t *ses;
    invt_stream_t *strms;
    invt_mediafile_t *mfiles;

    lseek(fd, 0, SEEK_SET);
    read_n_bytes(fd, (char *)&sescnt, sizeof(sescnt), "stobj file");

    read_stobj_info(fd, deletepos, &hdr, &ses, &strms, &mfiles);

    sescnt.ic_eof = hdr->sh_streams_off;

    free(hdr); free(ses); free(strms); free(mfiles);

    sescnt.ic_curnum -= deletepos;
    lseek(fd, 0, SEEK_SET);
    write_n_bytes(fd, (char *)&sescnt, sizeof(sescnt), "stobj file");

    return 0;
}

int
find_stobj_insert_point(int fd, invt_seshdr_t *hdr)
{
    int i;
    invt_sescounter_t sescnt;
    invt_seshdr_t tmphdr;

    lseek(fd, 0, SEEK_SET);
    read_n_bytes(fd, (char *)&sescnt, sizeof(sescnt), "stobj file");

    for(i = 0; i < sescnt.ic_curnum; i++) {
	read_n_bytes(fd, (char *)&tmphdr, sizeof(tmphdr), "stobj file");
	if(hdr->sh_time <= tmphdr.sh_time) {
	    break;
	}
    }

    return i;
}

int
update_invidx_entry(int fd, char *filename, int stobj_fd)
{
    int i;
    time32_t start_time;
    time32_t end_time;
    invt_counter_t cnt;
    invt_seshdr_t hdr;
    invt_sescounter_t sescnt;
    invt_entry_t entry;
    off_t first_offset;
    off_t last_offset;

    /* find the start and stop in stobj file */
    lseek(stobj_fd, 0, SEEK_SET);
    read_n_bytes(stobj_fd, &sescnt, sizeof(sescnt), "stobj file");
    
    memset(&hdr, 0, sizeof(hdr));
    first_offset = STOBJ_OFFSET(0,0);
    lseek(stobj_fd, first_offset, SEEK_SET);
    read_n_bytes(stobj_fd, &hdr, sizeof(hdr), "stobj file");
    start_time = hdr.sh_time;

    memset(&hdr, 0, sizeof(hdr));
    last_offset = STOBJ_OFFSET((sescnt.ic_curnum - 1), 0);
    lseek(stobj_fd, last_offset, SEEK_SET);
    read_n_bytes(stobj_fd, &hdr, sizeof(hdr), "stobj file");
    end_time = hdr.sh_time;

    lseek(fd, 0, SEEK_SET);
    read_n_bytes(fd, &cnt, sizeof(cnt), "invidx file");

    for(i = 0; i < cnt.ic_curnum; i++) {
    	lseek(fd, IDX_HDR_OFFSET(i), SEEK_SET);
	read_n_bytes(fd, &entry, sizeof(entry), "invidx file");
	if(strcmp(entry.ie_filename, filename) == 0) {
	    entry.ie_timeperiod.tp_start = start_time;
	    entry.ie_timeperiod.tp_end = end_time;
	    lseek(fd, IDX_HDR_OFFSET(i), SEEK_SET);
	    write_n_bytes(fd, &entry, sizeof(entry), "invidx file");
	    return i;
	}
    }
    
    entry.ie_timeperiod.tp_start = start_time;
    entry.ie_timeperiod.tp_end = end_time;
    strcpy(entry.ie_filename, filename);
    lseek(fd, 0, SEEK_END);
    write_n_bytes(fd, &entry, sizeof(entry), "invidx file");

    cnt.ic_curnum++;
    lseek(fd, 0, SEEK_SET);
    write_n_bytes(fd, &cnt, sizeof(cnt), "invidx file");

    return i;
}

int
insert_stobj_into_stobjfile(int invidx_fileidx, char *filename, int fd,
	invt_seshdr_t *hdr, invt_session_t *ses,
	invt_stream_t *strms, invt_mediafile_t *mfiles)
{
    int i;
    invt_sescounter_t sescnt;
    int pos;
    char *buf;
    int invidx_fd = invidx_file[invidx_fileidx].fd;

    if(hdr == NULL || ses == NULL || strms == NULL || mfiles == NULL)
	return 0;

    lseek(fd, 0, SEEK_SET);
    read_n_bytes(fd, (char *)&sescnt, sizeof(sescnt), "stobj file");

    pos = find_stobj_insert_point(fd, hdr);

    /* if stobj file is full, we need to split it */
    if(sescnt.ic_curnum >= sescnt.ic_maxnum) {
	int new_fd;
	char new_filename[INV_STRLEN];
	invt_seshdr_t *s_hdr;
	invt_session_t *s_ses;
	invt_stream_t *s_strms;
	invt_mediafile_t *s_mfiles;

	new_fd = create_stobjfile(invidx_fileidx, new_filename, sizeof(new_filename), hdr, ses, strms, mfiles);

	for(i = pos; i < sescnt.ic_curnum; i++) {
	    read_stobj_info(fd, i, &s_hdr, &s_ses, &s_strms, &s_mfiles);
	    
	    insert_stobj_into_stobjfile(invidx_fileidx, new_filename, new_fd, s_hdr, s_ses, s_strms, s_mfiles);

	    free(s_hdr); free(s_ses); free(s_strms); free(s_mfiles);
	}
	delete_stobj_entries(fd, pos);
	update_invidx_entry(invidx_fd, new_filename, new_fd);
	update_invidx_entry(invidx_fd, filename, fd);

	return 0;
    }

    /* stobj file not full, just insert the entry */
    hdr->sh_sess_off = STOBJ_OFFSET( sescnt.ic_maxnum, pos);
    hdr->sh_streams_off = sescnt.ic_eof;

    /* for seshdr: malloc buffer, copy new entry into buffer, read file entries into buffer, write the lot out */
    buf = malloc(((sescnt.ic_curnum - pos) + 1) * sizeof(invt_seshdr_t));
    memmove(buf, hdr, sizeof(invt_seshdr_t));
    lseek(fd, STOBJ_OFFSET(pos, 0), SEEK_SET);
    read_n_bytes(fd, buf + sizeof(invt_seshdr_t), (sescnt.ic_curnum - pos) * sizeof(invt_seshdr_t), "stobj file");
    lseek(fd, STOBJ_OFFSET(pos, 0), SEEK_SET);
    write_n_bytes(fd, buf, ((sescnt.ic_curnum - pos) + 1) * sizeof(invt_seshdr_t), "stobj file");
    free(buf);

    /* for session: malloc buffer, copy new entry into buffer, read file entries into buffer, write the lot out */
    buf = malloc(((sescnt.ic_curnum - pos) + 1) * sizeof(invt_session_t));
    memmove(buf, ses, sizeof(invt_session_t));
    lseek(fd, STOBJ_OFFSET(sescnt.ic_maxnum, pos), SEEK_SET);
    read_n_bytes(fd, buf + sizeof(invt_session_t), (sescnt.ic_curnum - pos) * sizeof(invt_session_t), "stobj file");
    lseek(fd, STOBJ_OFFSET(sescnt.ic_maxnum, pos), SEEK_SET);
    write_n_bytes(fd, buf, ((sescnt.ic_curnum - pos) + 1) * sizeof(invt_session_t), "stobj file");
    free(buf);

    /* write the streams and mediafile info at the end of the stobj file */
    stobj_put_streams(fd, hdr, ses, strms, mfiles);

    /* update counter */
    sescnt.ic_curnum++;
    sescnt.ic_eof = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    write_n_bytes(fd, (char *)&sescnt, sizeof(sescnt), "stobj file");

    /* update the inventory index entry */
    update_invidx_entry(invidx_fd, filename, fd);

    return 0;
}

static void
stobj_makefname_len( char *fname, int fname_len )
{	
    /* come up with a new unique name */
    uuid_t	fn;
    char	str[UUID_STR_LEN + 1];

    uuid_generate( fn );
    uuid_unparse( fn, str );

    snprintf(fname, fname_len, "%s/%s.StObj", inventory_path, str);
}

int
stobj_create( char *fname )
{
    int fd;	
    invt_sescounter_t sescnt;

    /* create the new storage object */
    if (( fd = open( fname, O_RDWR | O_EXCL | O_CREAT, S_IRUSR|S_IWUSR )) < 0 ) {
	return -1;
    }
    
    INVLOCK( fd, LOCK_EX );
    fchmod( fd, INV_PERMS );

    memset(&sescnt, 0, sizeof(sescnt));
    sescnt.ic_vernum = INV_VERSION;
    sescnt.ic_curnum = 0; /* there are no sessions as yet */
    sescnt.ic_maxnum = INVT_STOBJ_MAXSESSIONS;
    sescnt.ic_eof = SC_EOF_INITIAL_POS;

    lseek(fd, 0, SEEK_SET);
    write_n_bytes ( fd, (char *)&sescnt, sizeof(sescnt), "new stobj file" );
	
    INVLOCK( fd, LOCK_UN );
    return fd;
}

int
stobj_put_streams( int fd, invt_seshdr_t *hdr, invt_session_t *ses, 
		   invt_stream_t *strms,
		   invt_mediafile_t *mfiles )
{
    uint	nstm	= ses->s_cur_nstreams;
    off64_t	off	= hdr->sh_streams_off;
    off64_t	mfileoff= off + (off64_t)( nstm * sizeof( invt_stream_t ) );
    uint	nmfiles = 0;
    uint	i,j;

    /* fix the offsets in streams */
     for ( i = 0; i < nstm; i++ ) {
	strms[i].st_firstmfile = mfileoff + (off64_t) ((size_t) nmfiles * sizeof( invt_mediafile_t ) );
	/* now fix the offsets in mediafiles */
	for ( j = 0; j < strms[i].st_nmediafiles; j++ ) {
			
	    /* no need to fix the last element's next ptr */
	    if ( j < strms[i].st_nmediafiles - 1 )
		mfiles[ nmfiles + j ].mf_nextmf =  strms[i].st_firstmfile + (off64_t) ((j+1) * sizeof( invt_mediafile_t ));

	    /* no need to fix the first element's prev ptr */
	    if ( j )
		mfiles[ nmfiles + j ].mf_prevmf = strms[i].st_firstmfile + (off64_t) ((j-1) * sizeof( invt_mediafile_t ));
	}

	/* adjust the offsets of the first and the last elements 
	   in the mediafile chain */
	mfiles[ nmfiles ].mf_prevmf = 0;
	nmfiles += strms[i].st_nmediafiles;
	mfiles[ nmfiles - 1 ].mf_nextmf = 0;		
    }

    /* first put the streams. hdr already points at the right place */
    lseek(fd, off, SEEK_SET);
    write_n_bytes( fd, strms, nstm * sizeof( invt_stream_t ), "stobj file" );

    lseek(fd, mfileoff, SEEK_SET);
    write_n_bytes( fd, mfiles, nmfiles * sizeof( invt_mediafile_t ), "stobj file" );

    return 1;	
}

/*ARGSUSED*/
int
invidx_select(WINDOW *win, node_t *current, node_t *list)
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
invidx_highlight(WINDOW *win, node_t *current, node_t *list)
{
    static char txt[256];
    data_t *d;
    invt_entry_t *invtentry;

    if(current == NULL || current->data == NULL) {
	return 0;
    }
    d = (data_t *)(current->data);
    invtentry = invidx_file[d->file_idx].data[d->data_idx];

    wclear(infowin);

    put_info_header("inventory index entry");

    snprintf(txt, sizeof(txt), "path:  %s", invtentry->ie_filename);
    put_info_line(1, txt);

    snprintf(txt, sizeof(txt), "start: %s", ctime32(&invtentry->ie_timeperiod.tp_start));
    txt[strlen(txt) - 1] = '\0';
    put_info_line(2, txt);

    snprintf(txt, sizeof(txt), "end:   %s", ctime32(&invtentry->ie_timeperiod.tp_end));
    txt[strlen(txt) - 1] = '\0';
    put_info_line(3, txt);

    return FALSE;
}

/*ARGSUSED*/
int
invidx_undelete(WINDOW *win, node_t *current, node_t *list)
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

/*ARGSUSED*/
int
invidx_prune(char *mountpt, uuid_t *uuidp, time32_t prunetime, node_t *node, node_t *list)
{
    data_t *d;
    invt_entry_t *invidx_entry;

    if(node == NULL || node->data == NULL) {
	return BOOL_FALSE;
    }
    d = ((data_t *)(node->data));

    invidx_entry = invidx_file[d->file_idx].data[d->data_idx];
    if(invidx_entry == NULL) {
	return BOOL_FALSE;
    }

    if(invidx_entry->ie_timeperiod.tp_start < prunetime) {
	return BOOL_TRUE;
    }

    return BOOL_FALSE;
}

node_t *
find_invidx_node(node_t *list, int inv_fileidx)
{
    node_t *n;
    data_t *d;
    
    n = list;
    while(n != NULL && n->data != NULL) {
    	d = n->data;
	/* is this an invidx entry we're looking at? */
	if(d->ops == &invidx_ops) {
	    if(d->file_idx == inv_fileidx) {
	    	return n;
	    }
	}
	n = n->next;
    }
    
    return NULL;
}

char *
get_inventry_stobjfile(data_t *d)
{
    data_t *child_data;

    if(d->nbr_children <= 0) {
	return NULL;
    }

    /* all the children will be from the same stobj file, so we can just
       get the name for the first */

    child_data = d->children[0]->data;

    return stobj_file[child_data->file_idx].name;
}

int
find_overlapping_invidx(int fidx, invt_entry_t *inv_entry)
{
    int i = 0;
    invt_entry_t *entry;

    for(i = 0; i < invidx_file[fidx].numrecords; i++) {
	entry = invidx_file[fidx].data[i];
	if(entry->ie_timeperiod.tp_end < inv_entry->ie_timeperiod.tp_start
	   || entry->ie_timeperiod.tp_start > inv_entry->ie_timeperiod.tp_end) {
	    continue;
	}
	return i;
    }

    return -1;
}

int
find_invidx_insert_pos(int fidx, invt_entry_t *inv_entry)
{
    int i = 0;
    invt_entry_t *entry;
    invt_entry_t *next_entry;

    for(i = 0; i < invidx_file[fidx].numrecords - 1; i++) {
	entry = invidx_file[fidx].data[i];
	next_entry = invidx_file[fidx].data[i+1];

	if(inv_entry->ie_timeperiod.tp_start > entry->ie_timeperiod.tp_end
	   && inv_entry->ie_timeperiod.tp_end < next_entry->ie_timeperiod.tp_start ) {
	    return i;
	}
    }

    return -1;
}

int
find_matching_stobjfile(char *filename)
{
    int i = 0;

    for(i = 0; i < stobj_numfiles; i++) {
	if(stobj_file[i].name == NULL)
	    continue;
	if(strcmp(stobj_file[i].name, filename) == 0) {
	    return i;
	}
    }

    return -1;
}

int
find_matching_invidxfile(char *filename)
{
    int i = 0;

    for(i = 0; i < invidx_numfiles; i++) {
	if(invidx_file[i].name == NULL)
	    continue;
	if(strcmp(invidx_file[i].name, filename) == 0) {
	    return i;
	}
    }

    return -1;
}

int
add_invidx_data(int fidx, invt_entry_t *data)
{
    if(invidx_file[fidx].numrecords == 0) {
	invidx_file[fidx].data = malloc(sizeof(*invidx_file[fidx].data));
	if(invidx_file[fidx].data == NULL) {
	    fprintf(stderr, "%s: internal memory error: invidx data allocation\n",
		    g_programName);
	    exit(1);
	}
    }
    else {
	invidx_file[fidx].data = realloc(invidx_file[fidx].data,
					 sizeof(*invidx_file[fidx].data)
					 * (invidx_file[fidx].numrecords + 1));
	if(invidx_file[fidx].data == NULL) {
	    fprintf(stderr, "%s: internal memory error: invidx data reallocation\n",
		    g_programName);
	    exit(1);
	}
    }
    invidx_file[fidx].data[invidx_file[fidx].numrecords] = data;
    invidx_file[fidx].numrecords++;

    return 0;
}

node_t *
generate_invidx_menu(char * inv_path, node_t *startnode, int level, char *idxFileName)
{
    int			i;
    int			idx;
    int			len;
    char		*txt;
    node_t		*n;
    invt_entry_t	*invidx_entry;
    char		*stobjname;

    if((idx = open_invidx(idxFileName)) < 0) {
	return startnode;
    }

    invidx_entry = (invt_entry_t *)( invidx_file[idx].mapaddr + sizeof(invt_counter_t));

    n = startnode;
    for (i=0; i < invidx_file[idx].counter->ic_curnum; i++) {
	len = strlen(invidx_entry[i].ie_filename) + 54;
	txt = malloc(len);
	if(txt == NULL) {
	    fprintf(stderr, "%s: internal memory error: invidx_text\n", g_programName);
	    exit(1);
	}
	snprintf(txt, len, "      inv file: %s ", basename(invidx_entry[i].ie_filename));

	n = list_add(n, node_create(BOOL_TRUE,	/* hidden */
				    BOOL_FALSE,	/* expanded */
				    level,	/* level */
				    BOOL_FALSE,	/* deleted */
				    idx,	/* file_idx */
				    txt,	/* text */
				    &(invidx_ops), /* menu ops */
				    startnode,	/* parent */
				    NULL,	/* children */
				    0,		/* nbr_children */
				    i));	/* data_idx */
	if(n == NULL) {
	    fprintf(stderr, "%s: internal memory error: list_add: node_create: invidx\n",
		    g_programName);
	    exit(1);
	}

	add_invidx_data(idx, &(invidx_entry[i]));
	stobjname = GetNameOfStobj(inv_path, invidx_entry[i].ie_filename);
	n = generate_stobj_menu( n, level + 1, stobjname );
	free(stobjname);
    }

    return n;
}

int
add_invidx(char *name, int fd, int nEntries, char *mapaddr, invt_counter_t *counter)
{
    static int highwatermark = 10;

    if(invidx_numfiles == 0) {
	invidx_file = malloc(sizeof(*invidx_file) * highwatermark);
	if(invidx_file == NULL) {
	    fprintf(stderr, "%s: internal memory error: malloc invidx_file\n", g_programName);
	    exit(1);
	}
    }
    else {
	if(invidx_numfiles >= highwatermark - 1) {
	    highwatermark += highwatermark;

	    invidx_file = realloc(invidx_file, sizeof(*invidx_file) * highwatermark);
	    if(invidx_file == NULL) {
		fprintf(stderr, "%s: internal memory error: realloc invidx\n", g_programName);
		exit(1);
	    }
	}
    }

    invidx_file[invidx_numfiles].nEntries = nEntries;
    invidx_file[invidx_numfiles].mapaddr  = mapaddr;
    invidx_file[invidx_numfiles].fd       = fd;
    invidx_file[invidx_numfiles].name     = name;
    invidx_file[invidx_numfiles].counter  = counter;
    invidx_file[invidx_numfiles].data	  = NULL;
    invidx_file[invidx_numfiles].numrecords = 0;

    invidx_numfiles++;

    return invidx_numfiles - 1;
}

void *
remmap_invidx(int fidx, int num)
{
    int		i;
    int		nEntries;
    char	*mapaddr;
    invt_entry_t *inv_entry;

    nEntries = invidx_file[fidx].counter->ic_curnum;

    munmap( invidx_file[fidx].mapaddr,
	    (nEntries * sizeof(invt_entry_t)) + sizeof(invt_counter_t));

    /* need to lseek on the file to grow it to the right size - no autogrow on linux */
    lseek(invidx_file[fidx].fd, (num * sizeof(invt_entry_t)) - 1, SEEK_END);
    write_n_bytes(invidx_file[fidx].fd, "", 1, invidx_file[fidx].name);

    mapaddr = mmap_n_bytes(invidx_file[fidx].fd,
			   (nEntries + 1) * (num * sizeof(invt_entry_t))
			   + sizeof(invt_counter_t),
			   BOOL_FALSE,
			   invidx_file[fidx].name);

    invidx_file[fidx].nEntries += num;
    invidx_file[fidx].mapaddr = mapaddr;
    invidx_file[fidx].counter = (invt_counter_t *)mapaddr;

    /* regenerate the data index for the file given the new mapping */
    inv_entry = (invt_entry_t *)(invidx_file[fidx].mapaddr + sizeof(invt_counter_t));
    for(i=0; i < nEntries; i++) {
    	if(i >= invidx_file[fidx].numrecords) {
	    add_invidx_data(fidx, &(inv_entry[i]));
	}
	else {
	    invidx_file[fidx].data[i] = &(inv_entry[i]);
	}
    }

    return &(inv_entry[i]);
}

int
open_invidx(char *idxFileName)
{
    int fd;
    int nEntries;
    char *mapaddr;
    char *name;
    invt_counter_t header;

    errno=0;
    fd = open_and_lock( idxFileName, FILE_WRITE, wait_for_locks );
    if (fd < 0) {
        return fd;
    }

    name = strdup(idxFileName);
    if(name == NULL) {
	fprintf(stderr, "%s: internal memory error:  strdup idxFileName\n", g_programName);
	exit(1);
    }

    read_n_bytes( fd, &header, sizeof(invt_counter_t), idxFileName);

    nEntries = header.ic_curnum;

    mapaddr = mmap_n_bytes(fd,
			   nEntries*sizeof(invt_entry_t) + sizeof(invt_counter_t),
			   BOOL_FALSE,
			   idxFileName);

    return add_invidx(name, fd, nEntries, mapaddr, (invt_counter_t *)mapaddr);
}


int
close_invidx(int idx)
{
    int nEntries;

    if(idx >= invidx_numfiles || invidx_file[idx].fd < 0)
	return 0;

    nEntries = invidx_file[idx].counter->ic_curnum;
    munmap( invidx_file[idx].mapaddr,
	    (invidx_file[idx].nEntries*sizeof(invt_entry_t)) + sizeof(invt_counter_t) );

    if(nEntries != 0 && nEntries != invidx_file[idx].nEntries) {
	ftruncate(invidx_file[idx].fd,
		  sizeof(invt_counter_t) + (nEntries * sizeof(invt_entry_t)) );
    }

    close( invidx_file[idx].fd );

    if(nEntries == 0) {
	unlink(invidx_file[idx].name);
    }
    free( invidx_file[idx].name );
    free( invidx_file[idx].data );

    invidx_file[idx].fd = -1;
    invidx_file[idx].name = NULL;
    invidx_file[idx].data = NULL;

    return 0;
}

int
close_all_invidx()
{
    int i;

    for(i = 0; i < invidx_numfiles; i++) {
	if(invidx_file[i].fd < 0)
	    continue;
	close_invidx(i);
    }

    free(invidx_file);
    invidx_file = NULL;
    return 0;
}
