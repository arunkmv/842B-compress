#include <stdint-gcc.h>
#include <cstdio>
#include <cstring>
#include "Compressor.h"

using namespace std;

int main() {
    printf("\n===============================");
    printf("\n        842 Compression        ");
    printf("\n===============================\n\n");

    uint8_t *in, *out;
    size_t iLen = 32;

    in = (uint8_t *) malloc(iLen);
    out = (uint8_t *) malloc(2 * iLen);
    memset(in, 0, iLen);
    memset(out, 0, 2*iLen);

    uint8_t tmp[] = {0x30, 0x30, 0x31, 0x31, 0x32, 0x32, 0x33, 0x33, 0x34, 0x34, 0x35, 0x35, 0x36, 0x36, 0x37, 0x37,
                     0x38, 0x38, 0x39, 0x39, 0x40, 0x40, 0x41, 0x41, 0x42, 0x42, 0x43, 0x43, 0x44, 0x44, 0x45,
                     0x45};//"0011223344556677889900AABBCCDDEE";

    strncpy((char *) in, (const char *) tmp, 32);

    compress::Compressor compressor(new compress::CompressorConfig(iLen));
    compressor.process(in, out);

    for (int i = 0; i < 64; i++) {
        printf("%02x:", out[i]);
    }

    free(in);
    free(out);
}