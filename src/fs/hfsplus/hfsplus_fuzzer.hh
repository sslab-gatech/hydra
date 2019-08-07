#ifndef FS_FUZZ_HFSPLUS_FUZZER_HH
#define FS_FUZZ_HFSPLUS_FUZZER_HH

#include "fsfuzzer.hh"

static class hfsplus_fuzzer: public fsfuzzer 
{
    public:
      hfsplus_fuzzer(): fsfuzzer("hfsplus") { }

      void fix_checksum();
      void fix_general_checksum();

      void compress(const char *in_path, void *buffer, const char *meta_path);

      void decompress(const void *meta_buffer, size_t meta_len, bool checksum = true);
      void general_decompress(const void *meta_buffer, size_t meta_len, bool checksum = true);
      

} hfsplus_fuzzer;

#endif
