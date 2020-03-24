#include "CompressorConfig.h"

namespace compress {

class HashManager {
public:
    explicit HashManager(CompressorConfig *config);

private:
    CompressorConfig *config;
};

}