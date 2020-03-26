#include "HashManager.h"
#include <climits>

#define OP_REPEAT         (0x1B)
#define OP_ZEROS          (0x1C)

#define OP_BITS             (5)
#define REPEAT_BITS         (6)

#define MAX_REPEAT_COUNT  (0x3f)

namespace compress {

    class Compressor {
    public:
        explicit Compressor(CompressorConfig *config);

    private:
        CompressorConfig *config;
        HashManager *hashManager;
        uint8_t *inbeg, *in, *out;
        uint64_t last, next;
        uint8_t currBit;
        uint8_t repeat_count;
        uint64_t data8;
        uint32_t data4[2];
        uint16_t data2[4];

        void process(uint8_t *input, uint8_t *output);

        void addToOutput(uint64_t data, uint8_t bits);

        void loadNextData();

        void updateForNextSubBlock();

        void addRepeatTemplate();

        void addZeroTemplate();

        void splitAdd(uint64_t data, uint8_t bits, int splitAt);

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