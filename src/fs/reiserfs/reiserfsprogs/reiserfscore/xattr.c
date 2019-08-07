#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <sys/acl.h>
#include "reiserfs_lib.h"

static inline unsigned short from32to16(unsigned int x)
{
	/* add up 16-bit and 16-bit for 16+c bit */
	x = (x & 0xffff) + (x >> 16);
	/* add up carry.. */
	x = (x & 0xffff) + (x >> 16);
	return x;
}

static unsigned int do_csum(const unsigned char *buff, int len)
{
	int odd;
	unsigned int result = 0;

	if (len <= 0)
		goto out;
	odd = 1 & (unsigned long) buff;
	if (odd) {
#ifdef __LITTLE_ENDIAN
		result += (*buff << 8);
#else
		result = *buff;
#endif
		len--;
		buff++;
	}
	if (len >= 2) {
		if (2 & (unsigned long) buff) {
			result += *(unsigned short *) buff;
			len -= 2;
			buff += 2;
		}
		if (len >= 4) {
			const unsigned char *end = buff + ((unsigned)len & ~3);
			unsigned int carry = 0;
			do {
				unsigned int w = *(unsigned int *) buff;
				buff += 4;
				result += carry;
				result += w;
				carry = (w > result);
			} while (buff < end);
			result += carry;
			result = (result & 0xffff) + (result >> 16);
		}
		if (len & 2) {
			result += *(unsigned short *) buff;
			buff += 2;
		}
	}
	if (len & 1)
#ifdef __LITTLE_ENDIAN
		result += *buff;
#else
		result += (*buff << 8);
#endif
	result = from32to16(result);
	if (odd)
		result = ((result >> 8) & 0xff) | ((result & 0xff) << 8);
out:
	return result;
}

__wsum csum_partial(const void *buff, int len, __wsum wsum)
{
	unsigned int sum = (__force unsigned int)wsum;
	unsigned int result = do_csum(buff, len);

	/* add in old sum, and carry.. */
	result += sum;
	if (sum > result)
		result += 1;
	return (__force __wsum)result;
}

inline __u32 reiserfs_xattr_hash(const char *msg, int len)
{
	return csum_partial(msg, len, 0);
}

int reiserfs_check_xattr(const void *body, int len)
{
	const struct reiserfs_xattr_header *xah = body;
	int hdrsz = sizeof(struct reiserfs_xattr_header);
	__u32 hash, disk_hash;

	if (len < hdrsz)
		return -EINVAL;

	hash = reiserfs_xattr_hash(body + hdrsz, len - hdrsz);
	disk_hash = le32_to_cpu(xah->h_hash);
	return xah->h_magic == cpu_to_le32(REISERFS_XATTR_MAGIC) &&
	      (disk_hash == hash || from32to16(disk_hash) == hash);
}

int reiserfs_acl_count(size_t size)
{
	ssize_t s;
	size -= sizeof(struct reiserfs_acl_header);
	s = size - 4 * sizeof(struct reiserfs_acl_entry_short);
	if (s < 0) {
		if (size % sizeof(struct reiserfs_acl_entry_short))
			return -1;
		return size / sizeof(struct reiserfs_acl_entry_short);
	} else {
		if (s % sizeof(struct reiserfs_acl_entry))
			return -1;
		return s / sizeof(struct reiserfs_acl_entry) + 4;
	}
}
