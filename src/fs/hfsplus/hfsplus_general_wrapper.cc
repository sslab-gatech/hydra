#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "hfsplus_fuzzer.hh"

extern "C" void compress(const char *input_path, void *buffer, const char *meta_path)
{
  hfsplus_fuzzer.general_compress(input_path, buffer, meta_path);
}

extern "C" void decompress(const void *mem, size_t len)
{
  hfsplus_fuzzer.general_decompress(mem, len, true);
}

extern "C" void sync_to_file(const char *path)
{
  hfsplus_fuzzer.sync_to_file(path);
}
