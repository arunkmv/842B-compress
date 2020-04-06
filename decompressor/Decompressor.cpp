#include "Decompressor.h"

compress::Decompressor::Decompressor(CompressorConfig *config) {
    this->config = config;
    this->currBit = 0;
}

int compress::Decompressor::loadNextBits(uint8_t n) {
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