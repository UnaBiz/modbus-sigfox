//  Generated list of all units based on Modbus table.
#ifndef ALL_UNITS_H
#define ALL_UNITS_H
#include <Arduino.h>
#include "Flash/src/Flash.h"
const uint16_t max_unit_length = 12; const char all_units[] PROGMEM =
"- / 1000\0"
"? / 10\0"
"% / 10\0"
"A\0"
"A / 1000\0"
"h\0"
"h / 100\0"
"Hz / 100\0"
"V / 100\0"
"VA / 0.1\0"
"var / 0.1\0"
"varh / 0.001\0"
"W / 0.1\0"
"Wh / 0.001\0"
"Wh / 1E-06\0"
;

#endif  //  ALL_UNITS_H