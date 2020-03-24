#include <stdint-gcc.h>
#include "Compressor.h"

compress::Compressor::Compressor(compress::CompressorConfig *config, compress::HashManager *hashManager) {
    this->config = config;
    this->hashManager = hashManager;
    this->repeat_count = 0;
}

void compress::Compressor::loadNextData() {
    this->data8    = ((uint64_t *)this->in)[0];
    this->data4[0] = ((uint32_t *)this->in)[0];
    this->data4[1] = ((uint32_t *)this->in)[1];
    this->data2[0] = ((uint16_t *)this->in)[0];
    this->data2[1] = ((uint16_t *)this->in)[1];
    this->data2[2] = ((uint16_t *)this->in)[2];
    this->data2[3] = ((uint16_t *)this->in)[3];
}

void compress::Compressor::process(uint8_t *input, uint8_t *output) {
    this->inbeg = input;
    this->in = input;
    this->out = output;

    this->last = ~(*(uint64_t *)input);

    for(int i = 0; i < config->subBlockCount; i++) {
        this->next = (*(uint64_t *)input);
        loadNextData();
        if(last == next) {
            processRepeat();
        }
    }

}

void compress::Compressor::processRepeat() {
    if(++this->repeat_count < MAX_REPEAT_COUNT) {

    }
}
