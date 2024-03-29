#include "Decompressor.h"

compress::Decompressor::Decompressor(CompressorConfig *config) {
    this->config = config;
    this->currBit = 0;
}

int compress::Decompressor::loadNextBits(u_int64_t *data, uint8_t bits) {
    int nBits = this->currBit + bits;

    if (bits > 64) {
        fprintf(stderr, "Loading next bits: invalid bit count %u\n", bits);
        return -EINVAL;
    }

    if (nBits > 64)
        return splitLoad(data, bits, 32);
    else if (this->inputLength < 8 && nBits > 32 && nBits <= 56)
        return splitLoad(data, bits, 16);
    else if (this->inputLength < 4 && nBits > 16 && nBits <= 24)
        return splitLoad(data, bits, 8);

    if (DIV_ROUND_UP(nBits, 8) > this->inputLength)
        return -EOVERFLOW;

    if (nBits <= 8)
        *data = *(this->in) >> (8 - nBits);
    else if (nBits <= 16)
        *data = this->config->asBigEndian<uint16_t>(*(uint16_t *) (this->in)) >> (16 - nBits);
    else if (nBits <= 32)
        *data = this->config->asBigEndian<uint32_t>(*(uint32_t *) (this->in)) >> (32 - nBits);
    else
        *data = this->config->asBigEndian<uint64_t>(*(uint64_t *) (this->in)) >> (64 - nBits);

    *data &= ((uint64_t) 1 << (bits)) - 1;

    this->currBit += bits;

    if (this->currBit > 7) {
        this->in += this->currBit / 8;
        this->inputLength -= this->currBit / 8;
        this->currBit %= 8;
    }

    return 0;
}

int compress::Decompressor::splitLoad(uint64_t *data, uint8_t bits, int splitAt) {
    uint64_t temp = 0;
    int err;

    if (bits <= splitAt)
        return -EINVAL;

    err = loadNextBits(&temp, bits - splitAt);
    if (err)
        return err;

    err = loadNextBits(data, splitAt);
    if (err)
        return err;

    *data |= temp << splitAt;
    return 0;
}

int compress::Decompressor::processIndex(uint8_t n, uint8_t bits, uint64_t bufferSize) {
    uint64_t index, offset, totalBytes = round_down(abs(this->out - this->outbeg), 8);
    int err;

    err = loadNextBits(&index, bits);
    if (err)
        return err;

    offset = index * n;

    if (totalBytes > bufferSize) {
        uint64_t bufferSection = round_down(totalBytes, bufferSize);
        uint64_t bufferPosition = totalBytes - bufferSection;

        if (offset >= bufferPosition)
            bufferSection -= bufferSize;

        offset += bufferSection;
    }

    if (offset + n > totalBytes) {
        fprintf(stderr, "index%x %lx points beyond end %lx\n", n,
               (unsigned long) offset, (unsigned long) totalBytes);
        return -EINVAL;
    }

    if (n != 2 && n != 4 && n != 8)
        printf("invalid index size %x\n", n);

    memcpy(this->out, &this->outbeg[offset], n);
    this->out += n;
    this->outputLength -= n;

    return 0;
}

int compress::Decompressor::processOPIndex(uint8_t n) {
    switch (n) {
        case 2:
            return processIndex(2, I2_BITS, I2_FIFO_SIZE);
        case 4:
            return processIndex(4, I4_BITS, I4_FIFO_SIZE);
        case 8:
            return processIndex(8, I8_BITS, I8_FIFO_SIZE);
        default:
            return -EINVAL;
    }
}

int compress::Decompressor::processOPData(uint8_t n) {
    uint64_t data;
    int err;

    if (n > this->outputLength)
        return -ENOSPC;

    err = loadNextBits(&data, n * 8);
    if (err)
        return err;

    switch (n) {
        case 2:
            *(uint16_t *) (this->out) = this->config->asBigEndian<uint16_t>(data);
            break;
        case 4:
            *(uint32_t *) (this->out) = this->config->asBigEndian<uint32_t>(data);
            break;
        case 8:
            *(uint64_t *) (this->out) = this->config->asBigEndian<uint64_t>(data);
            break;
        default:
            return -EINVAL;
    }

    this->out += n;
    this->outputLength -= n;

    return 0;
}

int compress::Decompressor::processTemplate() {
    int i, err = 0;

    if (this->currOp >= OPS_MAX)
        return -EINVAL;

    for (i = 0; i < 4; i++) {
        uint8_t op = decompTemplates[this->currOp][i];

#ifdef DEBUG
        printf("op is %x\n", op);
#endif

        switch (op & OP_ACTION) {
            case OP_ACTION_DATA:
                err = processOPData(op & OP_AMOUNT);
                break;
            case OP_ACTION_INDEX:
                err = processOPIndex(op & OP_AMOUNT);
                break;
            case OP_ACTION_NOOP:
                break;
            default:
                fprintf(stderr, "Invalid op %x\n", op);
                return -EINVAL;
        }
        if (err)
            return err;
    }
    return 0;
}

int compress::Decompressor::process(const uint8_t *input, uint8_t *output) {
    this->in = (uint8_t *) input;
    this->out = output;
    this->outbeg = output;
    this->inputLength = this->config->inputLength;
    this->outputLength = *this->config->outputLength;

    int err;
    uint64_t repeat, bytes, temp, crc, maxLength = this->outputLength;
    *(this->config->outputLength) = 0;

    do {

        err = loadNextBits(&(this->currOp), OP_BITS);
        if (err)
            return err;

#ifdef DEBUG
        printf("template is %lx\n", (unsigned long) this->currOp);
#endif

        switch (this->currOp) {
            case OP_REPEAT:
                err = loadNextBits(&repeat, REPEAT_BITS);
                if (err)
                    return err;

                if (this->out == out)
                    return -EINVAL;

                repeat++;

                if (repeat * 8 > this->outputLength)
                    return -ENOSPC;

                while (repeat-- > 0) {
                    memcpy(this->out, this->out - 8, 8);
                    this->out += 8;
                    this->outputLength = 8;
                }

                break;
            case OP_ZEROS:
                if (this->outputLength < 8)
                    return -ENOSPC;

                memset(this->out, 0, 8);
                this->out += 8;
                this->outputLength -= 8;

                break;
            case OP_SHORT_DATA:
                err = loadNextBits(&bytes, SHORT_DATA_BITS);
                if (err)
                    return err;

                if (!bytes || bytes > SHORT_DATA_BITS_MAX)
                    return -EINVAL;

                while (bytes-- > 0) {
                    err = loadNextBits(&temp, 8);
                    if (err)
                        return err;

                    *this->out = (uint8_t) temp;
                    this->out += 8;
                    this->outputLength -= 8;
                }

                break;
            case OP_END:

                break;
            default:
                err = processTemplate();
                if (err)
                    return err;

                break;
        }
    } while (this->currOp != OP_END);

    err = loadNextBits(&crc, CRC_BITS);
    if (err)
        return err;

    if (crc != (uint64_t)crc32_be(0, this->outbeg, maxLength - this->outputLength)) {
        fprintf(stderr, "CRC mismatch in decompressed data\n");
        return -EINVAL;
    }

    if ((maxLength - this->outputLength) > UINT_MAX)
        return -ENOSPC;

    *(this->config->outputLength) = maxLength - this->outputLength;

    return 0;
}
