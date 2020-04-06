#include <cstdio>
#include <cerrno>
#include <climits>
#include <cstring>
#include "../common/CompressorConfig.h"
#include "../common/842defs.h"
#include "../common/crc32.h"

using namespace std;

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

        int loadNextBits(u_int64_t *data, uint8_t bits);

        int splitLoad(uint64_t *data, uint8_t bits, int splitAt);

        template<typename T>
        T asBigEndian(T u) {
            static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");
            if (this->config->byteOrder) {
                return u;
            }

            union {
                T u;
                unsigned char u8[sizeof(T)];
            } source, dest;

            source.u = u;

            for (size_t k = 0; k < sizeof(T); k++)
                dest.u8[k] = source.u8[sizeof(T) - k - 1];

            return dest.u;
        }

        int processTemplate();
    };
}