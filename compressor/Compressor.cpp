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

int compress::Compressor::addToOutput(uint64_t data, uint8_t bits) {
    int nBits = this->currBit + bits;
    //Offset of bit position to the nearest multiple of 8
    int offset = (((nBits - 1) | 7) + 1) - nBits;
    uint64_t outVal;
    uint8_t *outPtr = this->out;

#ifdef DEBUG
    printf("add %u bits %lx\n", (unsigned char) bits, (unsigned long) data);
#endif

    if (bits > 64)
        return -EINVAL;

    if (nBits > 64)
        return splitAdd(data, bits, 32);
    else if (this->outputLength < 8 && nBits > 32 && nBits <= 56)
        return splitAdd(data, bits, 16);
    else if (this->outputLength < 4 && nBits > 16 && nBits <= 24)
        return splitAdd(data, bits, 8);

    if (DIV_ROUND_UP(nBits, 8) > this->outputLength)
        return -ENOSPC;

    outVal = *outPtr & bit_mask[this->currBit];
    data <<= offset;

    if (nBits <= 8)
        *outPtr = outVal | data;
    else if (nBits <= 16)
        *(uint16_t *) outPtr = asBigEndian<uint16_t>(outVal << 8 | data);
    else if (nBits <= 24)
        *(uint32_t *) outPtr = asBigEndian<uint32_t>(outVal << 24 | data << 8);
    else if (nBits <= 32)
        *(uint32_t *) outPtr = asBigEndian<uint32_t>(outVal << 24 | data);
    else if (nBits <= 40)
        *(uint64_t *) outPtr = asBigEndian<uint64_t>(outVal << 56 | data << 24);
    else if (nBits <= 48)
        *(uint64_t *) outPtr = asBigEndian<uint64_t>(outVal << 56 | data << 16);
    else if (nBits <= 56)
        *(uint64_t *) outPtr = asBigEndian<uint64_t>(outVal << 56 | data << 8);
    else
        *(uint64_t *) outPtr = asBigEndian<uint64_t>(outVal << 56 | data);
    //printf("%llx\n", *(u_int64_t *) outPtr);

    this->currBit = nBits;
    if (this->currBit > 7) {
        this->out += (this->currBit / 8);
        this->outputLength -= (this->currBit / 8);
        this->currBit %= 8;
    }

    return 0;
}

int compress::Compressor::splitAdd(uint64_t data, uint8_t bits, int splitAt) {
    uint64_t lowerBitMask = (((uint64_t) 1 << splitAt) - 1);
    int err;

    if (bits < splitAt)
        return -EINVAL;

    err = addToOutput(data >> splitAt, bits - splitAt);
    if (err)
        return err;

    return addToOutput(data & lowerBitMask, splitAt);
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
    this->inputLength -= 8;
}

int compress::Compressor::addRepeatTemplate() {
    int err;
#ifdef DEBUG
    printf("repeat count %x \n", this->repeat_count);
#endif

    if (!this->repeat_count || --(this->repeat_count) > MAX_REPEAT_COUNT)
        return -EINVAL;

    err = addToOutput(OP_REPEAT, OP_BITS);
    if (err)
        return err;

    return addToOutput(this->repeat_count, REPEAT_BITS);
}

int compress::Compressor::addZeroTemplate() {
    return this->addToOutput(OP_ZEROS, OP_BITS);
}

int compress::Compressor::addShortTemplate() {
    int i, err;

    if (!(this->inputLength) || this->inputLength > SHORT_DATA_BITS_MAX)
        return -EINVAL;

    err = addToOutput(OP_SHORT_DATA, OP_BITS);
    if (err)
        return err;

    err = addToOutput(this->inputLength, SHORT_DATA_BITS);
    if (err)
        return err;

    for (i = 0; i < this->inputLength; i++) {
        err = addToOutput(this->in[i], 8);
        if (err)
            return err;
    }

    return 0;
}

int compress::Compressor::addEndTemplate() {
    return addToOutput(OP_END, OP_BITS);
}

int compress::Compressor::addTemplate(int op) {
    int i, err, n = 0;
    uint8_t *templateToAdd = templateCombinations[op];
    bool inval = false;

    if (op >= OPS_MAX)
        return -EINVAL;

#ifdef DEBUG
    printf("template %x\n", templateToAdd[4]);
#endif

    err = addToOutput(templateToAdd[4], OP_BITS);
    if (err)
        return err;

    for (i = 0; i < 4; i++) {

#ifdef DEBUG
        printf("op %x\n", templateToAdd[i]);
#endif

        switch (templateToAdd[i] & OP_AMOUNT) {
            case OP_AMOUNT_8:
                if (n)
                    inval = true;
                else if (templateToAdd[i] & OP_ACTION_INDEX)
                    err = addToOutput(this->pointer8[0], I8_BITS);
                else if (templateToAdd[i] & OP_ACTION_DATA)
                    err = addToOutput(this->data8[0], 64);
                else
                    inval = true;
                break;
            case OP_AMOUNT_4:
                if (n == 2 && templateToAdd[i] & OP_ACTION_DATA)
                    err = addToOutput(getInputData(2, 32), 32);
                else if (n != 0 && n != 4)
                    inval = true;
                else if (templateToAdd[i] & OP_ACTION_INDEX)
                    err = addToOutput(this->pointer4[n >> 2], I4_BITS);
                else if (templateToAdd[i] & OP_ACTION_DATA)
                    err = addToOutput(this->data4[n >> 2], 32);
                else
                    inval = true;
                break;
            case OP_AMOUNT_2:
                if (n != 0 && n != 2 && n != 4 && n != 6)
                    inval = true;
                if (templateToAdd[i] & OP_ACTION_INDEX)
                    err = addToOutput(this->pointer2[n >> 1], I2_BITS);
                else if (templateToAdd[i] & OP_ACTION_DATA)
                    err = addToOutput(this->data2[n >> 1], 16);
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

        if (err)
            return err;

        if (inval) {
            printf("Invalid template %x op %d : %x %x %x %x\n",
                   op, i, templateToAdd[0], templateToAdd[1], templateToAdd[2], templateToAdd[3]);
            return -EINVAL;
        }

        n += templateToAdd[i] & OP_AMOUNT;
    }

    if (n != 8) {
        printf("Invalid template %x op %d : %x %x %x %x\n",
               op, n, templateToAdd[0], templateToAdd[1], templateToAdd[2], templateToAdd[3]);
        return -EINVAL;
    }

    return 0;
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

/**
 * 842 Compressor
 *
 * Compress the uncompressed buffer of length @iLen at @input to the output buffer
 * @output, using no more than @oLen bytes, using the 842 compression format.
 *
 * Returns: 0 on success, error on failure.  The @oLen parameter
 * will contain the number of output bytes written on success, or
 * 0 on error.
 */

int compress::Compressor::process(const uint8_t *input, uint8_t *output) {
    this->inbeg = input;
    this->in = (uint8_t *) input;
    this->out = output;
    this->inputLength = config->inputLength;
    this->outputLength = *(config->outputLength);
    this->hashManager = new compress::HashManager(data8, data4, data2,
                                                  pointer8, pointer4, pointer2);
    int err;
    uint64_t pad, maxLength = this->outputLength;
    *(this->config->outputLength) = 0;

    if (this->inputLength % 8) {
        printf("Can only compress multiples of 8 bytes, but len is len %llu (%% 8 = %llu)\n", this->inputLength,
               this->inputLength % 8);
        return -EINVAL;
    }

    //Last for initial sub-block made different to next
    this->last = ~(*(uint64_t *) input);

    while (this->inputLength > 7) {

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
            err = addRepeatTemplate();
            if (err)
                return err;
            this->repeat_count = 0;
            if (this->last == this->next) {
                updateForNextSubBlock();
                continue;
            }
        }

        //Check if sub block is zero
        if (next == 0) {
            err = addZeroTemplate();
            if (err)
                return err;
        } else {
            processNext();
        }

        updateForNextSubBlock();
    }

    if (this->repeat_count) {
        err = addRepeatTemplate();
        if (err)
            return err;
        this->repeat_count = 0;
    }

    if (this->inputLength > 0) {
        err = addShortTemplate();
        if (err)
            return err;
        this->in += this->inputLength;
        this->inputLength = 0;
    }

    err = addEndTemplate();
    if (err)
        return err;

    uint32_t crc = crc32_be(0, input, this->config->inputLength);
    err = addToOutput(crc, CRC_BITS);
    if (err)
        return err;

    if (this->currBit) {
        this->out++;
        this->outputLength--;
        this->currBit = 0;
    }

    pad = (8 - ((maxLength - this->outputLength) % 8)) % 8;
    if (pad) {
        if (pad > this->outputLength) /* we were so close! */
            return -ENOSPC;
        memset(this->out, 0, pad);
        this->out += pad;
        this->outputLength -= pad;
    }

    if ((maxLength - this->outputLength) > UINT_MAX)
        return -ENOSPC;

    *(this->config->outputLength) = maxLength - this->outputLength;

    if (this->config->displayStats) {
        displayCR(input, output);
    }

    return 0;
}

void compress::Compressor::displayCR(const uint8_t *input, uint8_t *output) {
    float cr = (float) (abs(this->in - input)) / abs(this->out - output);
    printf("Compression ratio: %f\n", cr);
}
