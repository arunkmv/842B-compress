using namespace std;

namespace compress {

    class CompressorConfig {
    public:
        int blockSize;
        bool byteOrder;

        CompressorConfig(int iLen);

        static bool findEndianness();
    };
}