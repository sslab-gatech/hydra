// SPDX-License-Identifier: MIT

#include <iostream>
#include <set>
#include <string>

#include <stdio.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

#include "xfs_fuzzer.hh"
#include "utils.hh"
extern "C" {
  #include "crc32c.h"
}

// #define LOG_FUZZ

#define swap64(val) \
   ( (((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | \
        (((val) >> 24) & 0x0000000000FF0000) | (((val) >>  8) & 0x00000000FF000000) | \
        (((val) <<  8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | \
        (((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000) )

#define swap32(val) \
	( (((val) >> 24) & 0x000000FF) | (((val) >>  8) & 0x0000FF00) | \
   (((val) <<  8) & 0x00FF0000) | (((val) << 24) & 0xFF000000) )


#define XFS_CRC_SEED  (~(uint32_t)0)
#define BBSHIFT 			9

extern "C" uint32_t crc32c_le(uint32_t crc, unsigned char const *p, size_t len);
#define crc32c(c,p,l)   crc32c_le((c),(unsigned char const *)(p),(l))

static inline uint32_t
xfs_start_cksum_safe(char *buffer, size_t length, unsigned long cksum_offset)
{
	uint32_t zero = 0;
	uint32_t crc;

	/* Calculate CRC up to the checksum. */
	crc = crc32c(XFS_CRC_SEED, buffer, cksum_offset);

	/* Skip checksum field */
	crc = crc32c(crc, &zero, sizeof(uint32_t));

	/* Calculate the rest of the CRC. */
	return crc32c(crc, &buffer[cksum_offset + sizeof(uint32_t)],
		      length - (cksum_offset + sizeof(uint32_t)));
}

void xfs_fuzzer::fix_checksum() 
{
  uint32_t crc;
	uint32_t original_crc;

  for (crc_info_t &crc_info : crc_infos) {
		// printf("bn: 0x%lx blen: 0x%lx offset: 0x%lx\n", crc_info.bn << BBSHIFT, crc_info.blen << BBSHIFT, crc_info.cksum_offset);
		// memcpy(&original_crc, (char*)image_buffer_ + (crc_info.bn << BBSHIFT) + crc_info.cksum_offset, sizeof(uint32_t));
		// original_crc = swap32(original_crc);
		crc = xfs_start_cksum_safe((char*)image_buffer_ + (crc_info.bn << BBSHIFT), 
															crc_info.blen << BBSHIFT, crc_info.cksum_offset);
		// printf("original_crc: 0x%x crc: 0x%x\n", original_crc, ~crc);
    *(uint32_t *)((char*)image_buffer_ + (crc_info.bn << BBSHIFT) + crc_info.cksum_offset) = ~crc;

  }
}

void xfs_fuzzer::fix_general_checksum()
{
    ;
}

void xfs_fuzzer::compress(
    const char *in_path,
    void *buffer,
    const char *meta_path) 
{
  bool generate_meta_image = meta_path != NULL;
  
  struct stat st;
  stat(in_path, &st);

  image_size_ = st.st_size;
  block_size_ = (1 << 9); // hardcoded here
  block_count_ = image_size_ / block_size_;

  int in_image_fd = open(in_path, O_RDONLY);
  if (in_image_fd < 0)
    FATAL("[-] image %s compression failed.", in_path);

  image_buffer_ = buffer;
  if (read(in_image_fd, image_buffer_, image_size_) != image_size_)
    FATAL("[-] image %s compression failed.", in_path);

  close(in_image_fd);

  int pid, status;
  char crc_info_path[32];
  sprintf(crc_info_path, "/tmp/crc_XXXXXX");
  mkstemp(crc_info_path);

  pid = fork();
  if (!pid) {
    char* args[7];
    args[0] = const_cast<char*>("xfs_fuzz_metadump");
    args[1] = const_cast<char*>("-a");
    args[2] = const_cast<char*>("-o");
    args[3] = const_cast<char*>(in_path);
    args[4] = const_cast<char*>(meta_path);
    args[5] = const_cast<char*>(crc_info_path);
    args[6] = NULL;
    if (execvp(args[0], args) < 0) {
      FATAL("[-] xfs_fuzz_metadump -a -o %s %s %s failed.", in_path, meta_path, crc_info_path);
    }
  }

  waitpid(pid, &status, 0);

  
  // read meta image
  int meta_image_fd = open(meta_path, O_RDONLY);
  if (meta_image_fd < 0)
    FATAL("[-] could not open meta image %s.", meta_path);

  uint64_t blknum;
  std::set<uint64_t> meta_blocks;
  uint64_t meta_block_cnt = 0;
#ifdef LOG_FUZZ
  while (read(meta_image_fd, &blknum, sizeof(uint64_t)) == sizeof(uint64_t) && meta_block_cnt < 0x20) {
#else
  while (read(meta_image_fd, &blknum, sizeof(uint64_t)) == sizeof(uint64_t)) {
#endif
    meta_blocks.insert(swap64(blknum));
    meta_block_cnt++;
  }
  close(meta_image_fd);
  if (meta_block_cnt == 0)
    FATAL("[-] no meta_block is found.");

  // read crc info
  int crc_info_fd = open(crc_info_path, O_RDONLY);
  if (crc_info_fd < 0)
    FATAL("[-] could not open crc info file %s.", meta_path);

  crc_info_t crc_info;
  while (read(crc_info_fd, &crc_info, sizeof(uint64_t) * 3) == sizeof(uint64_t) * 3) {
    crc_infos.push_back(crc_info);
    // printf("bn: 0x%lx blen: 0x%lx offset: 0x%lx\n", crc_info.bn, crc_info.blen, crc_info.cksum_offset);
  }
  close(crc_info_fd);

  meta_image_fd = -1;
  if (generate_meta_image) {
    meta_image_fd = open(meta_path, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (meta_image_fd < 0)
      FATAL("[-] image %s compression failed.", in_path);
  }

  if (!release_metadata(meta_blocks, meta_image_fd, true)) 
    FATAL("[-] image %s compression failed.", in_path);

  if (generate_meta_image)
    close(meta_image_fd);

  print_metadata();
}

void xfs_fuzzer::decompress(
    const void *meta_buffer,
    size_t meta_len,
    bool checksum) {

  size_t meta_offset = 0;

  for (extent_t &extent : metadata_) {
    memcpy((char *)image_buffer_ + extent.first,
        (char *)meta_buffer + meta_offset, extent.second);
    meta_offset += extent.second;
  }
  
  if (checksum)
    fix_checksum();
}

void xfs_fuzzer::general_decompress(
    const void *meta_buffer,
    size_t meta_len,
    bool checksum) {

  size_t meta_offset = 0;

  for (extent_t &extent : metadata_) {
    memcpy((char *)image_buffer_ + extent.first,
        (char *)meta_buffer + meta_offset, extent.second);
    meta_offset += extent.second;
  }
  
  if (checksum)
    fix_general_checksum();
}
