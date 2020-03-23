

#include "CompressorConfig.h"

namespace compress {
    class Compressor {
    public:
        Compressor(CompressorConfig *config);

    private:
        CompressorConfig *config;
    };
}