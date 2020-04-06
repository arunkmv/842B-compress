#include "Decompressor.h"

compress::Decompressor::Decompressor(CompressorConfig *config) {
    this->config = config;
    this->currBit = 0;
}

int compress::Decompressor::loadNextBits(uint8_t bits) {
    int nBits = this->currBit + bits;
    uint64_t data;

    if (bits > 64) {
        printf("load next bits invalid bit count %u\n", bits);
        return -EINVAL;
    }

    if (nBits > 64)
        splitLoad(bits, 32);
    else if (this->inputLength < 8 && nBits > 32 && nBits <= 56)
        return splitLoad(bits, 16);
    else if (this->inputLength < 4 && nBits > 16 && nBits <= 24)
        return splitLoad(bits, 8);

    if (DIV_ROUND_UP(bits, 8) > this->inputLength)
        return -EOVERFLOW;

    if (nBits <= 8)
        data = *in >> (8 - nBits);
    else if (nBits <= 16)
        data = asBigEndian<uint16_t>(*(uint16_t *) in) >> (16 - nBits);
    else if (nBits <= 32)
        data = asBigEndian<uint32_t>(*(uint32_t *) in) >> (32 - nBits);
    else
        data = asBigEndian<uint64_t>(*(uint64_t *) in) >> (64 - nBits);

    this->currOp = data & ((1 << (bits - 1)) - 1);

    this->currBit += bits;

    if (this->currBit > 7) {
        this->in += this->currBit / 8;
        this->inputLength -= this->currBit / 8;
        this->currBit %= 8;
    }

    return 0;
}

int compress::Decompressor::splitLoad(uint8_t bits, int splitAt) {
    return 0;
}

int compress::Decompressor::process(const uint8_t *input, uint8_t *output) {
    this->in = (uint8_t *) input;
    this->out = output;
    this->outbeg = output;
    this->inputLength = this->config->inputLength;
    this->outputLength = *this->config->outputLength;

    int err;
    uint64_t maxLength = this->outputLength;
    *(this->config->outputLength) = 0;

    do {
        err = loadNextBits(OP_BITS);
        if (err)
            return err;
    } while (this->currOp != OP_END);
    return 0;
}
