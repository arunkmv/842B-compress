using namespace std;

namespace compress {

    class CompressorConfig {
    public:
        int blockSize;
        int subBlockCount;
        bool byteOrder;

        CompressorConfig();

        static bool findEndianness();
    };
}