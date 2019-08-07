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

#include "reiserfs_fuzzer.hh"
#include "utils.hh"

void reiserfs_fuzzer::fix_checksum() 
{
    ;
}

void reiserfs_fuzzer::fix_general_checksum() 
{
    ;
}

void reiserfs_fuzzer::compress(
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

  int pid, status;

  pid = fork();
  if (!pid) {
    int devnull_fd = open("/dev/null",O_WRONLY | O_CREAT, 0666);
    dup2(devnull_fd, 1);
    dup2(devnull_fd, 2);
    char* args[5];
    args[0] = const_cast<char*>("debugreiserfs");
    args[1] = const_cast<char*>("-K");
    args[2] = const_cast<char*>(meta_path);
    args[3] = const_cast<char*>(in_path);
    args[4] = NULL;
    execvp(args[0], args);
    close(devnull_fd);
  }

  waitpid(pid, &status, 0);

  // read meta image
  int meta_image_fd = open(meta_path, O_RDONLY);
  uint64_t blknum;
  std::set<uint64_t> meta_blocks;

  read(meta_image_fd, &block_size_, sizeof(uint32_t));
  block_count_ = image_size_ / block_size_;

  while (read(meta_image_fd, &blknum, sizeof(uint64_t)) == sizeof(uint64_t)) {
    meta_blocks.insert(blknum);
    printf("%ld\n", blknum);
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

void reiserfs_fuzzer::decompress(
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

void reiserfs_fuzzer::general_decompress(
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
