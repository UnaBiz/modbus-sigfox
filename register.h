//  COPY THE CONTENTS FROM https://docs.google.com/spreadsheets/d/1Spf1qlaAH2hzwa2MvuDNv2pzeSAhlU451IRfEV6stMI/edit?usp=sharing
//  Define the name and type of a register.
#ifndef REGISTER_H
#define REGISTER_H

#include <avr/pgmspace.h>
#include "unabiz-arduino-master/Akeru.h"
#include "modbus-types.h"

extern Akeru akeru;

struct Register {
  //  Every Modbus register is defined by its address and size.
  const char *name;  //  Name of register.
  const uint16_t address;  //  Address of register.
  const uint16_t size;  //  In 16-bit words.
  const char *unit;  //  Unit e.g. "A / 1000".
  const enum MODBUS_TYPES type;  //  Type of register e.g. TYPE_U32.
  const boolean transmit;  //  True if this register should be transmitted to SIGFOX.
};

const uint16_t max_register_size = 8;  //  TODO: Compute max number of words per field across all fields.
const PROGMEM Register all_registers[] = {
    { "simple_voltage_v1", 50520, 2, "V / 100", TYPE_U32, true },
    { "frequency_f", 50526, 2, "Hz / 100", TYPE_U32, true }
};

void parseType(Register reg, uint16_t data[], String &displayValue, String &transmitValue) {
  //  Parse register types.
  displayValue = ""; transmitValue = "";
  if (false) {}
  else if (reg.type == TYPE_DATETIME_3) {
    for (uint16_t i = 0; i < 3; i++) {
      uint16_t value = data[i];
      if (i > 0) displayValue = displayValue + " ";
      displayValue.concat(akeru.toHex(value));
      transmitValue.concat(akeru.toHex(value));
    }
  }
  else if (reg.type == TYPE_S16) {
    int16_t result = data[0];
    displayValue.concat(result);
    transmitValue.concat(akeru.toHex(result));
  }
  else if (reg.type == TYPE_S32) {
    int32_t result = ((int32_t) data[0] << 16) + data[1];
    displayValue.concat(result);
    transmitValue.concat(akeru.toHex(result));
  }
  else if (reg.type == TYPE_STRING_16) {
    //  Unicode string.
    const uint16_t count = reg.size;
    for (uint16_t i = 0; i < count; i++) {
      char value = data[i];
      displayValue.concat(value);
      transmitValue.concat(akeru.toHex(value));
    }
  }
  else if (reg.type == TYPE_STRING_NORM) {
    //  ASCII string.
    const uint16_t count = reg.size;
    for (uint16_t i = 0; i < count; i++) {
      uint16_t value = data[i];
      char ch1 = value >> 8;
      char ch2 = value & 0xff;
      displayValue.concat(ch1); displayValue.concat(ch2);
      transmitValue.concat(akeru.toHex(ch1));
      transmitValue.concat(akeru.toHex(ch2));
    }
  }
  else if (reg.type == TYPE_U16) {
    uint16_t result = data[0];
    displayValue.concat(result);
    transmitValue.concat(akeru.toHex(result));
  }
  else if (reg.type == TYPE_U16_HEX) {
    uint16_t result = data[0];
    displayValue.concat(akeru.toHex(result));
    transmitValue.concat(akeru.toHex(result));
  }
  else if (reg.type == TYPE_U16_ARRAY) {
    const uint16_t count = reg.size;
    for (uint16_t i = 0; i < count; i++) {
      uint16_t value = data[i];
      if (i > 0) displayValue = displayValue + ", ";
      displayValue.concat(value);
      transmitValue.concat(akeru.toHex(value));
    }
  }
  else if (reg.type == TYPE_U32) {
    uint32_t result = ((uint32_t) data[0] << 16) + data[1];
    displayValue.concat(result);
    transmitValue.concat(akeru.toHex(result));
  }
  else if (reg.type == TYPE_U32_ARRAY) {
    const uint32_t count = reg.size / 2;
    for (uint16_t i = 0; i < count; i++) {
      uint32_t value = data[i];
      if (i > 0) displayValue = displayValue + ", ";
      displayValue.concat(value);
      transmitValue.concat(akeru.toHex(value));
    }
  }
  else if (reg.type == TYPE_U64_HEX) {
    for (uint16_t i = 0; i < 8; i++) {
      uint16_t value = data[i];
      displayValue.concat(akeru.toHex(value));
      transmitValue.concat(akeru.toHex(value));
    }
  }
  else if (reg.type == TYPE_U8) {
    uint8_t result = (uint8_t) data[0];
    displayValue.concat(result);
    transmitValue.concat(akeru.toHex(result));
  }
  else if (reg.type == TYPE_U8_HEX) {
    uint8_t result = (uint8_t) data[0];
    displayValue.concat(akeru.toHex(result));
    transmitValue.concat(akeru.toHex(result));
  }
}

#endif //  REGISTER_H
