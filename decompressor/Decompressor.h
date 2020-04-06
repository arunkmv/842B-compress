#include <stdint-gcc.h>
#include "../common/CompressorConfig.h"
#include "../common/842defs.h"

namespace compress {

    class Decompressor {
    public:
        explicit Decompressor(CompressorConfig *config);

        int process(const uint8_t *input, uint8_t *output);

    private:
        CompressorConfig *config;
        uint8_t *in, *out, *outbeg;
        uint8_t currBit;
        uint64_t currOp;
        uint64_t inputLength, outputLength;

        int loadNextBits(uint8_t n);
    };
}