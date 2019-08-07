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

#ifndef ARCH_XLATE_H
#define ARCH_XLATE_H

#include <strings.h>
#include "types.h"
#include "global.h"
#include "content.h"
#include "content_inode.h"
#include "drive.h"
#include "media.h"
#include "inomap.h"
#include "rec_hdr.h"
#include "inv_priv.h"
#include "swap.h"

/*
 * xlate_global_hdr - endian convert struct global_hdr
 *
 * Note: gh_upper field is not converted.  This must be done elsewhere
 *       at the time of assignment, because its contents are unknown.
 */
void xlate_global_hdr(global_hdr_t *gh1, global_hdr_t *gh2, int dir);

/*
 * xlate_drive_hdr - endian convert struct xlate_drive_hdr
 * which is loaded as the gh_upper field on the global header
 */
void xlate_drive_hdr(drive_hdr_t *dh1, drive_hdr_t *dh2, int dir);

/*
 * xlate_media_hdr - endian convert struct media_hdr
 * which is loaded into drive_hdr.dh_upper
 */
void xlate_media_hdr(media_hdr_t *mh1, media_hdr_t *mh2, int dir);

/*
 * xlate_content_hdr - endian convert struct content_hdr
 * which is loaded into media_hdr.mh_upper
 */
void xlate_content_hdr(content_hdr_t *ch1, content_hdr_t *ch2, int dir);

/*
 * xlate_content_inode_hdr - endian convert struct content_inode_hdr
 * which is loaded into content_hdr.ch_specific
 */
void xlate_content_inode_hdr(content_inode_hdr_t *cih1, content_inode_hdr_t *cih2, int dir);

/*
 * xlate_startpt - endian convert struct startpt
 */
void xlate_startpt(startpt_t *sp1, startpt_t *sp2, int dir);

/*
 * xlate_hnk - endian convert struct hnk
 * Note: struct hnk is defined in 3 different inomap.h files but they're
 *       all the same.  Bad things will happen if they're not...
 */
void xlate_hnk(hnk_t *h1, hnk_t *h2, int dir);

/*
 * xlate_filehdr - endian convert struct filehdr
 */
void xlate_filehdr(filehdr_t *fh1, filehdr_t *fh2, int dir);

/*
 * xlate_bstat - endian convert struct bstat
 */
void xlate_bstat(bstat_t *bs1, bstat_t *bs2, int dir);

/*
 * xlate_extenthdr - endian convert struct extenthdr
 */
void xlate_extenthdr(extenthdr_t *eh1, extenthdr_t *eh2, int dir);

/*
 * xlate_direnthdr - endian convert struct direnthdr
 */
void xlate_direnthdr(direnthdr_t *dh1, direnthdr_t *dh2, int dir);

/*
 * xlate_direnthdr_v1 - endian convert struct direnthdr_v1
 */
void xlate_direnthdr_v1(direnthdr_v1_t *dh1, direnthdr_v1_t *dh2, int dir);

/*
 * xlate_extattrhdr - endian convert struct extattrhdr
 */
void xlate_extattrhdr(extattrhdr_t *eh1, extattrhdr_t *eh2, int dir);

/*
 * xlate_rec_hdr - endian convert struct rec_hdr
 */
void xlate_rec_hdr(rec_hdr_t *rh1, rec_hdr_t *rh2, int dir);

/*
 * endian convert inventory structures
 */
void xlate_invt_seshdr(invt_seshdr_t *ish1, invt_seshdr_t *ish2, int dir);
void xlate_invt_session(invt_session_t *is1, invt_session_t *is2, int dir);
void xlate_invt_breakpt(invt_breakpt_t *ib1, invt_breakpt_t *ib2, int dir);
void xlate_invt_stream(invt_stream_t *ist1, invt_stream_t *ist2, int dir);
void xlate_invt_mediafile(invt_mediafile_t *im1, invt_mediafile_t *im2, int dir);

#endif /* ARCH_XLATE_H */
