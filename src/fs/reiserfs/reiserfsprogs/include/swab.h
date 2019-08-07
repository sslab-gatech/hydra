/*
 *  Copyright 2002-2004 by Hans Reiser, licensing governed by
 *  reiserfsprogs/README
 */

#ifndef REISERFSPROGS_SWAB_H
#define REISERFSPROGS_SWAB_H

#include <endian.h>
#include <linux/types.h>

#define __constant_swab16(x) ((__u16)(				\
	(((__u16)(x) & (__u16)0x00ffU) << 8) |			\
	(((__u16)(x) & (__u16)0xff00U) >> 8) ))

#define __swab16(x)						\
({								\
	__u16 __x = (x);					\
	__constant_swab16(__x);					\
})

#define __constant_swab32(x) ((__u32)(				\
	(((__u32)(x) & (__u32)0x000000ffUL) << 24) |		\
	(((__u32)(x) & (__u32)0x0000ff00UL) <<  8) |		\
	(((__u32)(x) & (__u32)0x00ff0000UL) >>  8) |		\
	(((__u32)(x) & (__u32)0xff000000UL) >> 24)))

#define __swab32(x)						\
({								\
	__u32 __x = (x);					\
	__constant_swab32(__x);					\
})

#define __constant_swab64(x) ((__u64)(				\
	(((__u64)(x) & (__u64)0x00000000000000ffULL) << 56) |	\
	(((__u64)(x) & (__u64)0x000000000000ff00ULL) << 40) |	\
	(((__u64)(x) & (__u64)0x0000000000ff0000ULL) << 24) |	\
	(((__u64)(x) & (__u64)0x00000000ff000000ULL) <<  8) |	\
	(((__u64)(x) & (__u64)0x000000ff00000000ULL) >>  8) |	\
	(((__u64)(x) & (__u64)0x0000ff0000000000ULL) >> 24) |	\
	(((__u64)(x) & (__u64)0x00ff000000000000ULL) >> 40) |	\
	(((__u64)(x) & (__u64)0xff00000000000000ULL) >> 56)))


#define __swab64(x)						\
({								\
	__u64 __x = (x);					\
	__constant_swab64(__x);					\
})

#ifndef le32_to_cpu
#ifdef __CHECKER__
#define __force		__attribute__((force))
#else
#define __force
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define cpu_to_le64(x) ((__force __le64)(__u64)(x))
#define le64_to_cpu(x) ((__force __u64)(__le64)(x))
#define cpu_to_le32(x) ((__force __le32)(__u32)(x))
#define le32_to_cpu(x) ((__force __u32)(__le32)(x))
#define cpu_to_le16(x) ((__force __le16)(__u16)(x))
#define le16_to_cpu(x) ((__force __u16)(__le16)(x))

#define constant_cpu_to_le64(x) ((__force __le64)(__u64)(x))
#define constant_le64_to_cpu(x) ((__force __u64)(__le64)(x))
#define constant_cpu_to_le32(x) ((__force __le32)(__u32)(x))
#define constant_le32_to_cpu(x) ((__force __u32)(__le32)(x))
#define constant_cpu_to_le16(x) ((__force __le16)(__u16)(x))
#define constant_le16_to_cpu(x) ((__force __u16)(__le16)(x))

#elif __BYTE_ORDER == __BIG_ENDIAN
#define cpu_to_le64(x) ((__force __le64)__swab64((x)))
#define le64_to_cpu(x) __swab64((__force __u64)(__le64)(x))
#define cpu_to_le32(x) ((__force __le32)__swab32((x)))
#define le32_to_cpu(x) __swab32((__force __u32)(__le32)(x))
#define cpu_to_le16(x) ((__force __le16)__swab16((x)))
#define le16_to_cpu(x) __swab16((__force __u16)(__le16)(x))

#define constant_cpu_to_le64(x) ((__force __le64)__constant_swab64((x)))
#define constant_le64_to_cpu(x) __constant_swab64((__force __u64)(__le64)(x))
#define constant_cpu_to_le32(x) ((__force __le32)__constant_swab32((x)))
#define constant_le32_to_cpu(x) __constant_swab32((__force __u32)(__le32)(x))
#define constant_cpu_to_le16(x) ((__force __le16)__constant_swab16((x)))
#define constant_le16_to_cpu(x) __constant_swab16((__force __u16)(__le16)(x))

#else
# error "nuxi/pdp-endian archs are not supported"
#endif
#endif

#endif /* REISERFS_SWAB_H */
