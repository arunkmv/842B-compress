using namespace std;

namespace compress {

    class CompressorConfig {
    public:
        int blockSize;
        bool byteOrder;
        bool displayStats;

        CompressorConfig(int iLen);

        static bool findEndianness();
    };
}