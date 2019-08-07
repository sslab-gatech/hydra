/*
 * Copyright (C) 2018 Oracle.  All Rights Reserved.
 *
 * Author: Darrick J. Wong <darrick.wong@oracle.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write the Free Software Foundation,
 * Inc.,  51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistr.h>
#include <uninorm.h>
#include "xfs.h"
#include "path.h"
#include "xfs_scrub.h"
#include "common.h"

/*
 * Detect collisions of Unicode-normalized names.
 *
 * Record all the name->ino mappings in a directory/xattr, with a twist!
 * The twist is that we perform unicode normalization on every name we
 * see, so that we can warn about a directory containing more than one
 * directory entries that normalize to the same Unicode string.  These
 * entries are at best a sign of Unicode mishandling, or some sort of
 * weird name substitution attack if the entries do not point to the
 * same inode.  Warn if we see multiple dirents that do not all point to
 * the same inode.
 *
 * For extended attributes we perform the same collision checks on the
 * attribute, though any collision is enough to trigger a warning.
 *
 * We flag these collisions as warnings and not errors because XFS
 * treats names as a sequence of arbitrary nonzero bytes.  While a
 * Unicode collision is not technically a filesystem corruption, we
 * ought to say something if there's a possibility for misleading a
 * user.
 *
 * To normalize, we use Unicode NFKC.  We use the composing
 * normalization mode (e.g. "E WITH ACUTE" instead of "E" then "ACUTE")
 * because that's what W3C (and in general Linux) uses.  This enables us
 * to detect multiple object names that normalize to the same name and
 * could be confusing to users.  Furthermore, we use the compatibility
 * mode to detect names with compatible but different code points to
 * strengthen those checks.
 */

struct name_entry {
	struct name_entry	*next;
	xfs_ino_t		ino;
	size_t			uninamelen;
	uint8_t			uniname[0];
};
#define NAME_ENTRY_SZ(nl)	(sizeof(struct name_entry) + 1 + \
				 (nl * sizeof(uint8_t)))

struct unicrash {
	struct scrub_ctx	*ctx;
	bool			compare_ino;
	size_t			nr_buckets;
	struct name_entry	*buckets[0];
};
#define UNICRASH_SZ(nr)		(sizeof(struct unicrash) + \
				 (nr * sizeof(struct name_entry *)))

/*
 * We only care about validating utf8 collisions if the underlying
 * system configuration says we're using utf8.  If the language
 * specifier string used to output messages has ".UTF-8" somewhere in
 * its name, then we conclude utf8 is in use.  Otherwise, no checking is
 * performed.
 *
 * Most modern Linux systems default to utf8, so the only time this
 * check will return false is if the administrator configured things
 * this way or if things are so messed up there is no locale data at
 * all.
 */
#define UTF8_STR		".UTF-8"
#define UTF8_STRLEN		(sizeof(UTF8_STR) - 1)
static bool
is_utf8_locale(void)
{
	const char		*msg_locale;
	static int		answer = -1;

	if (answer != -1)
		return answer;

	msg_locale = setlocale(LC_MESSAGES, NULL);
	if (msg_locale == NULL)
		return false;

	if (strstr(msg_locale, UTF8_STR) != NULL)
		answer = 1;
	else
		answer = 0;
	return answer;
}

/* Initialize the collision detector. */
static bool
unicrash_init(
	struct unicrash		**ucp,
	struct scrub_ctx	*ctx,
	bool			compare_ino,
	size_t			nr_buckets)
{
	struct unicrash		*p;

	if (!is_utf8_locale()) {
		*ucp = NULL;
		return true;
	}

	if (nr_buckets > 65536)
		nr_buckets = 65536;
	else if (nr_buckets < 16)
		nr_buckets = 16;

	p = calloc(1, UNICRASH_SZ(nr_buckets));
	if (!p)
		return false;
	p->ctx = ctx;
	p->nr_buckets = nr_buckets;
	p->compare_ino = compare_ino;
	*ucp = p;

	return true;
}

/* Initialize the collision detector for a directory. */
bool
unicrash_dir_init(
	struct unicrash		**ucp,
	struct scrub_ctx	*ctx,
	struct xfs_bstat	*bstat)
{
	/*
	 * Assume 64 bytes per dentry, clamp buckets between 16 and 64k.
	 * Same general idea as dir_hash_init in xfs_repair.
	 */
	return unicrash_init(ucp, ctx, true, bstat->bs_size / 64);
}

/* Initialize the collision detector for an extended attribute. */
bool
unicrash_xattr_init(
	struct unicrash		**ucp,
	struct scrub_ctx	*ctx,
	struct xfs_bstat	*bstat)
{
	/* Assume 16 attributes per extent for lack of a better idea. */
	return unicrash_init(ucp, ctx, false, 16 * (1 + bstat->bs_aextents));
}

/* Free the crash detector. */
void
unicrash_free(
	struct unicrash		*uc)
{
	struct name_entry	*ne;
	struct name_entry	*x;
	size_t			i;

	if (!uc)
		return;

	for (i = 0; i < uc->nr_buckets; i++) {
		for (ne = uc->buckets[i]; ne != NULL; ne = x) {
			x = ne->next;
			free(ne);
		}
	}
	free(uc);
}

/* Steal the dirhash function from libxfs, avoid linking with libxfs. */

#define rol32(x, y)		(((x) << (y)) | ((x) >> (32 - (y))))

/*
 * Implement a simple hash on a character string.
 * Rotate the hash value by 7 bits, then XOR each character in.
 * This is implemented with some source-level loop unrolling.
 */
static xfs_dahash_t
unicrash_hashname(
	const uint8_t		*name,
	size_t			namelen)
{
	xfs_dahash_t		hash;

	/*
	 * Do four characters at a time as long as we can.
	 */
	for (hash = 0; namelen >= 4; namelen -= 4, name += 4)
		hash = (name[0] << 21) ^ (name[1] << 14) ^ (name[2] << 7) ^
		       (name[3] << 0) ^ rol32(hash, 7 * 4);

	/*
	 * Now do the rest of the characters.
	 */
	switch (namelen) {
	case 3:
		return (name[0] << 14) ^ (name[1] << 7) ^ (name[2] << 0) ^
		       rol32(hash, 7 * 3);
	case 2:
		return (name[0] << 7) ^ (name[1] << 0) ^ rol32(hash, 7 * 2);
	case 1:
		return (name[0] << 0) ^ rol32(hash, 7 * 1);
	default: /* case 0: */
		return hash;
	}
}

/*
 * Normalize a name according to Unicode NFKC normalization rules.
 * Returns true if the name was already normalized.
 */
static bool
unicrash_normalize(
	const char		*in,
	uint8_t			*out,
	size_t			outlen)
{
	size_t			inlen = strlen(in);

	assert(inlen <= outlen);
	if (!u8_normalize(UNINORM_NFKC, (const uint8_t *)in, inlen,
			out, &outlen)) {
		/* Didn't normalize, just return the same buffer. */
		memcpy(out, in, inlen + 1);
		return true;
	}
	out[outlen] = 0;
	return outlen == inlen ? memcmp(in, out, inlen) == 0 : false;
}

/* Complain about Unicode problems. */
static void
unicrash_complain(
	struct unicrash		*uc,
	const char		*descr,
	const char		*what,
	bool			normal,
	bool			unique,
	const char		*name,
	uint8_t			*uniname)
{
	char			*bad1 = NULL;
	char			*bad2 = NULL;

	bad1 = string_escape(name);
	bad2 = string_escape((char *)uniname);

	if (!normal && should_warn_about_name(uc->ctx))
		str_info(uc->ctx, descr,
_("Unicode name \"%s\" in %s should be normalized as \"%s\"."),
				bad1, what, bad2);
	if (!unique)
		str_warn(uc->ctx, descr,
_("Duplicate normalized Unicode name \"%s\" found in %s."),
				bad1, what);

	free(bad1);
	free(bad2);
}

/*
 * Try to add a name -> ino entry to the collision detector.  The name
 * must be normalized according to Unicode NFKC normalization rules to
 * detect byte-unique names that map to the same sequence of Unicode
 * code points.
 *
 * This function returns true either if there was no previous mapping or
 * there was a mapping that matched exactly.  It returns false if
 * there is already a record with that name pointing to a different
 * inode.
 */
static bool
unicrash_add(
	struct unicrash		*uc,
	uint8_t			*uniname,
	xfs_ino_t		ino,
	bool			*unique)
{
	struct name_entry	*ne;
	struct name_entry	*x;
	struct name_entry	**nep;
	size_t			uninamelen = u8_strlen(uniname);
	size_t			bucket;
	xfs_dahash_t		hash;

	/* Do we already know about that name? */
	hash = unicrash_hashname(uniname, uninamelen);
	bucket = hash % uc->nr_buckets;
	for (nep = &uc->buckets[bucket], ne = *nep; ne != NULL; ne = x) {
		if (u8_strcmp(uniname, ne->uniname) == 0) {
			*unique = uc->compare_ino ? ne->ino == ino : false;
			return true;
		}
		nep = &ne->next;
		x = ne->next;
	}

	/* Remember that name. */
	x = malloc(NAME_ENTRY_SZ(uninamelen));
	if (!x)
		return false;
	x->next = NULL;
	x->ino = ino;
	x->uninamelen = uninamelen;
	memcpy(x->uniname, uniname, uninamelen + 1);
	*nep = x;
	*unique = true;

	return true;
}

/* Check a name for unicode normalization problems or collisions. */
static bool
__unicrash_check_name(
	struct unicrash		*uc,
	const char		*descr,
	const char		*namedescr,
	const char		*name,
	xfs_ino_t		ino)
{
	uint8_t			uniname[(NAME_MAX * 2) + 1];
	bool			moveon;
	bool			normal;
	bool			unique;

	memset(uniname, 0, (NAME_MAX * 2) + 1);
	normal = unicrash_normalize(name, uniname, NAME_MAX * 2);
	moveon = unicrash_add(uc, uniname, ino, &unique);
	if (!moveon)
		return false;

	if (normal && unique)
		return true;

	unicrash_complain(uc, descr, namedescr, normal, unique, name,
			uniname);
	return true;
}

/* Check a directory entry for unicode normalization problems or collisions. */
bool
unicrash_check_dir_name(
	struct unicrash		*uc,
	const char		*descr,
	struct dirent		*dentry)
{
	if (!uc)
		return true;
	return __unicrash_check_name(uc, descr, _("directory"),
			dentry->d_name, dentry->d_ino);
}

/*
 * Check an extended attribute name for unicode normalization problems
 * or collisions.
 */
bool
unicrash_check_xattr_name(
	struct unicrash		*uc,
	const char		*descr,
	const char		*attrname)
{
	if (!uc)
		return true;
	return __unicrash_check_name(uc, descr, _("extended attribute"),
			attrname, 0);
}
