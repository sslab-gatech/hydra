#ifndef FS_FUZZ_VFAT_HH
#define FS_FUZZ_VFAT_HH

#include <stdint.h>

/* msdos_fs.h */
#define SECTOR_SIZE 512		/* sector size (bytes) */
#define MSDOS_DPS (SECTOR_SIZE / sizeof(struct msdos_dir_entry))
#define MSDOS_DPS_BITS 4	/* log2(MSDOS_DPS) */
#define MSDOS_DIR_BITS 5	/* log2(sizeof(struct msdos_dir_entry)) */

#define ATTR_NONE 0	/* no attribute bits */
#define ATTR_RO 1	/* read-only */
#define ATTR_HIDDEN 2	/* hidden */
#define ATTR_SYS 4	/* system */
#define ATTR_VOLUME 8	/* volume label */
#define ATTR_DIR 16	/* directory */
#define ATTR_ARCH 32	/* archived */

/* attribute bits that are copied "as is" */
#define ATTR_UNUSED (ATTR_VOLUME | ATTR_ARCH | ATTR_SYS | ATTR_HIDDEN)

#define DELETED_FLAG 0xe5	/* marks file as deleted when in name[0] */
#define IS_FREE(n) (!*(n) || *(n) == DELETED_FLAG)

#define MSDOS_NAME 11			/* maximum name length */
#define MSDOS_DOT ".          "		/* ".", padded to MSDOS_NAME chars */
#define MSDOS_DOTDOT "..         "	/* "..", padded to MSDOS_NAME chars */

struct msdos_dir_entry {
    uint8_t name[MSDOS_NAME];	/* name including extension */
    uint8_t attr;		/* attribute bits */
    uint8_t lcase;		/* Case for base and extension */
    uint8_t ctime_cs;		/* Creation time, centiseconds (0-199) */
    uint16_t ctime;		/* Creation time */
    uint16_t cdate;		/* Creation date */
    uint16_t adate;		/* Last access date */
    uint16_t starthi;		/* High 16 bits of cluster in FAT32 */
    uint16_t time, date, start;	/* time, date and first cluster */
    uint32_t size;		/* file size (in bytes) */
} __attribute__ ((packed));

/* fat.h */
#define VFAT_LN_ATTR (ATTR_RO | ATTR_HIDDEN | ATTR_SYS | ATTR_VOLUME)

#define FAT_STATE_DIRTY 0x01

/* ++roman: Use own definition of boot sector structure -- the kernel headers'
 * name for it is msdos_boot_sector in 2.0 and fat_boot_sector in 2.1 ... */
struct boot_sector {
    uint8_t ignored[3];		/* Boot strap short or near jump */
    uint8_t system_id[8];	/* Name - can be used to special case
				   partition manager volumes */
    uint8_t sector_size[2];	/* bytes per logical sector */
    uint8_t cluster_size;	/* sectors/cluster */
    uint16_t reserved;		/* reserved sectors */
    uint8_t fats;		/* number of FATs */
    uint8_t dir_entries[2];	/* root directory entries */
    uint8_t sectors[2];		/* number of sectors */
    uint8_t media;		/* media code (unused) */
    uint16_t fat_length;	/* sectors/FAT */
    uint16_t secs_track;	/* sectors per track */
    uint16_t heads;		/* number of heads */
    uint32_t hidden;		/* hidden sectors (unused) */
    uint32_t total_sect;	/* number of sectors (if sectors == 0) */

    /* The following fields are only used by FAT32 */
    uint32_t fat32_length;	/* sectors/FAT */
    uint16_t flags;		/* bit 8: fat mirroring, low 4: active fat */
    uint8_t version[2];		/* major, minor filesystem version */
    uint32_t root_cluster;	/* first cluster in root directory */
    uint16_t info_sector;	/* filesystem info sector */
    uint16_t backup_boot;	/* backup boot sector */
    uint8_t reserved2[12];	/* Unused */

    uint8_t drive_number;	/* Logical Drive Number */
    uint8_t reserved3;		/* Unused */

    uint8_t extended_sig;	/* Extended Signature (0x29) */
    uint32_t serial;		/* Serial number */
    uint8_t label[11];		/* FS label */
    uint8_t fs_type[8];		/* FS Type */

    /* fill up to 512 bytes */
    uint8_t junk[422];
} __attribute__ ((packed));

struct boot_sector_16 {
    uint8_t ignored[3];		/* Boot strap short or near jump */
    uint8_t system_id[8];	/* Name - can be used to special case
				   partition manager volumes */
    uint8_t sector_size[2];	/* bytes per logical sector */
    uint8_t cluster_size;	/* sectors/cluster */
    uint16_t reserved;		/* reserved sectors */
    uint8_t fats;		/* number of FATs */
    uint8_t dir_entries[2];	/* root directory entries */
    uint8_t sectors[2];		/* number of sectors */
    uint8_t media;		/* media code (unused) */
    uint16_t fat_length;	/* sectors/FAT */
    uint16_t secs_track;	/* sectors per track */
    uint16_t heads;		/* number of heads */
    uint32_t hidden;		/* hidden sectors (unused) */
    uint32_t total_sect;	/* number of sectors (if sectors == 0) */

    uint8_t drive_number;	/* Logical Drive Number */
    uint8_t reserved2;		/* Unused */

    uint8_t extended_sig;	/* Extended Signature (0x29) */
    uint32_t serial;		/* Serial number */
    uint8_t label[11];		/* FS label */
    uint8_t fs_type[8];		/* FS Type */

    /* fill up to 512 bytes */
    uint8_t junk[450];
} __attribute__ ((packed));

struct info_sector {
    uint32_t magic;		/* Magic for info sector ('RRaA') */
    uint8_t reserved1[480];
    uint32_t signature;		/* 0x61417272 ('rrAa') */
    uint32_t free_clusters;	/* Free cluster count.  -1 if unknown */
    uint32_t next_cluster;	/* Most recently allocated cluster. */
    uint8_t reserved2[12];
    uint32_t boot_sign;
};

typedef struct {
    uint8_t name[MSDOS_NAME];	/* name including extension */
    uint8_t attr;		/* attribute bits */
    uint8_t lcase;		/* Case for base and extension */
    uint8_t ctime_ms;		/* Creation time, milliseconds */
    uint16_t ctime;		/* Creation time */
    uint16_t cdate;		/* Creation date */
    uint16_t adate;		/* Last access date */
    uint16_t starthi;		/* High 16 bits of cluster in FAT32 */
    uint16_t time, date, start;	/* time, date and first cluster */
    uint32_t size;		/* file size (in bytes) */
} __attribute__ ((packed)) DIR_ENT;

typedef struct _dos_file {
    DIR_ENT dir_ent;
    char *lfn;
    off_t offset;
    off_t lfn_offset;
    struct _dos_file *parent;	/* parent directory */
    struct _dos_file *next;	/* next entry */
    struct _dos_file *first;	/* first entry (directory only) */
} DOS_FILE;

typedef struct {
    uint32_t value;
    uint32_t reserved;
} FAT_ENTRY;

typedef struct {
    int nfats;
    off_t fat_start;
    off_t fat_size;		/* unit is bytes */
    unsigned int fat_bits;	/* size of a FAT entry */
    unsigned int eff_fat_bits;	/* # of used bits in a FAT entry */
    uint32_t root_cluster;	/* 0 for old-style root dir */
    off_t root_start;
    unsigned int root_entries;
    off_t data_start;
    unsigned int cluster_size;
    uint32_t data_clusters;	/* not including two reserved cluster numbers */
    off_t fsinfo_start;		/* 0 if not present */
    long free_clusters;
    off_t backupboot_start;	/* 0 if not present */
    unsigned char *fat;
    DOS_FILE **cluster_owner;
    uint32_t serial;
    char label[11];
} DOS_FS;

/* value to use as end-of-file marker */
#define FAT_EOF(fs)	((atari_format ? 0xfff : 0xff8) | FAT_EXTD(fs))
#define FAT_IS_EOF(fs,v) ((uint32_t)(v) >= (0xff8|FAT_EXTD(fs)))
/* value to mark bad clusters */
#define FAT_BAD(fs)	(0xff7 | FAT_EXTD(fs))
/* range of values used for bad clusters */
#define FAT_MIN_BAD(fs)	((atari_format ? 0xff0 : 0xff7) | FAT_EXTD(fs))
#define FAT_MAX_BAD(fs)	((atari_format ? 0xff7 : 0xff7) | FAT_EXTD(fs))
#define FAT_IS_BAD(fs,v) ((v) >= FAT_MIN_BAD(fs) && (v) <= FAT_MAX_BAD(fs))

/* return -16 as a number with fs->fat_bits bits */
#define FAT_EXTD(fs)	(((1 << fs->eff_fat_bits)-1) & ~0xf)

/* marker for files with no 8.3 name */
#define FAT_NO_83NAME 32

#endif
