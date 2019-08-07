#ifndef FS_FUZZ_XFS_FUZZER_HH
#define FS_FUZZ_XFS_FUZZER_HH

#include "fsfuzzer.hh"

static class reiserfs_fuzzer: public fsfuzzer 
{
    public:
      reiserfs_fuzzer(): fsfuzzer("reiserfs") { }

      void fix_checksum();
      void fix_general_checksum();

      void compress(const char *in_path, void *buffer, const char *meta_path);

      void decompress(const void *meta_buffer, size_t meta_len, bool checksum = true);
      void general_decompress(const void *meta_buffer, size_t meta_len, bool checksum = true);

} reiserfs_fuzzer;

#endif
