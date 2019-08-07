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

#include <sys/param.h>
#include <strings.h>
#include <assert.h>
#include <uuid/uuid.h>

#include "config.h"

#include "arch_xlate.h"
#include "types.h"
#include "global.h"
#include "content.h"
#include "content_inode.h"
#include "drive.h"
#include "media.h"
#include "inomap.h"
#include "rec_hdr.h"
#include "inv_priv.h"
#include "mlog.h"

#define IXLATE(a1,a2,MEMBER)	\
	INT_XLATE((a1)->MEMBER, (a2)->MEMBER, dir, ARCH_CONVERT)
#define BXLATE(MEMBER)		\
	bcopy(&(ptr1)->MEMBER, &(ptr2)->MEMBER, sizeof((ptr1)->MEMBER))

/*
 * xlate_global_hdr - endian convert struct global_hdr
 *
 * Note: gh_upper field is not converted.  This must be done elsewhere
 *       at the time of assignment, because its contents are unknown.
 */
void
xlate_global_hdr(global_hdr_t *gh1, global_hdr_t *gh2, int dir)
{
	global_hdr_t *ptr1 = gh1;
	global_hdr_t *ptr2 = gh2;

	IXLATE(gh1, gh2, gh_version);
	IXLATE(gh1, gh2, gh_timestamp);
	IXLATE(gh1, gh2, gh_ipaddr);
	IXLATE(gh1, gh2, gh_checksum);

	if (dir < 0) {
		ptr1 = gh2;
		ptr2 = gh1;
	}

	BXLATE(gh_magic);
	BXLATE(gh_dumpid);
	BXLATE(gh_hostname);
	BXLATE(gh_dumplabel);
	BXLATE(gh_upper);
	BXLATE(gh_pad1);
	BXLATE(gh_pad2);
	BXLATE(gh_pad3);

	mlog(MLOG_NITTY, "xlate_global_hdr: pre-xlate\n"
	     "\tgh_magic %.100s\n"
	     "\tgh_version %u\n"
	     "\tgh_checksum %u\n"
	     "\tgh_timestamp %u\n"
	     "\tgh_ipaddr %llu\n"
	     "\tgh_hostname %.100s\n"
	     "\tgh_dumplabel %.100s\n",
	     ptr1->gh_magic,
	     ptr1->gh_version,
	     ptr1->gh_checksum,
	     ptr1->gh_timestamp,
	     ptr1->gh_ipaddr,
	     ptr1->gh_hostname,
	     ptr1->gh_dumplabel);

	mlog(MLOG_NITTY, "xlate_global_hdr: post-xlate\n"
	     "\tgh_magic %.100s\n"
	     "\tgh_version %u\n"
	     "\tgh_checksum %u\n"
	     "\tgh_timestamp %u\n"
	     "\tgh_ipaddr %llu\n"
	     "\tgh_hostname %.100s\n"
	     "\tgh_dumplabel %.100s\n",
	     ptr2->gh_magic,
	     ptr2->gh_version,
	     ptr2->gh_checksum,
	     ptr2->gh_timestamp,
	     ptr2->gh_ipaddr,
	     ptr2->gh_hostname,
	     ptr2->gh_dumplabel);

}

/*
 * xlate_drive_hdr - endian convert struct xlate_drive_hdr
 * which is loaded into global_hdr.gh_upper
 */
void
xlate_drive_hdr(drive_hdr_t *dh1, drive_hdr_t *dh2, int dir)
{
	drive_hdr_t *ptr1 = dh1;
	drive_hdr_t *ptr2 = dh2;

	IXLATE(dh1, dh2, dh_drivecnt);
	IXLATE(dh1, dh2, dh_driveix);
	IXLATE(dh1, dh2, dh_strategyid);

	if (dir < 0) {
		ptr1 = dh2;
		ptr2 = dh1;
	}

	BXLATE(dh_pad1);
	BXLATE(dh_specific);
	BXLATE(dh_upper);

	mlog(MLOG_NITTY, "xlate_drive_hdr: pre-xlate\n"
	     "\tdh_drivecnt %u\n"
	     "\tdh_driveix %u\n"
	     "\tdh_strategyid %d\n"
	     "\tdh_pad1 %s\n"
	     "\tdh_specific %s\n"
	     "\tdh_upper %s\n",
	     ptr1->dh_drivecnt, 
	     ptr1->dh_driveix,
	     ptr1->dh_strategyid,
	     ptr1->dh_pad1,
	     ptr1->dh_specific,
	     ptr1->dh_upper);	     

	mlog(MLOG_NITTY, "xlate_drive_hdr: post-xlate\n"
	     "\tdh_drivecnt %u\n"
	     "\tdh_driveix %u\n"
	     "\tdh_strategyid %d\n"
	     "\tdh_pad1 %s\n"
	     "\tdh_specific %s\n"
	     "\tdh_upper %s\n",
	     ptr2->dh_drivecnt, 
	     ptr2->dh_driveix,
	     ptr2->dh_strategyid,
	     ptr2->dh_pad1,
	     ptr2->dh_specific,
	     ptr2->dh_upper);	     
}

/*
 * xlate_media_hdr - endian convert struct media_hdr
 * which is loaded into drive_hdr.dh_upper
 */
void
xlate_media_hdr(media_hdr_t *mh1, media_hdr_t *mh2, int dir)
{
	media_hdr_t *ptr1 = mh1;
	media_hdr_t *ptr2 = mh2;

	mlog(MLOG_NITTY, "xlate_media_hdr\n");

	IXLATE(mh1, mh2, mh_mediaix);
	IXLATE(mh1, mh2, mh_mediafileix);
	IXLATE(mh1, mh2, mh_dumpfileix);
	IXLATE(mh1, mh2, mh_dumpmediafileix);
	IXLATE(mh1, mh2, mh_dumpmediaix);
	IXLATE(mh1, mh2, mh_strategyid);

	if (dir < 0) {
		ptr1 = mh2;
		ptr2 = mh1;
	}

	BXLATE(mh_medialabel);
	BXLATE(mh_prevmedialabel);
	BXLATE(mh_pad1);
	BXLATE(mh_mediaid);
	BXLATE(mh_prevmediaid);
	BXLATE(mh_pad2);
	BXLATE(mh_pad3);
	BXLATE(mh_specific);
	BXLATE(mh_upper);
}

/*
 * xlate_content_hdr - endian convert struct content_hdr
 * which is loaded into media_hdr.mh_upper
 */
void
xlate_content_hdr(content_hdr_t *ch1, content_hdr_t *ch2, int dir)
{
	content_hdr_t *ptr1 = ch1;
	content_hdr_t *ptr2 = ch2;

	mlog(MLOG_NITTY, "xlate_content_hdr\n");

	IXLATE(ch1, ch2, ch_strategyid);

	if (dir < 0) {
		ptr1 = ch2;
		ptr2 = ch1;
	}

	BXLATE(ch_mntpnt);
	BXLATE(ch_fsdevice);
	BXLATE(ch_pad1);
	BXLATE(ch_fstype);
	BXLATE(ch_fsid);
	BXLATE(ch_pad2);
	BXLATE(ch_pad3);
	BXLATE(ch_pad4);
	BXLATE(ch_specific);
}

/*
 * xlate_content_inode_hdr - endian convert struct content_inode_hdr
 * which is loaded into content_hdr.ch_specific
 */
void
xlate_content_inode_hdr(content_inode_hdr_t *cih1, content_inode_hdr_t *cih2, int dir)
{
	content_inode_hdr_t *ptr1 = cih1;
	content_inode_hdr_t *ptr2 = cih2;

	mlog(MLOG_NITTY, "xlate_content_inode_hdr\n");

	IXLATE(cih1, cih2, cih_mediafiletype);
	IXLATE(cih1, cih2, cih_dumpattr);
	IXLATE(cih1, cih2, cih_level);
	IXLATE(cih1, cih2, cih_last_time);
	IXLATE(cih1, cih2, cih_resume_time);
	IXLATE(cih1, cih2, cih_rootino);
	IXLATE(cih1, cih2, cih_inomap_hnkcnt);
	IXLATE(cih1, cih2, cih_inomap_segcnt);
	IXLATE(cih1, cih2, cih_inomap_dircnt);
	IXLATE(cih1, cih2, cih_inomap_nondircnt);
	IXLATE(cih1, cih2, cih_inomap_firstino);
	IXLATE(cih1, cih2, cih_inomap_lastino);
	IXLATE(cih1, cih2, cih_inomap_datasz);

	if (dir < 0) {
		ptr1 = cih2;
		ptr2 = cih1;
	}

	BXLATE(pad1);
	BXLATE(cih_last_id);
	BXLATE(cih_resume_id);
	BXLATE(cih_pad2);

	xlate_startpt(&cih1->cih_startpt, &cih2->cih_startpt, dir);
	xlate_startpt(&cih1->cih_endpt,   &cih2->cih_endpt,   dir);
}

/*
 * xlate_startpt - endian convert struct startpt
 */
void
xlate_startpt(startpt_t *sp1, startpt_t *sp2, int dir)
{
	mlog(MLOG_NITTY, "xlate_startpt\n");

	IXLATE(sp1, sp2, sp_ino);
	IXLATE(sp1, sp2, sp_offset);
	IXLATE(sp1, sp2, sp_flags);
	IXLATE(sp1, sp2, sp_pad1);
}

/*
 * xlate_hnk - endian convert struct hnk
 * Note: struct hnk is defined in 3 different inomap.c files but they're
 *       all the same.
 */
void
xlate_hnk(hnk_t *h1, hnk_t *h2, int dir)
{
	hnk_t *ptr1 = h1;
	hnk_t *ptr2 = h2;
	int i;

	mlog(MLOG_NITTY, "pre - xlate_hnk\n");

	for(i = 0; i < SEGPERHNK; i++) {
		IXLATE(h1, h2, seg[i].base);
		IXLATE(h1, h2, seg[i].lobits);
		IXLATE(h1, h2, seg[i].mebits);
		IXLATE(h1, h2, seg[i].hibits);
	}

	IXLATE(h1, h2, maxino);
	h2->nextp = NULL;

	if (dir < 0) {
		ptr1 = h2;
		ptr2 = h1;
	}

	BXLATE(pad);

	mlog(MLOG_NITTY, "post - xlate_hnk\n");
}

/*
 * xlate_filehdr - endian convert struct filehdr
 */
void
xlate_filehdr(filehdr_t *fh1, filehdr_t *fh2, int dir)
{
	filehdr_t *ptr1 = fh1;
	filehdr_t *ptr2 = fh2;

	IXLATE(fh1, fh2, fh_offset);
	IXLATE(fh1, fh2, fh_flags);
	IXLATE(fh1, fh2, fh_checksum);
	xlate_bstat(&fh1->fh_stat, &fh2->fh_stat, dir);

	if (dir < 0) {
		ptr1 = fh2;
		ptr2 = fh1;
	}

	BXLATE(fh_pad2);

	mlog(MLOG_NITTY, "xlate_filehdr: pre-xlate\n"
	     "\tfh_offset %llu\n"
	     "\tfh_flags %llu\n"
	     "\tfh_checksum %llu\n",
	     ptr1->fh_offset,
	     ptr1->fh_flags,
	     ptr1->fh_checksum);

	mlog(MLOG_NITTY, "xlate_filehdr: post-xlate\n"
	     "\tfh_offset %llu\n"
	     "\tfh_flags %llu\n"
	     "\tfh_checksum %llu\n",
	     ptr2->fh_offset,
	     ptr2->fh_flags,
	     ptr2->fh_checksum);
}

/*
 * xlate_bstat - endian convert struct bstat
 */
void
xlate_bstat(bstat_t *bs1, bstat_t *bs2, int dir)
{
	bstat_t *ptr1 = bs1;
	bstat_t *ptr2 = bs2;

	mlog(MLOG_NITTY, "xlate_bstat\n");

	IXLATE(bs1, bs2, bs_ino);
	IXLATE(bs1, bs2, bs_mode);
	IXLATE(bs1, bs2, bs_nlink);
	IXLATE(bs1, bs2, bs_uid);
	IXLATE(bs1, bs2, bs_gid);
	IXLATE(bs1, bs2, bs_rdev);
	IXLATE(bs1, bs2, bs_blksize);
	IXLATE(bs1, bs2, bs_size);

	IXLATE(bs1, bs2, bs_atime.tv_sec);
	IXLATE(bs1, bs2, bs_atime.tv_nsec);
	IXLATE(bs1, bs2, bs_mtime.tv_sec);
	IXLATE(bs1, bs2, bs_mtime.tv_nsec);
	IXLATE(bs1, bs2, bs_ctime.tv_sec);
	IXLATE(bs1, bs2, bs_ctime.tv_nsec);

	IXLATE(bs1, bs2, bs_blocks);
	IXLATE(bs1, bs2, bs_xflags);
	IXLATE(bs1, bs2, bs_extsize);
	IXLATE(bs1, bs2, bs_extents);
	IXLATE(bs1, bs2, bs_gen);
	IXLATE(bs1, bs2, bs_projid_lo);
	IXLATE(bs1, bs2, bs_forkoff);
	IXLATE(bs1, bs2, bs_projid_hi);
	IXLATE(bs1, bs2, bs_dmevmask);
	IXLATE(bs1, bs2, bs_dmstate);

	if (dir < 0) {
		ptr1 = bs2;
		ptr2 = bs1;
	}

	BXLATE(bs_pad);
	BXLATE(bs_pad1);

	mlog(MLOG_NITTY, "xlate_bstat: pre-xlate\n"
	     "\tbs_ino %llu\n"
	     "\tbs_mode  %lo\n",
	     ptr1->bs_ino,
	     ptr1->bs_mode);

	mlog(MLOG_NITTY, "xlate_bstat: post-xlate\n"
	     "\tbs_ino %llu\n"
	     "\tbs_mode  %lo\n",
	     ptr2->bs_ino,
	     ptr2->bs_mode);

}

/*
 * xlate_extenthdr - endian convert struct extenthdr
 */
void
xlate_extenthdr(extenthdr_t *eh1, extenthdr_t *eh2, int dir)
{
	extenthdr_t *ptr1 = eh1;
	extenthdr_t *ptr2 = eh2;
	
	mlog(MLOG_NITTY, "xlate_extenthdr\n");

	IXLATE(eh1, eh2, eh_sz);
	IXLATE(eh1, eh2, eh_offset);
	IXLATE(eh1, eh2, eh_type);
	IXLATE(eh1, eh2, eh_flags);
	IXLATE(eh1, eh2, eh_checksum);

	if (dir < 0) {
		ptr1 = eh2;
		ptr2 = eh1;
	}

	BXLATE(eh_pad);
}

/*
 * xlate_direnthdr - endian convert struct direnthdr
 */
void
xlate_direnthdr(direnthdr_t *dh1, direnthdr_t *dh2, int dir)
{
	direnthdr_t *ptr1 = dh1;
	direnthdr_t *ptr2 = dh2;

	IXLATE(dh1, dh2, dh_ino);
	IXLATE(dh1, dh2, dh_gen);
	IXLATE(dh1, dh2, dh_checksum);
	IXLATE(dh1, dh2, dh_sz);

	if (dir < 0) {
		ptr1 = dh2;
		ptr2 = dh1;
	}

	BXLATE(dh_name);

	mlog(MLOG_NITTY, "xlate_direnthdr: pre-xlate\n"
	     "\tdh_ino %llu\n"
	     "\tdh_gen %u\n"
	     "\tdh_checksum %d\n"
	     "\tdh_sz %d\n"
	     "\tdh_name %.8s\n",
	     ptr1->dh_ino,
	     ptr1->dh_gen,
	     ptr1->dh_checksum,
	     ptr1->dh_sz,
	     ptr1->dh_name );

	mlog(MLOG_NITTY, "xlate_direnthdr: post-xlate\n"
	     "\tdh_ino %llu\n"
	     "\tdh_gen %u\n"
	     "\tdh_checksum %d\n"
	     "\tdh_sz %d\n"
	     "\tdh_name %.8s\n",
	     ptr2->dh_ino,
	     ptr2->dh_gen,
	     ptr2->dh_checksum,
	     ptr2->dh_sz,
	     ptr2->dh_name );
}

/*
 * xlate_direnthdr_v1 - endian convert struct direnthdr_v1
 */
void
xlate_direnthdr_v1(direnthdr_v1_t *dh1, direnthdr_v1_t *dh2, int dir)
{
	direnthdr_v1_t *ptr1 = dh1;
	direnthdr_v1_t *ptr2 = dh2;

	IXLATE(dh1, dh2, dh_ino);
	IXLATE(dh1, dh2, dh_gen);
	IXLATE(dh1, dh2, dh_sz);
	IXLATE(dh1, dh2, dh_checksum);

	if (dir < 0) {
		ptr1 = dh2;
		ptr2 = dh1;
	}

	BXLATE(dh_name);

	mlog(MLOG_NITTY, "xlate_direnthdr_v1: pre-xlate\n"
	     "\tdh_ino %llu\n"
	     "\tdh_gen %u\n"
	     "\tdh_sz %d\n"
	     "\tdh_checksum %d\n"
	     "\tdh_name %.8s\n",
	     ptr1->dh_ino,
	     ptr1->dh_gen,
	     ptr1->dh_sz,
	     ptr1->dh_checksum,
	     ptr1->dh_name );

	mlog(MLOG_NITTY, "xlate_direnthdr_v1: post-xlate\n"
	     "\tdh_ino %llu\n"
	     "\tdh_gen %u\n"
	     "\tdh_sz %d\n"
	     "\tdh_checksum %d\n"
	     "\tdh_name %.8s\n",
	     ptr2->dh_ino,
	     ptr2->dh_gen,
	     ptr2->dh_sz,
	     ptr2->dh_checksum,
	     ptr2->dh_name );	     
}

/*
 * xlate_extattrhdr - endian convert struct extattrhdr
 */
void
xlate_extattrhdr(extattrhdr_t *eh1, extattrhdr_t *eh2, int dir)
{
	mlog(MLOG_NITTY, "xlate_extattrhdr\n");

	IXLATE(eh1, eh2, ah_sz);
	IXLATE(eh1, eh2, ah_valoff);
	IXLATE(eh1, eh2, ah_flags);
	IXLATE(eh1, eh2, ah_valsz);
	IXLATE(eh1, eh2, ah_checksum);
}

/*
 * xlate_rec_hdr - endian convert struct rec_hdr
 */
void
xlate_rec_hdr(rec_hdr_t *rh1, rec_hdr_t *rh2, int dir)
{
	rec_hdr_t *ptr1 = rh1;
	rec_hdr_t *ptr2 = rh2;

	mlog(MLOG_NITTY, "xlate_rec_hdr\n");
  
	IXLATE(rh1, rh2, magic);
	IXLATE(rh1, rh2, version);
	IXLATE(rh1, rh2, blksize);
	IXLATE(rh1, rh2, recsize);
	IXLATE(rh1, rh2, capability);
	IXLATE(rh1, rh2, file_offset);
	IXLATE(rh1, rh2, first_mark_offset);
	IXLATE(rh1, rh2, rec_used);
	IXLATE(rh1, rh2, checksum);
	IXLATE(rh1, rh2, ischecksum);

	if (dir < 0) {
		ptr1 = rh2;
		ptr2 = rh1;
	}

	BXLATE(pad1);
	BXLATE(dump_uuid);
	BXLATE(pad2);	

	mlog(MLOG_NITTY, "xlate_rec_hdr: pre-xlate\n"
	     "\tmagic %llu\n"
	     "\tversion %d\n"
	     "\tblksize %d\n"
	     "\trecsize %d\n"
	     "\tcapability %d\n"
	     "\tfile_offset %lld\n"
	     "\tfirst_mark_offset %lld\n"
	     "\trec_used %u\n"
	     "\tchecksum %d\n"
	     "\tischecksum %d\n",
	     ptr1->magic, 
	     ptr1->version,
	     ptr1->blksize,
	     ptr1->recsize,
	     ptr1->capability,
	     ptr1->file_offset,
	     ptr1->first_mark_offset,
	     ptr1->rec_used,
	     ptr1->checksum,
	     ptr1->ischecksum);

	mlog(MLOG_NITTY, "xlate_rec_hdr: post-xlate\n"
	     "\tmagic %llu\n"
	     "\tversion %d\n"
	     "\tblksize %d\n"
	     "\trecsize %d\n"
	     "\tcapability %d\n"
	     "\tfile_offset %lld\n"
	     "\tfirst_mark_offset %lld\n"
	     "\trec_used %u\n"
	     "\tchecksum %d\n"
	     "\tischecksum %d\n",
	     ptr2->magic, 
	     ptr2->version,
	     ptr2->blksize,
	     ptr2->recsize,
	     ptr2->capability,
	     ptr2->file_offset,
	     ptr2->first_mark_offset,
	     ptr2->rec_used,
	     ptr2->checksum,
	     ptr2->ischecksum);
}

/*
 * endian convert inventory structures
 */
void
xlate_invt_seshdr(invt_seshdr_t *ish1, invt_seshdr_t *ish2, int dir)
{	
	invt_seshdr_t *ptr1 = ish1;
	invt_seshdr_t *ptr2 = ish2;

	mlog(MLOG_NITTY, "xlate_invt_seshdr\n");

	IXLATE(ish1, ish2, sh_sess_off);
	IXLATE(ish1, ish2, sh_streams_off);
	IXLATE(ish1, ish2, sh_time);
	IXLATE(ish1, ish2, sh_flag);

	if (dir < 0) {
		ptr1 = ish2;
		ptr2 = ish1;
	}

	BXLATE(sh_level);
	BXLATE(sh_pruned);
	BXLATE(sh_padding);
}

void
xlate_invt_session(invt_session_t *is1, invt_session_t *is2, int dir)
{
	invt_session_t *ptr1 = is1;
	invt_session_t *ptr2 = is2;

	mlog(MLOG_NITTY, "xlate_invt_session\n");

	IXLATE(is1, is2, s_cur_nstreams);
	IXLATE(is1, is2, s_max_nstreams);

	if (dir < 0) {
		ptr1 = is2;
		ptr2 = is1;
	}

	BXLATE(s_sesid);
	BXLATE(s_fsid);
	BXLATE(s_label);
	BXLATE(s_mountpt);
	BXLATE(s_devpath);
	BXLATE(s_padding);

	mlog(MLOG_NITTY, "xlate_invt_session: pre-xlate\n"
	     "\ts_cur_nstreams %u\n"
	     "\ts_max_nstreams %u\n",
	     ptr1->s_cur_nstreams,
	     ptr1->s_max_nstreams);

	mlog(MLOG_NITTY, "xlate_invt_session: post-xlate\n"
	     "\ts_cur_nstreams %u\n"
	     "\ts_max_nstreams %u\n",
	     ptr2->s_cur_nstreams,
	     ptr2->s_max_nstreams);

}

void
xlate_invt_breakpt(invt_breakpt_t *ib1, invt_breakpt_t *ib2, int dir)
{
	mlog(MLOG_NITTY, "xlate_invt_breakpt\n");

	IXLATE(ib1, ib2, ino);
	IXLATE(ib1, ib2, offset);
}

void
xlate_invt_stream(invt_stream_t *ist1, invt_stream_t *ist2, int dir)
{
	invt_stream_t *ptr1 = ist1;
	invt_stream_t *ptr2 = ist2;

	mlog(MLOG_NITTY, "xlate_invt_stream\n");

	IXLATE(ist1, ist2, st_firstmfile);
	IXLATE(ist1, ist2, st_lastmfile);
	IXLATE(ist1, ist2, st_nmediafiles);
	IXLATE(ist1, ist2, st_interrupted);

	if (dir < 0) {
		ptr1 = ist2;
		ptr2 = ist1;
	}
	
	BXLATE(st_cmdarg);
	BXLATE(st_padding);

	xlate_invt_breakpt(&ist1->st_startino, &ist2->st_startino, dir);
	xlate_invt_breakpt(&ist1->st_endino,   &ist2->st_endino,   dir);
}

void
xlate_invt_mediafile(invt_mediafile_t *im1, invt_mediafile_t *im2, int dir)
{
	invt_mediafile_t *ptr1 = im1;
	invt_mediafile_t *ptr2 = im2;

	mlog(MLOG_NITTY, "xlate_invt_mediafile\n");

	IXLATE(im1, im2, mf_nextmf);
	IXLATE(im1, im2, mf_prevmf);
	IXLATE(im1, im2, mf_mfileidx);
	IXLATE(im1, im2, mf_size);

	if (dir < 0) {
		ptr1 = im2;
		ptr2 = im1;
	}

	BXLATE(mf_moid);
	BXLATE(mf_label);
	BXLATE(mf_flag);
	BXLATE(mf_padding);

	xlate_invt_breakpt(&im1->mf_startino, &im2->mf_startino, dir);
	xlate_invt_breakpt(&im1->mf_endino,   &im2->mf_endino,   dir);
}
