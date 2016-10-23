//  COPY THE CONTENTS FROM https://docs.google.com/spreadsheets/d/1Spf1qlaAH2hzwa2MvuDNv2pzeSAhlU451IRfEV6stMI/edit?usp=sharing
//  Define the Modbus types.
#ifndef MODBUS_TYPES_H
#define MODBUS_TYPES_H

//  Declare register types.
enum MODBUS_TYPES {
    TYPE_S16 = 3,
    TYPE_S32 = 4,
    TYPE_STRING_16 = 5,
    TYPE_STRING_NORM = 6,
    TYPE_U16 = 7,
    TYPE_U16_HEX = 8,
    TYPE_U16_ARRAY = 9,
    TYPE_U32 = 10,
    TYPE_U32_ARRAY = 11,
    TYPE_U64_HEX = 12,
    TYPE_U8 = 13,
};

#endif  //  MODBUS_TYPES_H
