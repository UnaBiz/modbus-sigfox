//  Define the Modbus register types.
#ifndef ALL_TYPES_H
#define ALL_TYPES_H
#include <Arduino.h>
#include "Flash/src/Flash.h"
enum MODBUS_TYPES {
    TYPE_S32 = 3,
    TYPE_U16 = 4,
    TYPE_U16_ARRAY = 5,
    TYPE_U32 = 6,
    TYPE_U32_ARRAY = 7,
    TYPE_U8 = 8,
};

#endif  //  ALL_TYPES_H