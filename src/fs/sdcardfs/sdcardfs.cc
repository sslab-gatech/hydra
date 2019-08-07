#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <set>

#include "sdcardfs.hh"

static int fd;

/* Unaligned fields must first be accessed byte-wise */
#define GET_UNALIGNED_W(f)          \
        ( (uint16_t)f[0] | ((uint16_t)f[1]<<8) )

#define ROUND_TO_MULTIPLE(n,m) ((n) && (m) ? (n)+(m)-1-((n)-1)%(m) : 0)

/* cut-over cluster counts for FAT12 and FAT16 */
#define FAT12_THRESHOLD  4085
#define FAT16_THRESHOLD 65525

void fs_read(off_t pos, int size, void *data) {
    int got;
    if (lseek(fd, pos, 0) != pos) {
        fprintf(stderr, "fail to seek to %lld", (long long)pos);
        _exit(1);
    }
    if ((got = read(fd, data, size)) < 0) {
        fprintf(stderr, "Read %d bytes at %lld", size, (long long)pos);
        _exit(1);
    }
    if (got != size) {
        fprintf(stderr, "Got %d bytes instead of %d at %lld", got, size, (long long)pos);
        _exit(1);
    }
}

void read_boot(DOS_FS *fs, std::set<uint64_t> &meta_blocks) {
    struct boot_sector b;
    unsigned total_sectors;
    unsigned int logical_sector_size, sectors;
    off_t fat_length;
    unsigned total_fat_entries;
    off_t data_size;

    fs_read(0, sizeof(b), &b);
    meta_blocks.insert(0); // add boot sector

    logical_sector_size = GET_UNALIGNED_W(b.sector_size);     
    printf("logical sector size: 0x%x\n", logical_sector_size);

    fs->cluster_size = b.cluster_size * logical_sector_size;
    printf("cluster size: 0x%x\n", fs->cluster_size);

    fs->nfats = b.fats;
    printf("nfats: %d\n", fs->nfats);

    sectors = GET_UNALIGNED_W(b.sectors);
    total_sectors = sectors ? sectors : b.total_sect;
    printf("#sector: 0x%x\n", total_sectors);

    fat_length = b.fat_length ? b.fat_length : b.fat32_length;
    printf("fat length: 0x%lx\n", fat_length);

    fs->fat_start = (off_t)b.reserved * logical_sector_size;
    printf("fat start: 0x%lx\n", fs->fat_start);
    uint64_t fat_start_nr = fs->fat_start / logical_sector_size;
    for (uint64_t i = 0; i < fs->nfats * fat_length; i++)
        meta_blocks.insert(fat_start_nr + i);
    
    fs->root_start = ((off_t)b.reserved + fs->nfats * fat_length) * logical_sector_size;
    printf("root start: 0x%lx\n", fs->root_start);
    uint64_t root_start_nr = fs->root_start / logical_sector_size;
    fs->root_entries = GET_UNALIGNED_W(b.dir_entries);
    uint64_t root_size = ROUND_TO_MULTIPLE(fs->root_entries << MSDOS_DIR_BITS,
                                    logical_sector_size);
    for (uint64_t i = 0; i < root_size / logical_sector_size; i++)
        meta_blocks.insert(root_start_nr + i);
     
    fs->data_start = fs->root_start + root_size;
    printf("data start: 0x%lx\n", fs->data_start);

    data_size = (off_t)total_sectors * logical_sector_size - fs->data_start;
    if (data_size < fs->cluster_size) {
        fprintf(stderr, "Filesystem has no space for any data clusters");
        _exit(1);
    }

	fs->data_clusters = data_size / fs->cluster_size;
    fs->root_cluster = 0;   /* indicates standard, pre-FAT32 root dir */
    fs->fsinfo_start = 0;   /* no FSINFO structure */
    fs->free_clusters = -1; /* unknown */

    // vFAT is kinda FAT16 in fact
    fs->fat_bits = (fs->data_clusters >= FAT12_THRESHOLD) ? 16 : 12;
    if (fs->data_clusters >= FAT16_THRESHOLD) {
        fprintf(stderr, "Too many clusters (%lu) for FAT16 filesystem.",
                                        (unsigned long)fs->data_clusters);
        _exit(1);
    }
    printf("fat bits: %d\n", fs->fat_bits);

    fs->eff_fat_bits = (fs->fat_bits == 32) ? 28 : fs->fat_bits;
    fs->fat_size = fat_length * logical_sector_size;

	fs->label[0] = 0;
    if (fs->fat_bits == 12 || fs->fat_bits == 16) {
		struct boot_sector_16 *b16 = (struct boot_sector_16 *)&b;
		if (b16->extended_sig == 0x29) {
	    	memmove(fs->label, b16->label, 11);
	    	fs->serial = b16->serial;
		}
    } else if (fs->fat_bits == 32) {
		if (b.extended_sig == 0x29) {
	    	memmove(fs->label, &b.label, 11);
	    	fs->serial = b.serial;
		}
    }
	printf("serial: %x label: %s\n", fs->serial, fs->label);

	uint64_t data_start_nr = fs->data_start / logical_sector_size;
	meta_blocks.insert(data_start_nr);
	for (uint64_t i = 1; i < data_size / logical_sector_size; i++) {
		uint64_t buf;
		fs_read((data_start_nr + i) * logical_sector_size, sizeof(uint64_t), &buf);
		if (buf != 0)
			meta_blocks.insert(data_start_nr + i);
	}
}

void find_meta(std::set<uint64_t> &meta_blocks, const char *src) {
	
    DOS_FS fs;

    fd = open(src, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "fail to open the image.\n");
        _exit(1);
    }

    memset(&fs, 0, sizeof(fs));
    meta_blocks.clear();
    read_boot(&fs, meta_blocks);

    /*
	for (auto blknr : meta_blocks) {
		printf("%ld\n", blknr);
	}
    */
     
    close(fd);
}

