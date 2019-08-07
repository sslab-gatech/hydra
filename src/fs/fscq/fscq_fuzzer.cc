#include <sys/stat.h>
#include <set>
#include <map>

#include "fscq_fuzzer.hh"
#include "fscq.hh"
#include "utils.hh"
extern "C" {
  #include "crc32c.h"
}

void fscq_fuzzer::fix_checksum() {
}

void fscq_fuzzer::fix_general_checksum() {
}

void fscq_fuzzer::compress(
        const char *in_path, void *buffer, const char *meta_path) {
}


void fscq_fuzzer::decompress(
        const char *in_path, size_t meta_len, bool checksum) {
}

void fscq_fuzzer::general_decompress(
    const void *meta_buffer, size_t meta_len, bool checksum) {

