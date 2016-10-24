//  Define the properties of a register.
#ifndef REGISTER_H
#define REGISTER_H

#include <Arduino.h>

struct Register {
  //  Every Modbus register is defined by these attributes.
  const boolean transmit;  //  True if this register should be transmitted to SIGFOX.
  const uint16_t address;  //  Address of register.
  const uint16_t size;     //  In 16-bit words.
  const uint16_t name_index;        //  Name of register, indexed to all_names.
  const uint16_t unit_index;        //  Unit e.g. "A / 1000", indexed to all_units.
  const uint16_t type;     //  Type of register from MODBUS_TYPES e.g. TYPE_U32.
};

const uint16_t max_register_size = 58;  //  Computed max number of words per field across all fields.

#endif //  REGISTER_H
