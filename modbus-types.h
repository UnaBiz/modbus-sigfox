//  COPY THE CONTENTS FROM https://docs.google.com/spreadsheets/d/1Spf1qlaAH2hzwa2MvuDNv2pzeSAhlU451IRfEV6stMI/edit?usp=sharing
//  Define the Modbus types.
#ifndef MODBUS_TYPES_H
#define MODBUS_TYPES_H

//  Declare register types.
enum MODBUS_TYPES {
  TYPE_DATETIME_3,
  TYPE_S16,
  TYPE_S32,
  TYPE_STRING_16,
  TYPE_STRING_NORM,
  TYPE_U16,
  TYPE_U16_HEX,
  TYPE_U16_ARRAY,
  TYPE_U32,
  TYPE_U32_ARRAY,
  TYPE_U64_HEX,
  TYPE_U8,
  TYPE_U8_HEX,
};

#endif  //  MODBUS_TYPES_H
