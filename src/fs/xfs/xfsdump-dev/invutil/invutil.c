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

#include <xfs/xfs.h>
#include <xfs/jdm.h>

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <string.h>
#include <libgen.h>
#include <uuid/uuid.h>

#include "types.h"
#include "mlog.h"
#include "inv_priv.h"
#include "getopt.h"
#include "timeutil.h"
#include "invutil.h"

char	*g_programName;
char	*g_programVersion;
char	*inventory_path;
bool_t	debug = BOOL_FALSE;
bool_t	force = BOOL_FALSE;
bool_t	wait_for_locks = BOOL_FALSE;
bool_t	resize_screen = BOOL_FALSE;
bool_t	redraw_screen = BOOL_FALSE;
bool_t	redraw_options = BOOL_FALSE;

#ifndef HAVE_CURSES
int
invutil_interactive(char *path, char *mountpt, uuid_t *uuidp, time32_t timeSecs)
{
	fprintf(stderr, "%s: libcurses support not compiled in, "
			"interactive mode is unavailable.\n", g_programName);
	exit(1);
}
#endif

int
main(int argc, char *argv[])
{
    int c = 0;
    bool_t check_option = BOOL_FALSE;
    bool_t mntpnt_option = BOOL_FALSE;
    bool_t uuid_option = BOOL_FALSE;
    bool_t interactive_option = BOOL_FALSE;
    bool_t session_option = BOOL_FALSE;
    static char version[32];
    char *mntPoint = NULL;
    char *r_mf_label = NULL;
    uuid_t uuid;
    uuid_t session;

    snprintf(version, sizeof(version), "version %s", VERSION);
    g_programName = basename(argv[0]);
    g_programVersion = version;
    uuid_clear(uuid);
    uuid_clear(session);

    while((c = getopt( argc, argv, GETOPT_CMDSTRING)) != EOF) {
	switch(c) {
	case GETOPT_DEBUG:
	    debug = BOOL_TRUE;
	    break;
	case GETOPT_INTERACTIVE:
	    if (force) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n",
			 g_programName,
			 GETOPT_FORCE,
			 c );
		usage();
	    }
	    if (session_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n",
			 g_programName,
			 GETOPT_PRUNESESSION,
			 c );
		usage();
	    }
	    interactive_option = BOOL_TRUE;
	    break;
	case GETOPT_NONINTERACTIVE: /* obsoleted by -F */
	    if (interactive_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n",
			 g_programName,
			 GETOPT_INTERACTIVE,
			 c );
		usage();
	    }
	    force = BOOL_TRUE;
	    break;
	case GETOPT_UUID:
	    if (check_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n", 
			 g_programName,
			 GETOPT_CHECKPRUNEFSTAB,
			 c );
		usage();
	    }
	    if (mntpnt_option || session_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n",
			 g_programName,
			 mntpnt_option ? GETOPT_PRUNEMNT : GETOPT_PRUNESESSION,
			 c );
		usage();
	    }
	    uuid_option = BOOL_TRUE;
	    uuid_parse(optarg, uuid);
	    break;
	case GETOPT_WAITFORLOCKS:
	    wait_for_locks = BOOL_TRUE;
	    break;
	case GETOPT_CHECKPRUNEFSTAB:
	    if (mntpnt_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n", 
			 g_programName,
			 GETOPT_PRUNEMNT,
			 c );
		usage();
	    }
	    if (uuid_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n", 
			 g_programName,
			 GETOPT_UUID,
			 c );
		usage();
	    }
	    if (session_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n",
			 g_programName,
			 GETOPT_PRUNESESSION,
			 c );
		usage();
	    }
	    check_option = BOOL_TRUE;
	    break;
	case GETOPT_FORCE:
	    if (interactive_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n",
			 g_programName,
			 GETOPT_INTERACTIVE,
			 c );
		usage();
	    }
	    force = BOOL_TRUE;
	    break;
	case GETOPT_PRUNEMNT:
	    if (check_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n", 
			 g_programName,
			 GETOPT_CHECKPRUNEFSTAB,
			 c );
		usage();
	    }
	    if (uuid_option || session_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n",
			 g_programName,
			 uuid_option ? GETOPT_UUID : GETOPT_PRUNESESSION,
			 c );
		usage();
	    }
	    mntpnt_option = BOOL_TRUE;
	    mntPoint = optarg;
	    break;
	case GETOPT_PRUNEMEDIALABEL:
	    r_mf_label = optarg;
	    break;
	case GETOPT_PRUNESESSION:
	    if (check_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n",
			 g_programName,
			 GETOPT_CHECKPRUNEFSTAB,
			 c );
		usage();
	    }
	    if (interactive_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n",
			 g_programName,
			 GETOPT_INTERACTIVE,
			 c );
		usage();
	    }
	    if (mntpnt_option || uuid_option) {
		fprintf( stderr, "%s: may not specify both -%c and -%c\n",
			 g_programName,
			 mntpnt_option ? GETOPT_PRUNEMNT : GETOPT_UUID,
			 c );
		usage();
	    }
	    session_option = BOOL_TRUE;
	    uuid_parse(optarg, session);
	    break;
	default:
	    usage();
	    break;
	}
    }

    if (r_mf_label != NULL && !(mntpnt_option || uuid_option)) {
	    fprintf( stderr, "%s: -%c requires either -%c or -%c\n",
			 g_programName,
			 GETOPT_PRUNEMEDIALABEL,
			 GETOPT_PRUNEMNT,
			 GETOPT_UUID );
	    usage();
    }

    /* date string only passed for -u and -M */
    if (uuid_option || mntpnt_option) {
	    if (optind != (argc - 1)) {
		    fprintf(stderr, "%s: Date missing for -%c option\n",
		    	 g_programName,
			 uuid_option ? GETOPT_UUID : GETOPT_PRUNEMNT);
		    usage();
	    }
    } else {
	    if (optind != argc) {
		    fprintf(stderr, "%s: Extra arguments specified\n",
		    	 g_programName);
		    usage();
	    }
    }

    /* sanity check the inventory database directory, setup global paths
     */
    if (!inv_setup_base()) {
        fprintf( stderr,
                "%s: both /var/lib/xfsdump and /var/xfsdump exist - fatal\n",
	        g_programName );
        exit(1);
    }
    inventory_path = INV_DIRPATH;

    if (check_option) {
        char *tempstr = "test";
        time32_t temptime = 0;
        CheckAndPruneFstab(inventory_path, BOOL_TRUE, tempstr, &uuid,
		&session, temptime, NULL);
    }
    else if (session_option) {
	CheckAndPruneFstab(
		    inventory_path, BOOL_FALSE , mntPoint, &uuid,
		    &session, (time32_t)0, r_mf_label);
    }
    else if (uuid_option || mntpnt_option) {
        time32_t timeSecs = ParseDate(argv[optind]);

	if (interactive_option) {
	    invutil_interactive(inventory_path, mntPoint, &uuid, timeSecs);
	    printf("\n");
	}
	else {
	    CheckAndPruneFstab(
		    inventory_path, BOOL_FALSE , mntPoint, &uuid,
		    &session, timeSecs, r_mf_label);
	}
    }
    else if ( interactive_option ) {
	invutil_interactive(inventory_path, mntPoint, &uuid, 0);
	printf("\n");
    }
    else {
	usage();
    }

    return 0;
}

/* get_mntpnt & mntpnt_equal are used to allow matching of mountpoints if
 * no hostname is supplied
 */

static char *
get_mntpnt(char *txt)
{
    char *p;

    p = strchr(txt, ':');
    if(p == NULL)
        return txt;

    return p+1;
}

int
mntpnt_equal(char *s1, char *s2)
{
    if(strchr(s1, ':') == NULL || strchr(s2, ':') == NULL) {
        s1 = get_mntpnt(s1);
        s2 = get_mntpnt(s2);
    }

    return STREQL(s1, s2);
}


time32_t
ParseDate(char *strDate)
{
    struct tm tm;
    time_t date = 0;
    time32_t date32;
    char **fmt;
    char *templateStr[] = {
        "%m/%d/%Y %I:%M:%S %p",
        "%m/%d/%Y %H:%M:%S",
        "%m/%d/%Y %I:%M %p",
        "%m/%d/%Y %H:%M",
        "%m/%d/%Y %I %p",
        "%m/%d/%Y %H",
        "%m/%d/%Y",
        "%m/%d/%y %I:%M:%S %p",
        "%m/%d/%y %H:%M:%S",
        "%m/%d/%y %I:%M %p",
        "%m/%d/%y %H:%M",
        "%m/%d/%y %I %p",
        "%m/%d/%y %H",
        "%m/%d/%y",
        "%m/%d",
        "%b %d, %Y %H:%M:%S %p",
        "%b %d, %Y %I:%M:%S",
        "%B %d, %Y %H:%M:%S %p",
        "%B %d, %Y %I:%M:%S",
        "%b %d, %Y %H:%M %p",
        "%b %d, %Y %I:%M",
        "%B %d, %Y %H:%M %p",
        "%B %d, %Y %I:%M",
        "%b %d, %Y %H %p",
        "%b %d, %Y %I",
        "%B %d, %Y %H %p",
        "%B %d, %Y %I",
        "%b %d, %Y",
        "%B %d, %Y",
        "%b %d",
        "%B %d",
        "%m%d%H",
        "%m%d", 
        0};

    for (fmt = &templateStr[0]; *fmt != NULL; fmt++) { 
        memset (&tm, 0, sizeof (struct tm)); /* ensure fields init'ed */
        if (strptime(strDate, *fmt, &tm) != NULL)
            break;
    }

#ifdef INV_DEBUG
    printf("INV_DEBUG: the date entered is %s\n", strDate);
    printf("INV_DEBUG: the hour parsed from string is %d\n", tm.tm_hour);
#endif

    if (*fmt == NULL || (date = mktime(&tm)) < 0) {
        fprintf(stderr, "%s: bad date, \"%s\"\n", g_programName, strDate );
        usage(); 
    }

    /* HACK to ensure tm_isdst is set BEFORE calling mktime(3) */ 
    if (tm.tm_isdst) {
        int dst = tm.tm_isdst;
        memset (&tm, 0, sizeof (struct tm));
        tm.tm_isdst = dst;
        (void)strptime(strDate, *fmt, &tm); 
        tm.tm_isdst = dst;
        date = mktime(&tm);
    }

    /* xfsdump inventory uses time32_t for portability.
     * make sure the given date did not overflow... */
    date32 = date;
    if (date32 != date) {
        fprintf(stderr, "%s: date out of range: \"%s\"\n", g_programName, strDate);
        usage();
    }

#ifdef INV_DEBUG
    printf("INV_DEBUG: the date entered is %s\n", strDate);
    printf("INV_DEBUG: the hour parsed from string is %d\n", tm.tm_hour);
    printf("INV_DEBUG: the date entered in secs is %d\n", date32);
#endif /* INV_DEBUG */

    return date32;
}

char *
GetNameOfStobj (char *inv_path, char *filename)
{
    char *str;
    char *name;

    str = basename(filename);
    name = (char *) malloc( strlen( inv_path ) + 1  + strlen( str ) + 1);
    strcpy( name, inv_path );
    strcat( name, "/" );
    strcat( name, str );

    return(name);
}

char *
GetNameOfInvIndex (char *inv_path, uuid_t uuid)
{
    char str[UUID_STR_LEN + 1];
    char *name;

    uuid_unparse( uuid, str );
    name = (char *) malloc( strlen( inv_path ) + 1  + strlen( str ) 
			     + strlen( INV_INVINDEX_PREFIX ) + 1);
    strcpy( name, inv_path );
    strcat( name, "/" );
    strcat( name, str );
    strcat( name, INV_INVINDEX_PREFIX );

    return(name);
}

char *
GetFstabFullPath(char *inv_path)
{
    char *fstabname;

    fstabname = (char *) malloc( strlen(inv_path) + 1 /* one for the "/" */
				   + strlen("fstab") + 1 );
    strcpy( fstabname, inv_path );
    strcat( fstabname, "/" );
    strcat( fstabname, "fstab" );
    return(fstabname);
}

void
CheckAndPruneFstab(char *inv_path, bool_t checkonly, char *mountPt,
	uuid_t *uuidp, uuid_t *sessionp, time32_t prunetime, char *r_mf_label)
{
    char	*fstabname;
    char	*mapaddr;
    char	*invname;
    int		fstabEntries;
    int		nEntries;
    int		fd;
    int		i;
    int		j;
    bool_t	removeflag;
    invt_fstab_t *fstabentry;
    invt_counter_t *counter,cnt;

    if (mountPt == NULL && uuid_is_null(*uuidp) && uuid_is_null(*sessionp)) {
	fprintf( stderr, "%s: mountpoint, uuid or session "
			 "must be specified\n", g_programName );
	fprintf( stderr, "%s: abnormal termination\n", g_programName );
	exit(1);
    }

    fstabname = GetFstabFullPath(inv_path);
    fd = open_and_lock( fstabname,
			FILE_WRITE,
			wait_for_locks );
    if (fd < 0) {
	fprintf( stderr, "%s: abnormal termination\n", g_programName );
	exit(1);
    }

    read_n_bytes(fd, &cnt, sizeof(invt_counter_t), fstabname);
    nEntries = cnt.ic_curnum;

    mapaddr = mmap_n_bytes(fd,
			   nEntries*sizeof(invt_fstab_t) + sizeof(invt_counter_t),
			   BOOL_FALSE,
			   fstabname);

    counter = (invt_counter_t *)mapaddr;
    fstabentry = (invt_fstab_t *)(mapaddr + sizeof(invt_counter_t));

    printf("Processing file %s\n", fstabname);

    /* check each entry in fstab for mount pt or uuid match */
    for (i = 0; i < counter->ic_curnum; )
    {
	removeflag = BOOL_FALSE;

	printf("   Found entry for %s\n" , fstabentry[i].ft_mountpt);

	for (j = i +1 ; j < counter->ic_curnum ; j++ ) {
	    if (uuid_compare(fstabentry[i].ft_uuid, fstabentry[j].ft_uuid) == 0)
	    {
		printf("     duplicate fstab entry\n");
		removeflag = BOOL_TRUE;
		break;
	    }
	}
	
	if (!removeflag)
	{
	    bool_t IdxCheckOnly = BOOL_TRUE;

	    invname = GetNameOfInvIndex(inv_path, fstabentry[i].ft_uuid);

#ifdef INV_DEBUG
	    printf("INV_DEBUG: ft_mountpt = %s, mountPt = %s, r_mf_label = %s\n", 
		   fstabentry[i].ft_mountpt, mountPt,
		   (r_mf_label ? r_mf_label : "(NULL)"));
#endif

	    if ( checkonly == BOOL_FALSE ) {
		if(mountPt != NULL && strcmp(mountPt, fstabentry[i].ft_mountpt) == 0) {
		    printf("     Match on hostname and path\n");
		    IdxCheckOnly = BOOL_FALSE;
		}
		else if((!uuid_is_null(*uuidp))
		   && (uuid_compare(*uuidp, fstabentry[i].ft_uuid) == 0)) {
		    printf("     Match on UUID only\n");
		    IdxCheckOnly = BOOL_FALSE;
		}
		else if(mountPt != NULL && mntpnt_equal(mountPt, fstabentry[i].ft_mountpt)) {
		    printf("     Match on directory name only: %s\n", mountPt);
		    IdxCheckOnly = BOOL_FALSE;
		}
		else if (!uuid_is_null(*sessionp)) {
		    // no session id to match against yet, defer deciding if this is
		    // a check-only run until we have a session
		    IdxCheckOnly = BOOL_FALSE;
		}
	    }

	    if (CheckAndPruneInvIndexFile(
			IdxCheckOnly, invname, sessionp,
			prunetime, r_mf_label) == -1 ) {
		removeflag = BOOL_TRUE;
	    }

	    free( invname );

	}

	if (removeflag == BOOL_TRUE)
	{
	    printf("     removing fstab entry %s\n", fstabentry[i].ft_mountpt);
	    if ( counter->ic_curnum > 1 ) {
		bcopy((void *)&fstabentry[i + 1],
		      (void *)&fstabentry[i],
		      (sizeof(invt_fstab_t) * (counter->ic_curnum - i - 1)));
	    }
	    counter->ic_curnum--;
	}
	else 
	    i++; /* next entry if this entry not removed */
    }

    fstabEntries = counter->ic_curnum;

    munmap( mapaddr,
	    (nEntries*sizeof(invt_fstab_t)) + sizeof(invt_counter_t));

    if ((fstabEntries != 0)  && (fstabEntries != nEntries)) {
	if(debug) {
	    printf("ftruncate fstab from %d to %d (%ld bytes)\n",
		   nEntries,
		   fstabEntries,
		   (long) sizeof(invt_counter_t) + (sizeof(invt_fstab_t) * fstabEntries));
	    printf("ftruncate %s\n", fstabname);
	}
	ftruncate(fd,
		  sizeof(invt_counter_t) + (sizeof(invt_fstab_t) * fstabEntries));
    }

    close(fd);

    if (fstabEntries == 0)
    {
	if(debug) {
	    printf("unlink fstab file %s\n", fstabname);
	}
	unlink( fstabname );
    }

    free( fstabname );
}

int
CheckAndPruneInvIndexFile( bool_t checkonly,
			   char *idxFileName,
			   uuid_t *sessionp,
			   time32_t prunetime,
			   char *r_mf_label) 
{
    char	*temp;
    int		fd;
    int		i;
    int		validEntries;
    int		nEntries;
    bool_t	removeflag;

    invt_entry_t *invIndexEntry;
    invt_counter_t *counter;
    invt_counter_t header;

    printf("      processing index file \n"
	   "       %s\n",idxFileName);

    fd = open_and_lock( idxFileName,
			FILE_WRITE,
			wait_for_locks );
    if (fd < 0) {
        return -1;
    }

    read_n_bytes( fd, &header, sizeof(invt_counter_t), idxFileName);
    nEntries = header.ic_curnum;

    temp = mmap_n_bytes(fd,
			nEntries*sizeof(invt_entry_t) + sizeof(invt_counter_t),
			BOOL_FALSE, idxFileName);

    counter = (invt_counter_t *)temp;
    invIndexEntry = (invt_entry_t *)( temp + sizeof(invt_counter_t));

    for (i=0; i < counter->ic_curnum; )
    {
	removeflag = BOOL_FALSE;
	printf("         Checking access for\n"
	       "          %s\n", invIndexEntry[i].ie_filename);
	if (debug) {
	    printf("          Time:\tbegin  %s",
		   ctime32(&(invIndexEntry[i].ie_timeperiod.tp_start)));
	    printf("\t\tend    %s",
		   ctime32(&(invIndexEntry[i].ie_timeperiod.tp_end)));
	}

	if (( access( invIndexEntry[i].ie_filename, R_OK | W_OK ) == -1)  &&
	   (errno == ENOENT))
	{
	    printf("         Unable to access %s referred in %s\n",
		   invIndexEntry[i].ie_filename, idxFileName);
	    printf("         removing index entry \n");
	    removeflag = BOOL_TRUE;
	}    

	if (CheckAndPruneStObjFile(checkonly, invIndexEntry[i].ie_filename,
		    sessionp, prunetime, r_mf_label) == -1) {
	    removeflag = BOOL_TRUE; /* The StObj is gone */
	}

	if (removeflag == BOOL_TRUE)
	{
	    if ( counter->ic_curnum > 1 ) {
	        bcopy((void *)&invIndexEntry[i + 1],
		      (void *)&invIndexEntry[i],
		      (sizeof(invt_entry_t) * (counter->ic_curnum - i - 1)));
	    }
	    counter->ic_curnum--;
	}
	else 
	    i++; /* next entry if this entry not removed */
    }

    validEntries = counter->ic_curnum;

    munmap( temp, (nEntries*sizeof(invt_entry_t)) + sizeof(invt_counter_t) );

    if ((validEntries != 0)  && (validEntries != nEntries)) {
	if(debug) {
	    printf("ftruncate idx from %d to %d (%ld bytes)\n",
		   nEntries,
		   validEntries,
		   (long) sizeof(invt_counter_t) + (validEntries * sizeof(invt_entry_t)) );
            printf("- truncate %s\n", idxFileName);
	}
    	ftruncate( fd,
		   sizeof(invt_counter_t) + (validEntries * sizeof(invt_entry_t)) );
    }

    if (debug) {
	printf("       pruned %d entries from this index file, %d remaining\n",
	       (nEntries - validEntries),
	       validEntries);
    }

    close( fd );

    if (validEntries == 0)
    {
	if(debug) {
	    printf("unlink idx file %s\n", idxFileName);
	}
	unlink( idxFileName );
	return(-1);
    }

    return(0);
}

int
CheckAndPruneStObjFile( bool_t checkonly,
			char *StObjFileName,
			uuid_t *sessionp,
			time32_t prunetime,
		        char *r_mf_label) 
{
    char	response[GEN_STRLEN];
    char	*temp;
    int		fd;
    int		i;
    int		validEntries;
    bool_t	removeflag;
    int		prunedcount = 0;
    int		removedcount = 0;
    bool_t	session_match;

    struct stat	sb;
    char	str[UUID_STR_LEN + 1];
    int		sescount;

    invt_seshdr_t *StObjhdr;
    invt_session_t *StObjses;

    invt_sescounter_t *counter;
    invt_sescounter_t header;

    fd = open_and_lock( StObjFileName,
			FILE_WRITE,
			wait_for_locks );
    if (fd < 0) {
        return -1;
    }

    read_n_bytes(fd, &header, sizeof(invt_sescounter_t), StObjFileName);

    if (fstat(fd, &sb) < 0) {
	fprintf(stderr, "Could not get stat info on %s\n", StObjFileName);
	perror("fstat");
	exit(1);
    }

    temp = mmap_n_bytes(fd, sb.st_size, BOOL_FALSE, StObjFileName);
    counter = (invt_sescounter_t *)temp;
    StObjhdr = (invt_seshdr_t *)( temp + sizeof(invt_sescounter_t));
    StObjses = (invt_session_t *)(temp + StObjhdr->sh_sess_off);

    sescount = 0;
    for (i=0; i < counter->ic_curnum; ) {
	removeflag = BOOL_FALSE;
	if (StObjhdr->sh_pruned)
	    prunedcount++;
	
	if (! StObjhdr->sh_pruned) {
	    printf("            Session %d: %s %s", 
		   sescount++,
		   StObjses->s_mountpt,
		   ctime32(&StObjhdr->sh_time));
	}
	if (debug) {
	    /* Note that the DMF people use some of this debug
	     * output for their interface to the inventory.
	     * They care about the following fields:
	     *	media label:
	     *	interrupted:
	     *	session label:
	     *	level:
	     * Do not change these fields w/out talking to
	     * them first.
	     */
	    int i;
	    int j;
	    invt_stream_t	*StObjstrm;
	    invt_mediafile_t *StObjmed;

	    if (StObjhdr->sh_pruned)
		printf("            Pruned Session: %s %s", 
		       StObjses->s_mountpt,
		       ctime32(&StObjhdr->sh_time));
	    printf("\t\tdevice:\t\t%s\n", StObjses->s_devpath);
	    printf("\t\tsession label:\t\"%s\"\n", StObjses->s_label);
	    uuid_unparse( StObjses->s_sesid, str );
	    printf("\t\tsession id:\t%s\n", str);
	    printf("\t\tlevel:\t\t%d\n", StObjhdr->sh_level);
	    printf("\t\tstreams:\t%d\n", StObjses->s_cur_nstreams );
	    for ( i = 0; i < (int) StObjses->s_cur_nstreams; i++ ) {
		printf( "\t\tstream %d:\n", i);
		StObjstrm = (invt_stream_t *)(temp + 
					      StObjhdr->sh_streams_off +
					      (i * sizeof(invt_stream_t)));
		printf( "\t\t\tpathname:\t%s\n",
			StObjstrm->st_cmdarg);
		printf( "\t\t\tinode start:\t%llu\n",
			(unsigned long long) StObjstrm->st_startino.ino);
		printf( "\t\t\tinode   end:\t%llu\n",
			(unsigned long long) StObjstrm->st_endino.ino);
		printf( "\t\t\tinterrupted:\t%s\n",
			StObjstrm->st_interrupted ? "YES" : "NO" );
		printf( "\t\t\tmedia files:\t%d\n",
			StObjstrm->st_nmediafiles);
		for ( j = 0; j < StObjstrm->st_nmediafiles; j++) {
		    StObjmed = (invt_mediafile_t *)(temp + 
						    StObjstrm->st_firstmfile +
						    (j * sizeof(invt_mediafile_t)));
		    printf( "\t\t\tmfile file %d:\n", j);
		    printf( "\t\t\t\tmfile size:\t%lld\n",
			    (long long) StObjmed->mf_size);
		    printf( "\t\t\t\tmfile start:\t%llu\n",
			    (unsigned long long) StObjmed->mf_startino.ino);
		    printf( "\t\t\t\tmfile end:\t%llu\n",
			    (unsigned long long) StObjmed->mf_endino.ino);
		    printf( "\t\t\t\tmedia label:\t\"%s\"\n",
			    StObjmed->mf_label);
		}
	    }
	}

#ifdef INV_DEBUG
        printf("INV_DEBUG: sh_time = %d, prunetime = %d\n",
	       StObjhdr->sh_time, prunetime);
        printf("INV_DEBUG: checkonly = %d, sh_pruned = %d\n",
               checkonly, StObjhdr->sh_pruned); 
#endif

	session_match = !uuid_compare(*sessionp, StObjses->s_sesid);
	if (session_match) {
	    printf("\t\tMatch on session id\n");
	}

	if ((checkonly == BOOL_FALSE)
	    && (! StObjhdr->sh_pruned)
	    && ((StObjhdr->sh_time < prunetime) || session_match)
	    && (uses_specified_mf_label(StObjhdr, StObjses, temp, r_mf_label))){
	    bool_t GotResponse = BOOL_FALSE;

	    uuid_unparse( StObjses->s_sesid, str );
		
	    if(force) {
		printf("-------------------------------------------------\n");
		printf("Pruning this matching entry:\n");
		printf( "UUID\t\t:\t%s\nMOUNT POINT\t:\t%s\n"
			"DEV PATH\t:\t%s\n"
			"LABEL\t\t:\t%s\n"
			"TIME OF DUMP\t:\t%s",
                        str, StObjses->s_mountpt, StObjses->s_devpath,
                        StObjses->s_label, ctime32(&StObjhdr->sh_time));
		removeflag = BOOL_TRUE;
	    }
	    else {
		printf("-------------------------------------------------\n");
		printf("An entry matching the mount point/time is :\n");
		printf("UUID\t\t:\t%s\nMOUNT POINT\t:\t%s\n"
		       "DEV PATH\t:\t%s\nTIME OF DUMP\t:\t%s",
		       str, StObjses->s_mountpt, StObjses->s_devpath,
		       ctime32(&StObjhdr->sh_time));
		while ( GotResponse == BOOL_FALSE )
		{
		    char *chp;

		    printf("\nDo you want to prune this entry: [y/n] ");
		    fgets( response, GEN_STRLEN, stdin );
		    chp = strchr( response, '\n');  
		    if (chp)
			*chp = '\0';   
		    if (strcasecmp( response, "Y" ) == 0) {
			removeflag = BOOL_TRUE;
			GotResponse = BOOL_TRUE;
		    }
		    else if (strcasecmp( response, "N" ) == 0) {
			GotResponse = BOOL_TRUE;
		    }
		}
	    }
	    printf("-------------------------------------------------\n\n");
	}

	if (removeflag == BOOL_TRUE) {
	    /* Mark this entry as pruned */
	    StObjhdr->sh_pruned = 1;
	    removedcount++;
	}

	i++;

	StObjhdr = (invt_seshdr_t *)( temp + sizeof(invt_sescounter_t) +
				      (i * sizeof(invt_seshdr_t)) );
	StObjses = (invt_session_t *)(temp + StObjhdr->sh_sess_off);
    }

    validEntries = counter->ic_curnum - prunedcount - removedcount;

    munmap( temp, sb.st_size);

    if (debug && removedcount)
	printf("       pruned %d entries from this StObj file,"
	       " %d remaining\n", removedcount, validEntries);

    close( fd );

    if (validEntries == 0)
    {
	if (debug)
	    printf("Removing: %s\n", StObjFileName);
	unlink( StObjFileName );
	return(-1);
    }

    return(0);
}

/* If any of the media objects have a label which matches the one supplied
 * by the user, or if none is supplied, or if the session has no media
 * objects, say the session is prunable.
 * Otherwise, say it should be bypassed.
 */

int
uses_specified_mf_label(
	invt_seshdr_t *StObjhdr,
	invt_session_t *StObjses,
	char *temp,
	char *r_mf_label)
{
    int			s, m;
    int			num_media_objects = 0;
    invt_stream_t	*StObjstrm;
    invt_mediafile_t	*StObjmed;

    if (!r_mf_label) {
	return 1;	/* prune */
    }
    for ( s = 0; s < (int) StObjses->s_cur_nstreams; s++ ) {
	StObjstrm = (invt_stream_t *)
	    (temp + StObjhdr->sh_streams_off + (s * sizeof(invt_stream_t)));
	for ( m = 0; m < StObjstrm->st_nmediafiles; m++, num_media_objects++) {
	    StObjmed = (invt_mediafile_t *)
		(temp + StObjstrm->st_firstmfile + (m * sizeof(invt_mediafile_t)));
	    if (!strncmp(StObjmed->mf_label, r_mf_label, 
			sizeof(StObjmed->mf_label))) {
		printf("\t\tSpecified media label \"%s\" found - "
			"pruning session \"%s\"\n",
			r_mf_label, StObjses->s_label);
		return 1;	/* prune */
	    }
	}
    }

    if (!num_media_objects)
	    return 1; /* prune */

    return 0;	/* don't prune */
}


/* copied from inv_api.c */
char *
inv_DEBUG_lock_str( int c )
{
#ifdef USE_LOCKF
    static char *lockstr[] = {
	"F_ULOCK",
	"F_LOCK",
	"F_TLOCK",
	"F_TEST",
    };

    if (c < 0 || c >= (sizeof(lockstr) / sizeof(char *))) {
	return "unknown lockf";
    }

    return lockstr[c];
#else
#ifdef USE_FLOCK
    int i;

    static struct {
	int cmd;
	char *str;
    } lockinfo[] = {
	{ LOCK_UN, "LOCK_UN" },
	{ LOCK_SH, "LOCK_SH" },
	{ LOCK_EX, "LOCK_EX" },
	{ LOCK_NB, "LOCK_NB" },
	{ NULL, NULL }
    };

    for (i = 0; lockinfo[i].str; i++) {
	if(lockinfo[i].cmd == c) {
	    return lockinfo[i].str;
	}
    }

    return "unknown flock";
#else
    return "unknown lock mechanism";
#endif /* USE_FLOCK */
#endif /* USE_LOCKF */
}

/*
 *      returns file descriptor >= 0 if open and lock successful.
 *
 *      returns LOCK_BUSY if lock busy.
 *      returns SYSCALL_FAILED for other failures.
 */
int
open_and_lock(char * path, Open_t open_type, uint lock_wait_flag)
{
    int     fd;
    int     rc;
    int     open_mode, lock_mode;

    switch (open_type) {
    case FILE_READ:
	open_mode = O_RDONLY;
	lock_mode = 0;
	break;
    case FILE_WRITE:
	open_mode = O_RDWR;
	lock_mode = LOCK_EX;
	break;
    case FILE_UNSAFE:
	open_mode = O_RDWR;
	lock_mode = 0;
	break;
    case DIRECTORY_READ:
	open_mode = O_RDONLY;
	lock_mode = 0;
	break;
    case DIRECTORY_WRITE:
	open_mode = O_RDWR;
	lock_mode = LOCK_EX;
	break;
    case DIRECTORY_UNSAFE:
	open_mode = O_RDONLY;
	lock_mode = 0;
	break;
    default:
	abort();
    }

    fd = open(path, open_mode);
    if (fd < 0) {
	fprintf( stderr, "%s: open of %s failed.\n",
		 g_programName, path);
	perror( "open" );
	return SYSCALL_FAILED;
    }

    if (lock_mode != 0) {
	int     local_mode = lock_mode;

	if (lock_wait_flag == 0)
	    local_mode &= ~LOCK_NB;

	rc = INVLOCK(fd, local_mode);
	if (rc < 0) {
	    if (errno == EACCES || errno == EWOULDBLOCK) {
		fprintf(stderr,"%s: Unable to get lock on "
			"file %s\n",
			g_programName,
			path);
		fprintf(stderr,"Conflict with other instances of"
			" xfsinvutil, xfsdump or xfsrestore.\n");
		rc = LOCK_BUSY;
	    } else {
		fprintf(stderr,"%s: lock on %s (fd == %d) failed.\n",
			g_programName,
			path,
			fd);
		perror("file lock");
		rc = SYSCALL_FAILED;
	    }
	    close(fd);
	    return rc;
	}
    }

    if (debug) {
	char    *lstr;

	switch(lock_mode) {
	case LOCK_EX: lstr = inv_DEBUG_lock_str(lock_mode); break;
	default:       lstr = "NO_LOCK";
	}

	fprintf(stderr, "open_and_lock: DEBUG open_mode %s"
		" lock_mode %s file %s\n",
		open_mode == O_RDONLY ? "O_RDONLY" : "O_RDWR", lstr, path);
    }

    return fd;
}

void
read_n_bytes(int fd, void * buf, size_t count, char *path)
{
    int     rc;

    rc = read(fd, buf, count);
    if (rc < 0) {
	fprintf(stderr, "%s: read of %ld bytes on %s failed.\n",
		g_programName, (long) count, path);
	perror(path);
	exit (2);
    } else  if (rc != count) {
	fprintf(stderr, "%s: read of %ld bytes on %s failed.\n",
		g_programName, (long) count, path);
	fprintf(stderr, "Tried to read %ld bytes, got %d.\n",
		(long) count, rc);
	exit (2);
    }
}

void
write_n_bytes(int fd, void * buf, size_t count, char *path)
{
    int     rc;

    rc = write(fd, buf, count);
    if (rc < 0) {
	fprintf(stderr, "%s: write of %ld bytes on %s failed.\n",
		g_programName, (long) count, path);
	perror(path);
	exit (2);
    } else if (rc != count) {
	fprintf(stderr, "%s: write of %ld bytes on %s failed.\n",
		g_programName, (long) count, path);
	fprintf(stderr, "Tried to write %ld bytes, wrote %d.\n",
		(long) count, rc);
	exit (2);
    }
}

void *
mmap_n_bytes(int fd, size_t count, bool_t checkonly, char *path)
{
    void *  temp;

    lseek( fd, 0, SEEK_SET );
    temp = mmap( NULL, count,
		 (checkonly == BOOL_TRUE) ? PROT_READ : PROT_READ|PROT_WRITE,
		 MAP_SHARED, fd, 0 );
	
    if (temp == (void *)-1) {
	fprintf( stderr, "%s: error in mmap of %ld bytes for file %s\n",
		 g_programName, (long) count, path);
	perror("mmap");
	fprintf( stderr, "%s: abnormal termination\n", g_programName );
	exit(1);
    }
    return temp;
}

void
usage (void)
{
    int pfxsz;

    fprintf(stderr, "%s: %s\n", g_programName, g_programVersion);
    pfxsz = fprintf(stderr, "Usage: \n");
    fprintf(stderr, "%*s%s [-F|-i] [-m media_label] -M mount_point mm/dd/yyyy\n",
		    pfxsz, "", g_programName);
    fprintf(stderr, "%*s%s [-F|-i] [-m media_label] -u UUID mm/dd/yyyy\n",
		    pfxsz, "", g_programName);
    fprintf(stderr, "%*s%s [-F] -s SESSION_ID\n",
		    pfxsz, "", g_programName);
    fprintf(stderr, "%*s%s -i\n", pfxsz, "", g_programName);
    fprintf(stderr, "%*s%s -C\n", pfxsz, "", g_programName);

    exit(1);
}

