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

#include "sdcardfs_fuzzer.hh"
#include "utils.hh"

void vfat_fuzzer::fix_checksum() 
{
    ;
}

void vfat_fuzzer::fix_general_checksum() 
{
    ;
}

extern void find_meta(std::set<uint64_t> &meta_blocks, const char *src);
void vfat_fuzzer::compress(
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

  std::set<uint64_t> meta_blocks;
  find_meta(meta_blocks, in_path);
  
  int meta_image_fd = -1;
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

void vfat_fuzzer::decompress(
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

void vfat_fuzzer::general_decompress(
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
