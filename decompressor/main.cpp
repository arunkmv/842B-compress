#include <cstdio>
#include <cstring>
#include "Decompressor.h"

using namespace std;

int main() {
    printf("\n===============================");
    printf("\n        842 Decompression        ");
    printf("\n===============================\n\n");

    uint8_t *in, *out;
    size_t iLen = 40;
    size_t oLen = 32;
    int err;

    in = (uint8_t *) malloc(iLen);
    out = (uint8_t *) malloc(oLen);
    memset(in, 0, iLen);
    memset(out, 0, oLen);

    uint8_t tmp[] = {0x01, 0x81, 0x81, 0x89, 0x89, 0x91, 0x91, 0x99, 0x98,
                     0x0d, 0x0d, 0x0d, 0x4d, 0x4d, 0x8d, 0x8d, 0xcd, 0xc0,
                     0x70, 0x70, 0x72, 0x72, 0x80, 0x80, 0x82, 0x82, 0x04,
                     0x24, 0x24, 0x34, 0x34, 0x44, 0x44, 0x54, 0x5f, 0x48,
                     0x4e, 0x4f, 0x91, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00} ;//"0011223344556677889900AABBCCDDEE";

    strncpy((char *) in, (const char *) tmp, iLen);

    compress::Decompressor decompressor(new compress::CompressorConfig(iLen, &oLen));
    err = decompressor.process(in, out);

    if (!err)
        printf("Decompression successful\n");
    else
        printf("Decompression unsuccessful. ERROR CODE:%d\n", err);

    for (int i = 0; i < 32; i++) {
        printf("%02x:", out[i]);
    }

    free(in);
    free(out);
}
