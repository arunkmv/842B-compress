#include "CompressorConfig.h"
#include "HashManager.h"

#define MAX_REPEAT_COUNT (0x3f)

namespace compress {
    enum TemplateType {
        Repeat, Zero
    };

    class Compressor {
    public:
        explicit Compressor(CompressorConfig *config, HashManager *hashManager);

    private:
        void process(uint8_t *input, uint8_t *output);

        void loadNextData();

        void updateForNextSubBlock();

        void addRepeatTemplate();

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