#ifndef FS_FUZZ_GFS2_FUZZER_HH
#define FS_FUZZ_GFS2_FUZZER_HH

#include "fsfuzzer.hh"

typedef struct {
  uint64_t bn;
  uint64_t blen;
  uint64_t cksum_offset;
} crc_info_t;

static class gfs2_fuzzer: public fsfuzzer 
{
    public:
      gfs2_fuzzer(): fsfuzzer("gfs2") { }

      void fix_checksum();
      void fix_general_checksum();

      void compress(const char *in_path, void *buffer, const char *meta_path);

      void decompress(const void *meta_buffer, size_t meta_len, bool checksum = true);
      void general_decompress(const void *meta_buffer, size_t meta_len, bool checksum = true);

      std::vector<crc_info_t> crc_infos; 

} gfs2_fuzzer;

#endif
