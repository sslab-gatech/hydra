/*
 * Copyright (c) 2006 Silicon Graphics, Inc.
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
#ifndef SWAP_H
#define SWAP_H

#include <xfs/xfs.h>
#include <swab.h>

#if __BYTE_ORDER == __BIG_ENDIAN
#define XFS_NATIVE_HOST 1
#else
#undef XFS_NATIVE_HOST
#endif

#define ARCH_NOCONVERT	1
#ifdef XFS_NATIVE_HOST
#define ARCH_CONVERT	ARCH_NOCONVERT
#else
#define ARCH_CONVERT	0
#endif

#ifndef HAVE_SWABMACROS
#define INT_SWAP16(type,var) ((typeof(type))(__swab16((__u16)(var))))
#define INT_SWAP32(type,var) ((typeof(type))(__swab32((__u32)(var))))
#define INT_SWAP64(type,var) ((typeof(type))(__swab64((__u64)(var))))
#endif

#define INT_SWAP(type, var) \
	((sizeof(type) == 8) ? INT_SWAP64(type,var) : \
	((sizeof(type) == 4) ? INT_SWAP32(type,var) : \
	((sizeof(type) == 2) ? INT_SWAP16(type,var) : \
	(var))))

#define INT_GET(ref,arch) \
	(((arch) == ARCH_NOCONVERT) ? (ref) : INT_SWAP((ref),(ref)))

#define INT_SET(ref,arch,valueref) \
	(__builtin_constant_p(valueref) ? \
	(void)( (ref) = ( ((arch) != ARCH_NOCONVERT) ? \
	  		   (INT_SWAP((ref),(valueref))) : (valueref)) ) : \
	(void)( ((ref) = (valueref)), \
			( ((arch) != ARCH_NOCONVERT) ? \
			   (ref) = INT_SWAP((ref),(ref)) : 0 ) ))

#define INT_XLATE(buf,p,dir,arch) \
	((dir > 0) ? ((p) = INT_GET((buf),(arch))) : INT_SET((buf),(arch),(p)))

#endif /* SWAP_H */
