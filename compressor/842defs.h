#include <stdint-gcc.h>

#define OP_REPEAT         (0x1B)
#define OP_ZEROS          (0x1C)

#define OP_BITS             (5)
#define REPEAT_BITS         (6)

#define MAX_REPEAT_COUNT  (0x3f)
#define OPS_MAX             (26)

#define INDEX_NOT_CHECKED   (-2)
#define INDEX_NOT_FOUND     (-1)

#define OP_ACTION	      (0x70)
#define OP_ACTION_INDEX	  (0x10)
#define OP_ACTION_DATA	  (0x20)
#define OP_ACTION_NOOP	  (0x40)
#define OP_AMOUNT	      (0x0f)
#define OP_AMOUNT_0	      (0x00)
#define OP_AMOUNT_2	      (0x02)
#define OP_AMOUNT_4	      (0x04)
#define OP_AMOUNT_8	      (0x08)

#define D2		(OP_ACTION_DATA  | OP_AMOUNT_2)
#define D4		(OP_ACTION_DATA  | OP_AMOUNT_4)
#define D8		(OP_ACTION_DATA  | OP_AMOUNT_8)
#define I2		(OP_ACTION_INDEX | OP_AMOUNT_2)
#define I4		(OP_ACTION_INDEX | OP_AMOUNT_4)
#define I8		(OP_ACTION_INDEX | OP_AMOUNT_8)
#define N0		(OP_ACTION_NOOP  | OP_AMOUNT_0)

static uint8_t templateCombinations[OPS_MAX][5] = { /* params size in bits */
        { I8, N0, N0, N0, 0x19 }, /* 8 */
        { I4, I4, N0, N0, 0x18 }, /* 18 */
        { I4, I2, I2, N0, 0x17 }, /* 25 */
        { I2, I2, I4, N0, 0x13 }, /* 25 */
        { I2, I2, I2, I2, 0x12 }, /* 32 */
        { I4, I2, D2, N0, 0x16 }, /* 33 */
        { I4, D2, I2, N0, 0x15 }, /* 33 */
        { I2, D2, I4, N0, 0x0e }, /* 33 */
        { D2, I2, I4, N0, 0x09 }, /* 33 */
        { I2, I2, I2, D2, 0x11 }, /* 40 */
        { I2, I2, D2, I2, 0x10 }, /* 40 */
        { I2, D2, I2, I2, 0x0d }, /* 40 */
        { D2, I2, I2, I2, 0x08 }, /* 40 */
        { I4, D4, N0, N0, 0x14 }, /* 41 */
        { D4, I4, N0, N0, 0x04 }, /* 41 */
        { I2, I2, D4, N0, 0x0f }, /* 48 */
        { I2, D2, I2, D2, 0x0c }, /* 48 */
        { I2, D4, I2, N0, 0x0b }, /* 48 */
        { D2, I2, I2, D2, 0x07 }, /* 48 */
        { D2, I2, D2, I2, 0x06 }, /* 48 */
        { D4, I2, I2, N0, 0x03 }, /* 48 */
        { I2, D2, D4, N0, 0x0a }, /* 56 */
        { D2, I2, D4, N0, 0x05 }, /* 56 */
        { D4, I2, D2, N0, 0x02 }, /* 56 */
        { D4, D2, I2, N0, 0x01 }, /* 56 */
        { D8, N0, N0, N0, 0x00 }, /* 64 */
};
