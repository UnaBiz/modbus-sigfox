//  COPY THE CONTENTS FROM https://docs.google.com/spreadsheets/d/1Spf1qlaAH2hzwa2MvuDNv2pzeSAhlU451IRfEV6stMI/edit?usp=sharing
//  Define the Modbus types.
#ifndef MODBUS_TYPES_H
#define MODBUS_TYPES_H

//  Declare register types.
enum MODBUS_TYPES {
  TYPE_S32 = 3,
  TYPE_U16 = 4,
  TYPE_U16_ARRAY = 5,
  TYPE_U32 = 6,
  TYPE_U32_ARRAY = 7,
  TYPE_U8 = 8,
};

#endif  //  MODBUS_TYPES_H
