#include <cstdio>
#include <cstring>
#include <climits>
#include "HashManager.h"
#include "../common/crc32.h"

namespace compress {

    class Compressor {
    public:
        explicit Compressor(CompressorConfig *config);

        int process(const uint8_t *input, uint8_t *output);

    private:
        CompressorConfig *config;
        HashManager *hashManager;
        const uint8_t *inbeg;
        uint8_t *in, *out;
        uint64_t last, next;
        uint64_t inputLength, outputLength;
        uint8_t currBit;
        uint8_t repeat_count;
        uint64_t data8[1];
        uint32_t data4[2];
        uint16_t data2[4];
        int pointer8[1], pointer4[2], pointer2[4];

        uint64_t getInputData(int n, int bits);

        int addToOutput(uint64_t data, uint8_t bits);

        int splitAdd(uint64_t data, uint8_t bits, int splitAt);

        void loadNextData();

        void updateForNextSubBlock();

        int addRepeatTemplate();

        int addZeroTemplate();

        int addShortTemplate();

        int addEndTemplate();

        int addTemplate(int op);

        void processNext();

        void displayCR(const uint8_t *string1, uint8_t *string2);

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
    };
}