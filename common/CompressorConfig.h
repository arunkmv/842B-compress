#ifndef _COMPRESSOR_CONFIG_H_
#define _COMPRESSOR_CONFIG_H_

#include <stdlib.h>

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
    };
}

#endif