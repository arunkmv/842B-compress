#include <cstdio>
#include <cerrno>
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
        uint8_t *in{}, *out{}, *outbeg{};
        uint8_t currBit;
        uint64_t currOp{};
        uint64_t inputLength{}, outputLength{};

        int loadNextBits(u_int64_t *data, uint8_t bits);

        int splitLoad(uint64_t *data, uint8_t bits, int splitAt);

        int processIndex(uint8_t n, uint8_t bits, uint64_t bufferSize);

        int processOPIndex(uint8_t n);

        int processOPData(u_int8_t n);

        int processTemplate();
    };
}