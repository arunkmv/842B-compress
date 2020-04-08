#ifndef _COMPRESSOR_CONFIG_H_
#define _COMPRESSOR_CONFIG_H_

#include <stdlib.h>
#include <climits>

using namespace std;

namespace compress {

    class CompressorConfig {
    public:
        size_t inputLength;
        size_t *outputLength;
        bool byteOrder;
        bool displayStats;

        CompressorConfig(size_t iLen, size_t *oLen);

        static bool findEndianness();

        template<typename T>
        T asBigEndian(T u) {
            static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");
            if (this->byteOrder) {
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
    };
}

#endif