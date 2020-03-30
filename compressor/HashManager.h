#include "CompressorConfig.h"
#include "842defs.h"

namespace compress {

    class HashManager {
    public:
        explicit HashManager(CompressorConfig *config, uint64_t *data8, uint32_t *data4, uint16_t *data2,
                             int *phraseIndex8,
                             int *phraseIndex4,
                             int *phraseIndex2);

        void resetPhrases();

        bool checkTemplate(int op);

    private:
        CompressorConfig *config;
        uint64_t *data8;
        uint32_t *data4;
        uint16_t *data2;
        int *phrase8, *phrase4, *phrase2;

        bool checkIndex(int i, int index);
    };

}