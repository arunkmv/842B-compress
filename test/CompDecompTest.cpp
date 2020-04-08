#include "../compressor/Compressor.h"
#include "../decompressor//Decompressor.h"

using namespace std;

int main() {
    printf("\n===============================");
    printf("\n     842 Comp - Decomp Test    ");
    printf("\n===============================\n\n");

    uint8_t *in, *out, *decompressed;
    size_t iLen = 32;
    size_t oLen = iLen * 2, dLen = iLen;

    int err;

    in = (uint8_t *) malloc(iLen);
    out = (uint8_t *) malloc(oLen);
    decompressed = (uint8_t *) malloc(iLen);
    memset(in, 0, iLen);
    memset(out, 0, oLen);
    memset(decompressed, 0, iLen);

    uint8_t tmp[] =
            {0x30, 0x30, 0x31, 0x31, 0x32, 0x32, 0x33, 0x33,
             0x34, 0x34, 0x35, 0x35, 0x36, 0x36, 0x37, 0x37,
             0x38, 0x38, 0x39, 0x39, 0x40, 0x40, 0x41, 0x41,
             0x42, 0x42, 0x43, 0x43, 0x44, 0x44, 0x45, 0x45};//"0011223344556677889900AABBCCDDEE";

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

    printf("\n\n");

    compress::Decompressor decompressor(new compress::CompressorConfig(oLen, &dLen));
    err = decompressor.process(out, decompressed);

    if (!err)
        printf("Decompression successful\n");
    else
        printf("Decompression unsuccessful. ERROR CODE:%d\n", err);

    for (int i = 0; i < iLen; i++) {
        printf("%02x:", decompressed[i]);
    }

    printf("\n\n");

    if (memcmp(in, decompressed, iLen) == 0) {
        printf("PASS: Original input and Decompressed data match!\n");
    } else {
        fprintf(stderr, "FAIL: Decompressed data differs from the original input data.\n");
        return -1;
    }

    free(in);
    free(out);
}