using namespace std;

namespace compress {

    enum HashFunction {
        BitSubset
    };

    class CompressorConfig {
    public:
        int blockSize;
        int subBlockCount;
        int hashTableSize[3]{};
        int dictionarySize[3]{};
        compress::HashFunction hash;
        CompressorConfig();
    };
}