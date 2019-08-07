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

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

/*
 * our custom mmap b/c Linux doesn't have MAP_AUTOGROW
 * and for an empty file we need to write to last byte
 * to ensure it can be accessed
 */

void *
mmap_autogrow(size_t len, int fd, off_t offset)
{
    struct stat buf;
    char nul_buffer[] = "";

    /* prealloc file if it is an empty file */
    if (fstat(fd, &buf) == -1) {;
	return (void*)MAP_FAILED;
    }
    if (buf.st_size < offset+len) {
	(void)lseek(fd, offset+len-1, SEEK_SET);
	(void)write(fd, nul_buffer, 1);
    }

    return mmap( 0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset );
}
