#include "HashManager.h"

compress::HashManager::HashManager(uint64_t *data8, uint32_t *data4,
                                   uint16_t *data2, int *pointer8, int *pointer4, int *pointer2) {
    this->data8 = data8;
    this->data4 = data4;
    this->data2 = data2;
    this->pointer8 = pointer8;
    this->pointer4 = pointer4;
    this->pointer2 = pointer2;

}

bool compress::HashManager::findIndex8(int index) {
    if ((this->hashTable8).find(data8[index]) == (this->hashTable8).end())
        this->pointer8[index] = INDEX_NOT_FOUND;
    else
        this->pointer8[index] = this->hashTable8[data8[index]];

    return this->pointer8[index] >= 0;
}

bool compress::HashManager::findIndex4(int index) {
    if ((this->hashTable4).find(data4[index]) == (this->hashTable4).end())
        this->pointer4[index] = INDEX_NOT_FOUND;
    else
        this->pointer4[index] = this->hashTable4[data4[index]];

    return this->pointer4[index] >= 0;
}

bool compress::HashManager::findIndex2(int index) {
    if ((this->hashTable2).find(data2[index]) == (this->hashTable2).end())
        this->pointer2[index] = INDEX_NOT_FOUND;
    else
        this->pointer2[index] = this->hashTable2[data2[index]];

    return this->pointer2[index] >= 0;
}

bool compress::HashManager::checkIndex(int b, int index) {
    if (b == 8) {
        return ((this->pointer8[index] == INDEX_NOT_CHECKED) ? findIndex8(index) : this->pointer8[index] >= 0);
    } else if (b == 4) {
        return ((this->pointer4[index] == INDEX_NOT_CHECKED) ? findIndex4(index) : this->pointer4[index] >= 0);
    } else if (b == 2) {
        return ((this->pointer2[index] == INDEX_NOT_CHECKED) ? findIndex2(index) : this->pointer2[index] >= 0);
    }
    return false;
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

void compress::HashManager::resetPointers() {
    this->pointer8[0] = INDEX_NOT_CHECKED;
    this->pointer4[0] = INDEX_NOT_CHECKED;
    this->pointer4[1] = INDEX_NOT_CHECKED;
    this->pointer2[0] = INDEX_NOT_CHECKED;
    this->pointer2[1] = INDEX_NOT_CHECKED;
    this->pointer2[2] = INDEX_NOT_CHECKED;
    this->pointer2[3] = INDEX_NOT_CHECKED;
}

void compress::HashManager::updateHashTables(const uint8_t *currPos, const uint8_t *begPos) {
    uint64_t pos = currPos - begPos;
    uint64_t pos8 = (pos >> 3) % (1 << I8_BITS);
    uint64_t pos4 = (pos >> 2) % (1 << I4_BITS);
    uint64_t pos2 = (pos >> 1) % (1 << I2_BITS);

    this->hashTable8[data8[0]]= pos8;
    this->hashTable4[data4[0]]= pos4 + 0;
    this->hashTable4[data4[1]]= pos4 + 1;
    this->hashTable2[data2[0]]= pos2 + 0;
    this->hashTable2[data2[1]]= pos2 + 1;
    this->hashTable2[data2[2]]= pos2 + 2;
    this->hashTable2[data2[3]]= pos2 + 3;

//    for (auto & i : this->hashTable8)
//        printf("%llx,  %x\n", i.first ,i.second);
}