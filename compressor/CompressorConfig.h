using namespace std;

namespace compress {

    enum HashFunction {
        BitSubset
    };

    class CompressorConfig {
    public:
        CompressorConfig();

        static bool findEndianness();

        int blockSize;
        int subBlockCount;
        int hashTableSize[3]{};
        int dictionarySize[3]{};
        bool byteOrder;
        compress::HashFunction hash;
    };
}