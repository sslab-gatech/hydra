// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "xfs_fuzzer.hh"

extern "C" void compress(const char *input_path, void *buffer, const char *meta_path)
{
  xfs_fuzzer.compress(input_path, buffer, meta_path);
}

extern "C" void decompress(const void *mem, size_t len)
{
  xfs_fuzzer.decompress(mem, len, true);
}

extern "C" void sync_to_file(const char *path)
{
  xfs_fuzzer.sync_to_file(path);
}
