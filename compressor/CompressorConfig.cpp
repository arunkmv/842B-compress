#include "CompressorConfig.h"

compress::CompressorConfig::CompressorConfig() {
    this->blockSize = 32;                       //Block size in bytes
    this->subBlockCount = blockSize / 8;
    this->byteOrder = findEndianness();
}

bool compress::CompressorConfig::findEndianness() {
    short int word = 0x0001;
    char *byte = (char *) &word;
    return byte[0] == 0;                        //Returns false if CPU is Little Endian and true if Big Endian
}
