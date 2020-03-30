#include <stdint-gcc.h>
#include <cstdio>
#include "Compressor.h"

compress::Compressor::Compressor(compress::CompressorConfig *config) {
    this->config = config;
    this->repeat_count = 0;
    this->currBit = 0;
}

static uint8_t bit_mask[8] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};

void compress::Compressor::addToOutput(uint64_t data, uint8_t bits) {
    int nBits = this->currBit + bits;
    //Offset of bit position to the nearest multiple of 8
    int offset = (((nBits - 1) | 7) + 1) - nBits;
    uint64_t outVal;
    uint8_t *outPtr = this->out;

    if (nBits > 64) {
        splitAdd(data, bits, 32);
    }

    outVal = *outPtr & bit_mask[this->currBit];
    data <<= offset;

    if (nBits <= 8)
        *outPtr = outVal | data;
    else if (nBits <= 16)
        *(uint16_t *) outPtr = (outVal << 8 | data);
    else if (nBits <= 24)
        *(uint32_t *) outPtr = (outVal << 24 | data << 8);
    else if (nBits <= 32)
        *(uint32_t *) outPtr = (outVal << 24 | data);
    else if (nBits <= 40)
        *(uint64_t *) outPtr = (outVal << 56 | data << 24);
    else if (nBits <= 48)
        *(uint64_t *) outPtr = (outVal << 56 | data << 16);
    else if (nBits <= 56)
        *(uint64_t *) outPtr = (outVal << 56 | data << 8);
    else
        *(uint64_t *) outPtr = (outVal << 56 | data);
    printf("%llx\n", *(u_int64_t *)outPtr);

    this->currBit = nBits;
    if (this->currBit > 7) {
        this->out += this->currBit / 8;
        this->currBit %= 8;
    }
}

void compress::Compressor::splitAdd(uint64_t data, uint8_t bits, int splitAt) {
    uint64_t lowerBitMask = (((uint64_t) 1 << splitAt) - 1);

    addToOutput(data >> splitAt, bits - splitAt);
    addToOutput(data & lowerBitMask, splitAt);
}

void compress::Compressor::loadNextData() {
    this->data8[0] = ((uint64_t *) this->in)[0];
    this->data4[0] = ((uint32_t *) this->in)[0];
    this->data4[1] = ((uint32_t *) this->in)[1];
    this->data2[0] = ((uint16_t *) this->in)[0];
    this->data2[1] = ((uint16_t *) this->in)[1];
    this->data2[2] = ((uint16_t *) this->in)[2];
    this->data2[3] = ((uint16_t *) this->in)[3];
}

void compress::Compressor::updateForNextSubBlock() {
    this->last = this->next;
    this->in += 8;
}

void compress::Compressor::addRepeatTemplate() {
    addToOutput(OP_REPEAT, OP_BITS);
    addToOutput(this->repeat_count, REPEAT_BITS);
}

void compress::Compressor::addZeroTemplate() {
    this->addToOutput(OP_ZEROS, OP_BITS);
}

void compress::Compressor::addTemplate(int op) {

}

void compress::Compressor::processNext() {

    int i;
    this->hashManager->resetPhrases();

    for (i = 0; i < OPS_MAX - 1; i++) {
        if(this->hashManager->checkTemplate(i)) {
            break;
        }
    }
    addTemplate(i);
}

void compress::Compressor::process(uint8_t *input, uint8_t *output) {
    this->inbeg = input;
    this->in = input;
    this->out = output;
    this->hashManager = new compress::HashManager(config, data8, data4, data2,
            phrase8, phrase4, phrase2);

    //Last for initial sub-block made different to next
    this->last = ~(*(uint64_t *) input);

    for (int i = 0; i < config->subBlockCount; i++) {

        this->next = (*(uint64_t *) this->in);

        loadNextData();

        //Check and add template for repeated sub blocks
        if (this->last == this->next) {
            if (++(this->repeat_count) <= MAX_REPEAT_COUNT) {
                updateForNextSubBlock();
                continue;
            }
        }
        if (this->repeat_count) {
            addRepeatTemplate();
            if (this->last == this->next) {
                updateForNextSubBlock();
                continue;
            }
        }

        //Check if sub block is zero
        if (next == 0) {
            addZeroTemplate();
        } else {
            processNext();
        }

        updateForNextSubBlock();
    }
}
