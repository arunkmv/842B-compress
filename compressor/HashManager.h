#include <unordered_map>
#include "../common/CompressorConfig.h"
#include "../common/842defs.h"

using namespace std;

namespace compress {

    class HashManager {
    public:
        explicit HashManager(uint64_t *data8, uint32_t *data4, uint16_t *data2,
                             int *pointer8,
                             int *pointer4,
                             int *pointer2);

        void resetPointers();

        bool checkTemplate(int op);

        void updateHashTables(const uint8_t *currPos, const uint8_t *begPos);

    private:
        uint64_t *data8;
        uint32_t *data4;
        uint16_t *data2;
        int *pointer8, *pointer4, *pointer2;
        unordered_map<uint64_t, int> hashTable8;
        unordered_map<uint32_t, int> hashTable4;
        unordered_map<uint16_t, int> hashTable2;

        bool checkIndex(int b, int index);

        bool findIndex8(int index);

        bool findIndex4(int index);

        bool findIndex2(int index);

    };

}