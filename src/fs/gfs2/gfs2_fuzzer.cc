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

#include "gfs2_fuzzer.hh"
#include "utils.hh"

// #define LOG_FUZZ

#define swap64(val) \
   ( (((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | \
        (((val) >> 24) & 0x0000000000FF0000) | (((val) >>  8) & 0x00000000FF000000) | \
        (((val) <<  8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | \
        (((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000) )

#define swap32(val) \
	( (((val) >> 24) & 0x000000FF) | (((val) >>  8) & 0x0000FF00) | \
   (((val) <<  8) & 0x00FF0000) | (((val) << 24) & 0xFF000000) )

void gfs2_fuzzer::fix_checksum() 
{
  return;

  uint32_t crc;
  uint32_t original_crc;

  for (crc_info_t &crc_info : crc_infos) {
		// printf("bn: 0x%lx blen: 0x%lx offset: 0x%lx\n", crc_info.bn << BBSHIFT, crc_info.blen << BBSHIFT, crc_info.cksum_offset);
		// memcpy(&original_crc, (char*)image_buffer_ + (crc_info.bn << BBSHIFT) + crc_info.cksum_offset, sizeof(uint32_t));
		// original_crc = swap32(original_crc);
		// crc = xfs_start_cksum_safe((char*)image_buffer_ + (crc_info.bn << BBSHIFT), 
		//													crc_info.blen << BBSHIFT, crc_info.cksum_offset);
		// printf("original_crc: 0x%x crc: 0x%x\n", original_crc, ~crc);
   //  *(uint32_t *)((char*)image_buffer_ + (crc_info.bn << BBSHIFT) + crc_info.cksum_offset) = ~crc;

  }
}

void gfs2_fuzzer::fix_general_checksum()
{
    ;
}

struct meta_info_t {
    uint64_t blk_nr;
    uint64_t type;
};

void gfs2_fuzzer::compress(
    const char *in_path,
    void *buffer,
    const char *meta_path) 
{
  bool generate_meta_image = meta_path != NULL;
  
  struct stat st;
  stat(in_path, &st);

  image_size_ = st.st_size;

  int in_image_fd = open(in_path, O_RDONLY);
  if (in_image_fd < 0)
    FATAL("[-] image %s compression failed.", in_path);

  image_buffer_ = buffer;
  if (read(in_image_fd, image_buffer_, image_size_) != image_size_)
    FATAL("[-] image %s compression failed.", in_path);

  close(in_image_fd);

  char *in_full_path = realpath(in_path, NULL);
  int status, pid;

  pid = fork();
  if (!pid) {
    char* args[7];
    args[0] = const_cast<char*>("gfs2_edit");
    args[1] = const_cast<char*>("saveforfuzz");
    args[2] = const_cast<char*>("-z");
    args[3] = const_cast<char*>("0");
    args[4] = const_cast<char*>(in_full_path);
    args[5] = const_cast<char*>(meta_path);
    args[6] = NULL;
    execvp(args[0], args);
  }

  waitpid(pid, &status, 0);
  
  // read meta image
  int meta_image_fd = open(meta_path, O_RDONLY);
  
  uint64_t blknum;
  std::set<uint64_t> meta_blocks;
  uint64_t meta_block_cnt = 0;
  struct meta_info_t meta_info;

  read(meta_image_fd, &block_size_, sizeof(uint32_t));
  // printf("blk size: 0x%x\n", block_size_);
  block_count_ = image_size_ / block_size_;

  while (read(meta_image_fd, &meta_info, sizeof(struct meta_info_t)) == sizeof(struct meta_info_t)) {
    // printf("blk nr: 0x%lx\n", meta_info.blk_nr);
    meta_blocks.insert(meta_info.blk_nr);
    meta_block_cnt++;
  }
  close(meta_image_fd);

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

void gfs2_fuzzer::decompress(
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

void gfs2_fuzzer::general_decompress(
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
