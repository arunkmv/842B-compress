#include "HashManager.h"

compress::HashManager::HashManager(compress::CompressorConfig *config, uint64_t *data8, uint32_t *data4,
                                   uint16_t *data2, int *phrase8, int *phrase4, int *phrase2) {
    this->config = config;
    this->data8 = data8;
    this->data4 = data4;
    this->data2 = data2;
    this->phrase8 = phrase8;
    this->phrase4 = phrase4;
    this->phrase2 = phrase2;

}

bool compress::HashManager::checkTemplate(int op) {
    uint8_t *templateToCheck = templateCombinations[op];
    int i, n = 0;
    bool match;

    if (op >= OPS_MAX) {
        return false;
    }

    for (i = 0; i < 4; i++) {
        if (templateToCheck[i] & OP_ACTION_INDEX) {
            if (templateToCheck[i] & OP_AMOUNT_2)
                match = checkIndex(2, n >> 1);
            else if (templateToCheck[i] & OP_AMOUNT_4)
                match = checkIndex(4, n >> 2);
            else if (templateToCheck[i] & OP_AMOUNT_8)
                match = checkIndex(8, 0);
            else
                return false;
            if (!match)
                return false;
        }
        n += templateToCheck[i] & OP_AMOUNT;
    }

    return true;
}

bool compress::HashManager::checkIndex(int i, int index) {
    return false
}

void compress::HashManager::resetPhrases() {
    this->phrase8[0] = INDEX_NOT_CHECKED;
    this->phrase4[0] = INDEX_NOT_CHECKED;
    this->phrase4[1] = INDEX_NOT_CHECKED;
    this->phrase2[0] = INDEX_NOT_CHECKED;
    this->phrase2[1] = INDEX_NOT_CHECKED;
    this->phrase2[2] = INDEX_NOT_CHECKED;
    this->phrase2[3] = INDEX_NOT_CHECKED;
}
