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
    size_t oLen = iLen * 2;
    int err;

    in = (uint8_t *) malloc(iLen);
    out = (uint8_t *) malloc(oLen);
    memset(in, 0, iLen);
    memset(out, 0, oLen);

    uint8_t tmp[] = {0x30, 0x30, 0x31, 0x31, 0x32, 0x32, 0x33, 0x33, 0x34, 0x34, 0x35, 0x35, 0x36, 0x36, 0x37, 0x37,
                     0x38, 0x38, 0x39, 0x39, 0x40, 0x40, 0x41, 0x41, 0x42, 0x42, 0x43, 0x43, 0x44, 0x44, 0x45,
                     0x45};//"0011223344556677889900AABBCCDDEE";

    memcpy((void *) in, (const void *) tmp, iLen);

    compress::Compressor compressor(new compress::CompressorConfig(iLen, &oLen));
    err = compressor.process(in, out);

    if (!err)
        printf("Compression successful\n");
    else
        printf("Compression unsuccessful. ERROR CODE:%d\n", err);

    for (int i = 0; i < 64; i++) {
        printf("%02x:", out[i]);
    }

    free(in);
    free(out);
}