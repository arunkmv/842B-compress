#include <cstdio>
#include "Compressor.h"

compress::Compressor::Compressor(compress::CompressorConfig *config) {
    this->config = config;
    this->repeat_count = 0;
    this->currBit = 0;
}

static uint8_t bit_mask[8] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};

uint64_t compress::Compressor::getInputData(int n, int bits) {
    if (bits == 16)
        return asBigEndian(*((uint16_t *) (this->in + n)));
    else if (bits == 32)
        return asBigEndian(*((uint32_t *) (this->in + n)));
    else if (bits == 64)
        return asBigEndian(*((uint64_t *) (this->in + n)));
    return 0;
}

void compress::Compressor::addToOutput(uint64_t data, uint8_t bits) {
    int nBits = this->currBit + bits;
    //Offset of bit position to the nearest multiple of 8
    int offset = (((nBits - 1) | 7) + 1) - nBits;
    uint64_t outVal;
    uint8_t *outPtr = this->out;

#ifdef DEBUG
    printf("add %u bits %lx\n", (unsigned char) bits, (unsigned long) data);
#endif

    if (nBits > 64) {
        return splitAdd(data, bits, 32);
    }

    outVal = *outPtr & bit_mask[this->currBit];
    data <<= offset;

    if (nBits <= 8)
        *outPtr = outVal | data;
    else if (nBits <= 16)
        *(uint16_t *) outPtr = asBigEndian(outVal << 8 | data);
    else if (nBits <= 24)
        *(uint32_t *) outPtr = asBigEndian(outVal << 24 | data << 8);
    else if (nBits <= 32)
        *(uint32_t *) outPtr = asBigEndian(outVal << 24 | data);
    else if (nBits <= 40)
        *(uint64_t *) outPtr = asBigEndian(outVal << 56 | data << 24);
    else if (nBits <= 48)
        *(uint64_t *) outPtr = asBigEndian(outVal << 56 | data << 16);
    else if (nBits <= 56)
        *(uint64_t *) outPtr = asBigEndian(outVal << 56 | data << 8);
    else
        *(uint64_t *) outPtr = asBigEndian(outVal << 56 | data);
    //printf("%llx\n", *(u_int64_t *) outPtr);

    this->currBit = nBits;
    if (this->currBit > 7) {
        this->out += (this->currBit / 8);
        this->currBit %= 8;
    }
}

void compress::Compressor::splitAdd(uint64_t data, uint8_t bits, int splitAt) {
    uint64_t lowerBitMask = (((uint64_t) 1 << splitAt) - 1);

    addToOutput(data >> splitAt, bits - splitAt);
    addToOutput(data & lowerBitMask, splitAt);
}

void compress::Compressor::loadNextData() {
    this->data8[0] = getInputData(0, 64);
    this->data4[0] = getInputData(0, 32);
    this->data4[1] = getInputData(4, 32);
    this->data2[0] = getInputData(0, 16);
    this->data2[1] = getInputData(2, 16);
    this->data2[2] = getInputData(4, 16);
    this->data2[3] = getInputData(6, 16);
}

void compress::Compressor::updateForNextSubBlock() {
    this->hashManager->updateHashTables(this->in, this->inbeg);
    this->last = this->next;
    this->in += 8;
    this->bSize -= 8;
}

void compress::Compressor::addRepeatTemplate() {
#ifdef DEBUG
    printf("repeat count %x \n", this->repeat_count);
#endif
    addToOutput(OP_REPEAT, OP_BITS);
    addToOutput(this->repeat_count, REPEAT_BITS);
}

void compress::Compressor::addZeroTemplate() {
    this->addToOutput(OP_ZEROS, OP_BITS);
}

void compress::Compressor::addShortTemplate() {
    int i;
    if (!(this->bSize) || this->bSize > SHORT_DATA_BITS_MAX)
        return;

    addToOutput(OP_SHORT_DATA, OP_BITS);
    addToOutput(this->bSize, SHORT_DATA_BITS);

    for (i = 0; i < this->bSize; i++)
        addToOutput(this->in[i], 8);
}

void compress::Compressor::addEndTemplate() {
    addToOutput(OP_END, OP_BITS);
}

void compress::Compressor::addTemplate(int op) {
    int i, n = 0;
    uint8_t *templateToAdd = templateCombinations[op];
    bool inval = false;

    #ifdef DEBUG
        printf("template %x\n", templateToAdd[4]);
    #endif

    addToOutput(templateToAdd[4], OP_BITS);
    for (i = 0; i < 4; i++) {

    #ifdef DEBUG
            printf("op %x\n", templateToAdd[i]);
    #endif

        switch (templateToAdd[i] & OP_AMOUNT) {
            case OP_AMOUNT_8:
                if (n)
                    inval = true;
                else if (templateToAdd[i] & OP_ACTION_INDEX)
                    addToOutput(this->pointer8[0], I8_BITS);
                else if (templateToAdd[i] & OP_ACTION_DATA)
                    addToOutput(this->data8[0], 64);
                else
                    inval = true;
                break;
            case OP_AMOUNT_4:
                if (n == 2 && templateToAdd[i] & OP_ACTION_DATA)
                    addToOutput(getInputData(2, 32), 32);
                else if (n != 0 && n != 4)
                    inval = true;
                else if (templateToAdd[i] & OP_ACTION_INDEX)
                    addToOutput(this->pointer4[n >> 2], I4_BITS);
                else if (templateToAdd[i] & OP_ACTION_DATA)
                    addToOutput(this->data4[n >> 2], 32);
                else
                    inval = true;
                break;
            case OP_AMOUNT_2:
                if (n != 0 && n != 2 && n != 4 && n != 6)
                    inval = true;
                if (templateToAdd[i] & OP_ACTION_INDEX)
                    addToOutput(this->pointer2[n >> 1], I2_BITS);
                else if (templateToAdd[i] & OP_ACTION_DATA)
                    addToOutput(this->data2[n >> 1], 16);
                else
                    inval = true;
                break;
            case OP_AMOUNT_0:
                inval = (n != 8) || !(templateToAdd[i] & OP_ACTION_NOOP);
                break;
            default:
                inval = true;
                break;
        }

        n += templateToAdd[i] & OP_AMOUNT;
    }

    if (inval || n != 8) {
        printf("Invalid template\n");
    }
}

void compress::Compressor::processNext() {

    int i;
    this->hashManager->resetPointers();

    for (i = 0; i < OPS_MAX - 1; i++) {
        if (this->hashManager->checkTemplate(i)) {
            break;
        }
    }
    addTemplate(i);
}

void compress::Compressor::process(const uint8_t *input, uint8_t *output) {
    this->inbeg = input;
    this->in = (uint8_t *)input;
    this->out = output;
    this->bSize = config->blockSize;
    this->hashManager = new compress::HashManager(config, data8, data4, data2,
                                                  pointer8, pointer4, pointer2);

    //Last for initial sub-block made different to next
    this->last = ~(*(uint64_t *) input);

    while (this->bSize > 7) {

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
            this->repeat_count = 0;
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

    if (this->repeat_count) {
        addRepeatTemplate();
        this->repeat_count = 0;
    }

    if (this->bSize > 0) {
        addShortTemplate();
        this->in += this->bSize;
        this->bSize = 0;
    }

    addEndTemplate();

    uint32_t crc = crc32_be(0, input, this->config->blockSize);
    addToOutput(crc, CRC_BITS);

    if (this->currBit) {
        this->out++;
        this->currBit = 0;
    }
}
