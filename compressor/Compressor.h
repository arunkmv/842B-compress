#include "CompressorConfig.h"
#include "HashManager.h"

#define OP_REPEAT	     (0x1B)
#define OP_ZEROS	     (0x1C)

#define OP_BITS	         (5)
#define REPEAT_BITS	     (6)

#define MAX_REPEAT_COUNT (0x3f)

namespace compress {

    class Compressor {
    public:
        explicit Compressor(CompressorConfig *config, HashManager *hashManager);

    private:
        void process(uint8_t *input, uint8_t *output);

        void addToOutput(uint64_t data, uint8_t bits);

        void loadNextData();

        void updateForNextSubBlock();

        void addRepeatTemplate();

        void addZeroTemplate();

        CompressorConfig *config;
        HashManager *hashManager;
        uint8_t *inbeg, *in, *out;
        uint64_t last, next;
        uint8_t repeat_count;
        uint64_t data8;
        uint32_t data4[2];
        uint16_t data2[4];
    };
}