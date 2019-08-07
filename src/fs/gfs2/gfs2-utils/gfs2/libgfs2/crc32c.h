#ifndef CRC32C_H
#define CRC32C_H
#include <stdlib.h>
#include <inttypes.h>

uint32_t crc32c(uint32_t seed, unsigned char const *data, size_t length);
void crc32c_optimization_init(void);

#endif
