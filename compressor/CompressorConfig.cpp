#include "CompressorConfig.h"

compress::CompressorConfig::CompressorConfig(int iLen) {
    this->blockSize = iLen;                       //Block size in bytes
    this->byteOrder = findEndianness();
    this->displayStats = true;
}

bool compress::CompressorConfig::findEndianness() {
    short int word = 0x0001;
    char *byte = (char *) &word;
    return byte[0] == 0;                        //Returns false if CPU is Little Endian and true if Big Endian
}
