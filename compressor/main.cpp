#include <stdint-gcc.h>
#include <cstdio>
#include "Compressor.h"

using namespace std;

int main() {
    printf("\n===============================");
    printf("\n        842 Compression        ");
    printf("\n===============================\n\n");

    uint64_t input[8] = {
            0x0000000000000000, 0x0000000000000000, 0x0000000000000001, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
    };
    uint64_t output[8] = {
            0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
    };

    uint64_t a = 0x12345678;
    uint8_t *b = (uint8_t *)output;
    *(u_int64_t *)b = a;

//    compress::Compressor compressor(new compress::CompressorConfig);
//    compressor.process(reinterpret_cast<uint8_t *>(input), reinterpret_cast<uint8_t *>(output));

    for (unsigned long long i : output) {
        printf("%llx", i);
        printf("\n");
    }
    return 0;
}