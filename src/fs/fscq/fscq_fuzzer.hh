#ifndef FS_FUZZ_FSCQ_FUZZER_HH
#define FS_FUZZ_FACQ_FUZZER_HH

#include <map>

#include "fsfuzzer.hh"
#include "fscq.hh"

static class fscq_fuzzer: public fsfuzzer {
    public:
        fscq_fuzzer(): fsfuzzer("fscq") {}

        void fix_checksum();

        void fix_general_checksum();

        void compress(const char *in_path, void *buffer,
                const char *meta_path = NULL);

        void decompress(const void *meta_buffer, size_t meta_len,
                bool checksum = true);

        void general_decompress(const void *meta_buffer, size_t meta_len,
                bool checksum = true);

    protected:
        std::map<address_range, uint64_t> address_map;

    private:
        std::set<uint64_t> metadata_blocks;

} fscq_fuzzer;

#endif
