//  COPY THE CONTENTS FROM https://docs.google.com/spreadsheets/d/1Spf1qlaAH2hzwa2MvuDNv2pzeSAhlU451IRfEV6stMI/edit?usp=sharing
//  Define the name and type of a register.
#ifndef REGISTER_H
#define REGISTER_H

#include <Arduino.h>
#include "Flash/src/Flash.h"
#include "unabiz-arduino-master/Akeru.h"
#include "modbus-types.h"

extern Akeru akeru;

struct Register {
  //  Every Modbus register is defined by these attributes.
  const boolean transmit;  //  True if this register should be transmitted to SIGFOX.
  const uint16_t address;  //  Address of register.
  const uint16_t size;     //  In 16-bit words.
  const char *name;        //  Name of register.
  const char *unit;        //  Unit e.g. "A / 1000".
  const uint16_t type;     //  Type of register from MODBUS_TYPES e.g. TYPE_U32.
};

const uint16_t max_register_size = 58;  //  Computed max number of words per field across all fields.
const Register OLD_all_registers[] = {  //  TODO: Move to Flash memory.
    { true, 50520, 2, "simple_voltage_v1", "V / 100", TYPE_U32 },
    { true, 50526, 2, "frequency_f", "Hz / 100", TYPE_U32 }
};
FLASH_TABLE(uint16_t, all_registers, 6,
    { 1, 50520, 2, 166, 11, 11 },  //  Simple voltage : V1 / V / 100
    { 1, 50526, 2, 57, 10, 11 },  //  Frequency : F / Hz / 100
    { 0, 50512, 2, 67, 9, 11 },  //  Hour Meter / h / 100
    { 0, 50514, 2, 123, 11, 11 },  //  Phase to Phase Voltage: U12 / V / 100
    { 0, 50516, 2, 124, 11, 11 },  //  Phase to Phase Voltage: U23 / V / 100
    { 0, 50518, 2, 125, 11, 11 },  //  Phase to Phase Voltage: U31 / V / 100
    { 0, 50522, 2, 167, 11, 11 },  //  Simple voltage : V2 / V / 100
    { 0, 50524, 2, 168, 11, 11 },  //  Simple voltage : V3 / V / 100
    { 0, 50528, 2, 37, 7, 11 },  //  Current : I1 / A / 1000
    { 0, 50530, 2, 38, 7, 11 },  //  Current : I2 / A / 1000
    { 0, 50532, 2, 39, 7, 11 },  //  Current : I3 / A / 1000
    { 0, 50534, 2, 99, 7, 11 },  //  Neutral Current : In / A / 1000
    { 0, 50536, 2, 2, 15, 5 },  //  ? Active Power +/- : P / W / 0.1
    { 0, 50538, 2, 8, 13, 5 },  //  ? Reactive Power +/- : Q / var / 0.1
    { 0, 50540, 2, 3, 12, 11 },  //  ? Apparent Power : S / VA / 0.1
    { 0, 50542, 2, 7, 3, 5 },  //  ? Power Factor : -: leading et + : lagging : PF / - / 1000
    { 0, 50544, 2, 13, 15, 5 },  //  Active Power phase 1 +/- : P1 / W / 0.1
    { 0, 50546, 2, 14, 15, 5 },  //  Active Power phase 2 +/- : P2 / W / 0.1
    { 0, 50548, 2, 15, 15, 5 },  //  Active Power phase 3 +/- : P3 / W / 0.1
    { 0, 50550, 2, 151, 13, 5 },  //  Reactive Power phase 1 +/- : Q1 / var / 0.1
    { 0, 50552, 2, 152, 13, 5 },  //  Reactive Power phase 2 +/- : Q2 / var / 0.1
    { 0, 50554, 2, 153, 13, 5 },  //  Reactive Power phase 3 +/- : Q3 / var / 0.1
    { 0, 50556, 2, 24, 12, 11 },  //  Apparent Power phase 1 : S1 / VA / 0.1
    { 0, 50558, 2, 25, 12, 11 },  //  Apparent Power phase 2 : S2 / VA / 0.1
    { 0, 50560, 2, 26, 12, 11 },  //  Apparent Power phase 3 : S3 / VA / 0.1
    { 0, 50562, 2, 126, 3, 5 },  //  Power Factor phase 1 -: leading and + : lagging : PF1 / - / 1000
    { 0, 50564, 2, 129, 3, 5 },  //  Power Factor phase 2 -: leading and + : lagging : PF2 / - / 1000
    { 0, 50566, 2, 132, 3, 5 },  //  Power Factor phase 3 -: leading and + : lagging : PF3 / - / 1000
    { 0, 50568, 2, 158, 2, 2 },  //  Reserved / -
    { 0, 50570, 2, 158, 2, 2 },  //  Reserved / -    { true, 50520, 2, 123, 456, TYPE_U32 },
    { true, 50526, 2, 789, 012, TYPE_U32 },
);

//const char * const name##_arr[] PROGMEM = { values }; \
//  _FLASH_STRING_ARRAY name(name##_arr, sizeof(name##_arr) / sizeof(name##_arr[0]));

const char * const zzz_digits_arr[] PROGMEM = {
    PSTR("Zero"),
    PSTR("One")
};

_FLASH_STRING_ARRAY zzz_digits(zzz_digits_arr,
                               sizeof(zzz_digits_arr) / sizeof(zzz_digits_arr[0]));

boolean checkRegister(Register reg) {
  Serial.println("transmit="); Serial.println(reg.transmit);
  if (!reg.transmit) return false;
  Serial.println("address="); Serial.println(reg.address);
  if (reg.address != 50520) return false;
  Serial.println("size="); Serial.println(reg.size);
  if (reg.size != 2) return false;
  Serial.println("name="); Serial.println(reg.name);
  //if (strcmp(reg.name, "simple_voltage_v1") != 0) return false;
  Serial.println("unit="); Serial.println(reg.unit);
  //if (strcmp(reg.unit, "V / 100") != 0) return false;
  Serial.println("type="); Serial.println(reg.type);
  if (reg.type != TYPE_U32) return false;
  return true;
}

void testRegisters() {
  Serial.begin(9600);
  Serial.println("length="); Serial.println(all_registers.rows());
  int i = 0;
  Register reg = {
      (boolean) all_registers[i][0], all_registers[i][1], all_registers[i][2],
      String(all_registers[i][3]).c_str(), String(all_registers[i][4]).c_str(), TYPE_U32 /* all_registers[i][5], */
  };
  if (!checkRegister(reg)) Serial.println("******Test failed");
  else Serial.println("OK");
  for (;;) {}
}

#endif //  REGISTER_H
