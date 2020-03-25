#include "CompressorConfig.h"

compress::CompressorConfig::CompressorConfig() {
    this->blockSize = 32;                       //Block size in bytes
    this->subBlockCount = blockSize / 8;
    this->hashTableSize[0] = 32;
    this->hashTableSize[1] = 64;
    this->hashTableSize[2] = 64;
    this->dictionarySize[0] = 8;
    this->dictionarySize[1] = 16;
    this->dictionarySize[2] = 16;
    this->hash = compress::HashFunction::BitSubset;
    this->byteOrder = findEndianness();
}

bool compress::CompressorConfig::findEndianness() {
    short int word = 0x0001;
    char *byte = (char *) &word;
    return byte[0] != 0;                        //Returns false if CPU is Little Endian and true if Big Endian
}
