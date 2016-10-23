//  Connect to a Modbus device via RS485 and send Modbus data to the SIGFOX cloud.
//  Needs a UnaBiz SIGFOX adapter, DFRobot RS485 shield and FTDI 5V UART-to-USB adapter (e.g. uUSB-PA5)
//  https://www.dfrobot.com/index.php?route=product/product&product_id=1024
//  https://www.dfrobot.com/wiki/index.php/RS485_Shield
//  http://www.4dsystems.com.au/product/uUSB_PA5/

//  Connect Modbus device to RS485 shield as follows:
//  Modbus Device + -> RS485 Shield A
//  Modbus Device - -> RS485 Shield B
//  Modbus Device 0 -> RS485 Shield GND
//  RS485 shield mode must be switched to Manual, not Auto

//  Need a debug serial port, because the USB port UART is used by RS485.
//  To download the sketch, turn the RS485 shield off using the ON-OFF switch on the shield.
//  Then restart the board and switch the RS485 shield on.

//  Connect an FTDI 5V UART-to-USB adapter (e.g. uUSB-PA5) to Arduino D6, D7 and GND.
//  FTDI TX -> Arduino D6
//  FTDI RX -> Arduino D7
//  FTDI GND -> Arduino GND

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Flash/src/Flash.h"
#include "unabiz-arduino-master/Akeru.h"
#include "ModbusMaster/src/ModbusMaster.h"
#include "modbus-types.h"
#include "register.h"

////////////////////////////////////////////////////////////
//  Begin SIGFOX Module Declaration

// SIGFOX module I/O definition
#define TX 5  //  For UnaBiz / Akene
#define RX 4  //  For UnaBiz / Akene
Akeru akeru(RX, TX);  // Instance of SIGFOX module.

//  End SIGFOX Module Declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin Sensor Declaration

#ifdef FORWARD_MODE
//  In Forward Mode, input/output on this port will be forwarded to RS485.
//  Used for allowing Simply Mobbus Master tool to communicate with real device.
SoftwareSerial forwardSerial(6, 7); // RX, TX
SoftwareSerial debugSerial(8, 9); // RX, TX.  Fake the ports so that debug is discarded.

#else  //  FORWARD_NODE
//  Need a debug serial port, because the USB port UART is used by RS485.
//  Connect an FTDI 5V UART-to-USB adapter (e.g. uUSB-PA5) to Arduino D6, D7 and GND.
//  FTDI TX -> Arduino D6, FTDI RX -> Arduino D7, FTDI GND -> Arduino GND. 
//  Don't use D2 because it's used by RS485 shield in manual transmission mode.
SoftwareSerial debugSerial(6, 7); // RX, TX
#endif  //  FORWARD_NODE

//  Parameters for DIRIS A20 based on:
//  http://www.socomec.com/range-single-circuit-multifunction-meters_en.html?product=/diris-a20_en.html&view=documentation
//  http://www.socomec.com/files/live/sites/systemsite/files/DOCUMENTATION/SCP_hors_cata/fip_11013.pdf
//  http://www.socomec.com/files/live/sites/systemsite/files/SCP/6_gestion_energie/diris/diris_a20/535746a_A20.pdf
//  http://www.socomec.com/files/live/sites/systemsite/files/SCP/6_gestion_energie/diris/diris_a20/PRO_880103_DirisA20.html
//  http://www.socomec.com/files/live/sites/systemsite/files/SCP/6_gestion_energie/diris/diris_a40/876_581_C.pdf
//  See diris-a20.html, diris-a20.png, diris-a20.pdf, diris-a20-operating-inst.pdf, diris-rs485-technical-guide.pdf.

//  List of registers to be read and transmitted optionally.
const uint16_t all_registers_count = sizeof(all_registers) / sizeof(Register);

//  Use D2 port as send/receive control for RS485 shield.
//  Set to high for the sending state, set to low for receiving state
const int rs485_transmit = 2;
const int slaveID = 5;  //  Default Slave ID of the Modbus device.
ModbusMaster node;  //  Instantiate ModbusMaster object
char debugOutput[128];  //  For debug output by ModusMaster.
size_t debugOutputSize = sizeof(debugOutput);

void debugPrint(const char *s) {
  //  Print to debug output.
#ifdef DEBUG
  strlcat(debugOutput, s, debugOutputSize);
#endif // DEBUG
}

void debugPrint(uint16_t u) {
  //  Print to debug output.
#ifdef DEBUG
  strlcat(debugOutput, String(u).c_str(), debugOutputSize);
#endif // DEBUG
}

void debugPrint(const String &s) {
  //  Print to debug output.
#ifdef DEBUG
  strlcat(debugOutput, s.c_str(), debugOutputSize);
#endif // DEBUG
}

void debugPrint(const __FlashStringHelper *s) {
  //  Print string to debug output.
#ifdef DEBUG
  strlcat(debugOutput, String(s).c_str(), debugOutputSize);
#endif // DEBUG
}

void debugFlush(SoftwareSerial s) {
  //  Show the contents of the debug buffer on the serial device.
#ifdef DEBUG
  s.println(debugOutput); debugOutput[0] = 0;
#endif // DEBUG
}

void preTransmission() {
  //  This will be called by ModbusMaster just before transmit.
  //  We set the transmit pin to high so that the RS485 shield will transmit.
  debugPrint(F("Set rs485_transmit=high before transmit\r\n"));
  digitalWrite(rs485_transmit, HIGH);    //  Set to high: RS485 shield waiting to transmit data
}

void postTransmission() {
  //  This will be called by ModbusMaster just after transmit.
  //  We set the transmit pin to low so that the RS485 shield will receive.
  debugPrint(F("Set rs485_transmit=low and delay before receive\r\n"));
  delayMicroseconds(660);  //  Need to wait for last char to be sent. See http://www.gammon.com.au/forum/?id=11428
  digitalWrite(rs485_transmit, LOW);    //  Set to low: RS485 shield waiting to receive data
}

#ifdef FORWARD_MODE
void forwardLoop() {
  //  Forward input from forwardSerial to RS485.  Forward output from RS485 to forwardSerial.
  if (forwardSerial.available()) {
    Serial.write(forwardSerial.read());
  }
  if (Serial.available()) {
    forwardSerial.write((uint8_t) Serial.read());
  }
}
#else

void parseType(Register reg, uint16_t data[], String &displayValue, String &transmitValue) {
  //  Parse register types for display and for sending to SIGFOX.
  displayValue = ""; transmitValue = "";
  if (false) {}
  #if NOTUSED
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
  #endif
  else if (reg.type == TYPE_S32) {
    int32_t result = ((int32_t) data[0] << 16) + data[1];
    displayValue.concat(result);
    transmitValue.concat(akeru.toHex(result));
  }
  #if NOTUSED
  else if (reg.type == TYPE_STRING_16) {
    //  Unicode string.
    const uint16_t count = reg.size;
    for (uint16_t i = 0; i < count; i++) {
      char value = (char) data[i];
      displayValue.concat(value);
      transmitValue.concat(akeru.toHex(value));
    }
  }
  else if (reg.type == TYPE_STRING_NORM) {
    //  ASCII string.
    const uint16_t count = reg.size;
    for (uint16_t i = 0; i < count; i++) {
      uint16_t value = data[i];
      char ch1 = (char) (value >> 8);
      char ch2 = (char) (value & 0xff);
      displayValue.concat(ch1); displayValue.concat(ch2);
      transmitValue.concat(akeru.toHex(ch1));
      transmitValue.concat(akeru.toHex(ch2));
    }
  }
  #endif
  else if (reg.type == TYPE_U16) {
    uint16_t result = data[0];
    displayValue.concat(result);
    transmitValue.concat(akeru.toHex(result));
  }
  #if NOTUSED
  else if (reg.type == TYPE_U16_HEX) {
    uint16_t result = data[0];
    displayValue.concat(akeru.toHex(result));
    transmitValue.concat(akeru.toHex(result));
  }
  #endif
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
  #if NOTUSED
  else if (reg.type == TYPE_U64_HEX) {
    for (uint16_t i = 0; i < 8; i++) {
      uint16_t value = data[i];
      displayValue.concat(akeru.toHex(value));
      transmitValue.concat(akeru.toHex(value));
    }
  }
  #endif
  else if (reg.type == TYPE_U8) {
    uint8_t result = (uint8_t) data[0];
    displayValue.concat(result);
    transmitValue.concat(akeru.toHex(result));
  }
  #if NOTUSED
  else if (reg.type == TYPE_U8_HEX) {
    uint8_t result = (uint8_t) data[0];
    displayValue.concat(akeru.toHex(result));
    transmitValue.concat(akeru.toHex(result));
  }
  #endif
}

//  Cache registers in pages of 16 words.  Reading more than 16 words may be inaccurate.
const uint16_t max_register_page_size = 12;  //  Number of words per page.
uint16_t register_page_data[max_register_page_size];  //  Read registers one page at a time.
uint16_t register_page_address = 0xffff;  //  Address of last page read.
uint16_t register_page_size = 0;  //  Size of last page read, in bytes.

void clearRegisterCache() {
  //  Erase the register cache.
  register_page_address = 0xffff;
  register_page_size = 0;
}

uint8_t readHoldingRegisters(uint16_t address, uint16_t size, uint16_t data[]) {
  //  Read Modbus holding registers from specified address and size (in words)
  //  to data buffer.
  debugPrint(F("readHoldingRegisters: Getting ")); debugPrint(size);
  debugPrint(F(" words at ")); debugPrint(address); debugPrint(F("\r\n"));
  if (address != 0xffff && address >= register_page_address &&
      address + size <= register_page_address + register_page_size) {
    //  If exists in the register page, reuse it.
    debugPrint(F("readHoldingRegisters: Return cache for ")); debugPrint(address);
    debugPrint(F(" size ")); debugPrint(size);
    debugPrint(F(" words\r\n"));
    debugFlush(debugSerial);
  } else {
    //  Else read in the page from RS485, which looks like:
    //    05 03 08 00 00 5d b8 00 00 00 00 2d b1
    //    Slave ID
    //       Function code
    //          Result size in bytes
    //             Result                  CRC
    //  The code below only sees the Result words.
    debugPrint(F("Reading ")); debugPrint(max_register_page_size);
    debugPrint(F(" words at ")); debugPrint(address); debugPrint(F("\r\n"));
    uint8_t result = node.readHoldingRegisters(address, max_register_page_size);

    debugFlush(debugSerial);
    if (result != node.ku8MBSuccess) {
      //  Return the error.
      debugPrint(F("Read failed: 0x")); debugPrint(String(result, HEX) + "\r\n");
      debugFlush(debugSerial);
      return result;
    }
    register_page_address = address;
    register_page_size = 0;
    String debugData = "  ";
    //  Read the entire page.
    for (;;) {
      if (register_page_size >= max_register_page_size) break;
      if (!node.available()) break;
      uint16_t w = node.receive();
      register_page_data[register_page_size++] = w;
      if (debugData != "") debugData.concat(" ");
      debugData.concat(akeru.toHex(w));
    }
    debugPrint(F("readHoldingRegisters: Received ")); debugPrint(register_page_size);
    debugPrint(F(" words:\r\n")); debugPrint(debugData + "\r\n");
    debugFlush(debugSerial);
  }
  //  Copy from our buffer to caller's buffer.
  uint16_t size2 = size;
  if (size > register_page_address + register_page_size - address)
    size2 = register_page_address + register_page_size - address;
  String debugData = "  ";
  for (uint16_t j = 0; j < size2; j++) {
    uint16_t w = register_page_data[address - register_page_address + j];
    data[j] = w;
    if (debugData != "") debugData.concat(" ");
    debugData.concat(akeru.toHex(w));
  }
  debugPrint(F("readHoldingRegisters: Returned ")); debugPrint(size2);
  debugPrint(F(" words:\r\n")); debugPrint(debugData + "\r\n");
  debugFlush(debugSerial);
  return node.ku8MBSuccess;
}

uint8_t readHoldingRegister(Register reg, uint16_t data[]) {
  //  Read Modbus holding register to data buffer.
  return readHoldingRegisters(reg.address, reg.size, data);
}

void concatHoldingRegister(String msg, Register reg, uint16_t data[]) {
  //  Decode Modbus holding register from data buffer and concat to msg
  //  if it should be transmitted to SIGFOX.
  //  Get name and unit from flash memory.
  String name = getNameByIndex(reg.name_index);
  String unit = getUnitByIndex(reg.unit_index);
  String displayValue = "", transmitValue = "";
  parseType(reg, data, displayValue, transmitValue);
  debugSerial.print(String("[ ") + reg.address + " ] ");
  debugSerial.print(name); debugSerial.print(F(" =\r\n  "));
  debugSerial.print(displayValue + " " + unit);
  debugSerial.println(F(""));
  if (reg.transmit && (msg + transmitValue).length() <= 24)
    msg = msg + transmitValue;  //  Send to SIGFOX if we should transmit.
}
#endif

//  End Sensor Declaration
////////////////////////////////////////////////////////////

void setup()
{
  ////////////////////////////////////////////////////////////
  //  Begin General Setup
  
  //  Initialize serial communication at 9600 bits per second:
  debugOutput[0] = 0;
  debugSerial.begin(9600);
  debugSerial.println(F("Started modbus-sigfox"));
#ifdef FORWARD_MODE
  forwardSerial.begin(9600);
#endif  //  FORWARD_MODE

  //  End General Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin Sensor Setup

  //  Init the transmission enable pin for the RS485 shield.
  pinMode(rs485_transmit, OUTPUT);
  //  Use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(9600);
  //  Communicate with Modbus slave over Serial (port 0).
  debugSerial.print(F("Communicating to Modbus Slave #")); debugSerial.println(slaveID);
  node.begin(slaveID, Serial);
  //  Callbacks allow us to configure the RS485 transceiver correctly (set rs485_transmit=high/low).
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  //  End Sensor Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Setup

  // Check SIGFOX Module.
  for (int i = 0; i < 5; i++) {
    //  Retry because module may be powering up.
    if (akeru.begin()) break;
    if (i == 5) debugSerial.println(F("****SIGFOX Module KO"));
    delay(1000);
  }
  debugSerial.println(F("SIGFOX Module OK"));
  //  Must not show any debug output because it will interfere with RS485.
  //akeru.echoOn(); //  Comment this line to hide debug output.

  //  End SIGFOX Module Setup
  ////////////////////////////////////////////////////////////

  //  Wait a while before starting.
  postTransmission();
  delay(1000);
}

void loop()
{
  ////////////////////////////////////////////////////////////
  //  Begin Sensor Loop

#ifdef FORWARD_MODE
  //  In Forward Mode: Forward input from forwardSerial to RS485.  Forward output from RS485 to forwardSerial.
  return forwardLoop();

#else  //  FORWARD_MODE
  //  Prepare SIGFOX message based on Modbus registers.  Must not exceed 12 characters.
  //  Based on https://github.com/4-20ma/ModbusMaster
  //  and https://www.cooking-hacks.com/documentation/tutorials/modbus-module-shield-tutorial-for-arduino-raspberry-pi-intel-galileo/  static uint32_t i;
  static uint32_t loop_count = 0;
  static uint16_t data[max_register_size];

  //  Read all registers and send selected registers to SIGFOX.
  clearRegisterCache();  //  Don't reuse the past register data.
  String msg = "";
  debugPrint("\r\n\r\n------ Loop "); debugPrint(String(loop_count++).c_str()); debugPrint("\r\n");
  for (uint16_t r = 0; r < all_registers.rows(); r++) {
    //  Read Modbus registers to data buffer.
    Register reg = {
        (boolean) all_registers[r][0], all_registers[r][1], all_registers[r][2],
        all_registers[r][3], all_registers[r][4], all_registers[r][5],
    };
    const uint8_t result = readHoldingRegister(reg, data);
    if (result == node.ku8MBSuccess)
      concatHoldingRegister(msg, reg, data);  //  Send data to SIGFOX.
    else if (reg.transmit)
      msg = msg + akeru.toHex(result);  //  Send error byte to SIGFOX.
  }
#endif  //  FORWARD_MODE

  //  End Sensor Loop
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Loop

  //  Send sensor data.
  if (akeru.sendString(msg)) {
    debugSerial.println("Message sent");
  } else {
    debugSerial.println("Message not sent");
  }

  //  End SIGFOX Module Loop
  ////////////////////////////////////////////////////////////

  //  Wait a while before looping.
  delay(1000);  //  1 second.
}

/*
Modbus Function Codes, Exception Codes

ku8MBIllegalFunction = 0x01
  Modbus protocol illegal function exception. More...
 
ku8MBIllegalDataAddress = 0x02
  Modbus protocol illegal data address exception. More...
 
ku8MBIllegalDataValue = 0x03
  Modbus protocol illegal data value exception. More...
 
ku8MBSlaveDeviceFailure = 0x04
  Modbus protocol slave device failure exception. More...
 
ku8MBSuccess = 0x00
  ModbusMaster success. More...
 
ku8MBInvalidSlaveID = 0xE0
  ModbusMaster invalid response slave ID exception. More...
 
ku8MBInvalidFunction = 0xE1
  ModbusMaster invalid response function exception. More...
 
ku8MBResponseTimedOut = 0xE2
  ModbusMaster response timed out exception. More...
 
ku8MBInvalidCRC = 0xE3
  ModbusMaster invalid response CRC exception. More...
 
Detailed Description

-- ku8MBIllegalFunction
ku8MBIllegalFunction = 0x01
Modbus protocol illegal function exception.

The function code received in the query is not an allowable action for the server (or slave). 
This may be because the function code is only applicable to newer devices, and was not 
implemented in the unit selected. It could also indicate that the server (or slave) is in the 
wrong state to process a request of this type, for example because it is unconfigured and is 
being asked to return register values.

-- ku8MBIllegalDataAddress
ku8MBIllegalDataAddress = 0x02
Modbus protocol illegal data address exception.

The data address received in the query is not an allowable address for the server (or slave). 
More specifically, the combination of reference number and transfer length is invalid. For a 
controller with 100 registers, the ADU addresses the first register as 0, and the last one as 
99. If a request is submitted with a starting register address of 96 and a quantity of registers 
of 4, then this request will successfully operate (address-wise at least) on registers 96, 97, 
98, 99. If a request is submitted with a starting register address of 96 and a quantity of 
registers of 5, then this request will fail with Exception Code 0x02 "Illegal Data Address" 
since it attempts to operate on registers 96, 97, 98, 99 and 100, and there is no register with address

-- ku8MBIllegalDataValue
ku8MBIllegalDataValue = 0x03
Modbus protocol illegal data value exception.

A value contained in the query data field is not an allowable value for server (or slave). 
This indicates a fault in the structure of the remainder of a complex request, such as that 
the implied length is incorrect. It specifically does NOT mean that a data item submitted 
for storage in a register has a value outside the expectation of the application program, 
since the MODBUS protocol is unaware of the significance of any particular value of any particular register.

-- ku8MBSlaveDeviceFailure
ku8MBSlaveDeviceFailure = 0x04
Modbus protocol slave device failure exception.

An unrecoverable error occurred while the server (or slave) was attempting to perform the 
requested action.

-- ku8MBSuccess
ku8MBSuccess = 0x00
ModbusMaster success.

Modbus transaction was successful; the following checks were valid:
slave ID
function code
response code
data
CRC

-- ku8MBInvalidSlaveID
ku8MBInvalidSlaveID = 0xE0
ModbusMaster invalid response slave ID exception.

The slave ID in the response does not match that of the request.

-- ku8MBInvalidFunction
ku8MBInvalidFunction = 0xE1
ModbusMaster invalid response function exception.

The function code in the response does not match that of the request.

-- ku8MBResponseTimedOut
ku8MBResponseTimedOut = 0xE2
ModbusMaster response timed out exception.

The entire response was not received within the timeout period, ModbusMaster::ku8MBResponseTimeout.

-- ku8MBInvalidCRC
ku8MBInvalidCRC = 0xE3
ModbusMaster invalid response CRC exception.

The CRC in the response does not match the one calculated.
 */

/*
Expected output:

Started modbus-sigfox
Communicating to Modbus Slave #5
SIGFOX Module OK
[ 4098 ] TC list :TC 1 / TC2 / TC3 / TC4 / TC5 / =
  65535, 65535, 65535, 65535 A
[ 36353 ] Current Transformer secondary : 1: 1 A  =
  5 A
[ 36354 ] Current Transformer primary =
  244 A
[ 50512 ] Hour Meter =
  1808 h / 100
[ 50514 ] Phase to Phase Voltage: U12 =
  0 V / 100
[ 50516 ] Phase to Phase Voltage: U23 =
  0 V / 100
[ 50518 ] Phase to Phase Voltage: U31 =
  0 V / 100
[ 50520 ] Simple voltage : V1 =
  24701 V / 100
[ 50522 ] Simple voltage : V2 =
  0 V / 100
[ 50524 ] Simple voltage : V3 =
  0 V / 100
[ 50526 ] Frequency : F =
  5001 Hz / 100
[ 50528 ] Current : I1 =
  0 A / 1000
[ 50530 ] Current : I2 =
  0 A / 1000
[ 50532 ] Current : I3 =
  0 A / 1000
[ 50534 ] Neutral Current : In =
  0 A / 1000
[ 50536 ] Active Power +/- : P =
  0 W / 0.1
[ 50538 ] Reactive Power +/- : Q =
  0 var / 0.1
[ 50540 ] Apparent Power : S =
  0 VA / 0.1
[ 50542 ] Power Factor : -: leading et + : laggin =
  1000 - / 1000
[ 50544 ] Active Power phase 1 +/- : P1 =
  0 W / 0.1
[ 50546 ] Active Power phase 2 +/- : P2 =
  0 W / 0.1
[ 50548 ] Active Power phase 3 +/- : P3 =
  0 W / 0.1
[ 50550 ] Reactive Power phase 1 +/- : Q1 =
  0 var / 0.1
[ 50552 ] Reactive Power phase 2 +/- : Q2 =
  0 var / 0.1
[ 50554 ] Reactive Power phase 3 +/- : Q3 =
  0 var / 0.1
[ 50556 ] Apparent Power phase 1 : S1 =
  0 VA / 0.1
[ 50558 ] Apparent Power phase 2 : S2 =
  0 VA / 0.1
[ 50560 ] Apparent Power phase 3 : S3 =
  0 VA / 0.1
[ 50562 ] Power Factor phase 1 -: leading and + : =
  1000 - / 1000
[ 50564 ] Power Factor phase 2 -: leading and + : =
  1000 - / 1000
[ 50566 ] Power Factor phase 3 -: leading and + : =
  1000 - / 1000
[ 50768 ] Hour Meter =
  1808 h / 100
[ 50780 ] Partial Positive Active Energy: Ea+ =
  0 Wh / 0.001
[ 50782 ] Partial Positive Reactive Energy: Er + =
  0 varh / 0.00
[ 51070 ] Max/avg I1 =
  0 A / 1000
[ 51072 ] Max/avg I2 =
  0 A / 1000
[ 51074 ] Max/avg I3 =
  0 A / 1000
[ 51076 ] Max/avg In =
  0 A / 1000
[ 51078 ] Max/avg P+ =
  0 W / 0.1
[ 51082 ] Max/avg Q+ =
  0 var / 0.1
[ 51086 ] Max/avg S =
  0 VA / 0.1
[ 51280 ] Hour Meter =
  18 h
[ 51281 ] Phase to Phase Voltage: U12 =
  0 V / 100
[ 51282 ] Phase to Phase Voltage: U23 =
  0 V / 100
[ 51283 ] Phase to Phase Voltage: U31 =
  0 V / 100
[ 51284 ] Simple voltage : V1 =
  24714 V / 100
[ 51285 ] Simple voltage : V2 =
  0 V / 100
[ 51286 ] Simple voltage : V3 =
  0 V / 100
[ 51287 ] Frequency : F =
  5000 Hz / 100
[ 51288 ] Current : I1 =
  0 A / 1000
[ 51289 ] Current : I2 =
  0 A / 1000
[ 51290 ] Current : I3 =
  0 A / 1000
[ 51291 ] Neutral Current : In =
  0 A / 1000
[ 51292 ] Active Power +/- : P =
   W / 0.1
[ 51293 ] Reactive Power +/- : Q =
   var / 0.1
[ 51294 ] Apparent Power : S =
  0 VA / 0.1
[ 51295 ] power factor : -: leading and + : laggi =
   - / 1000
[ 51296 ] Active Power phase 1 +/- : P1 =
   W / 0.1
[ 51297 ] Active Power phase 2 +/- : P2 =
   W / 0.1
[ 51298 ] Active Power phase 3 +/- : P3 =
   W / 0.1
[ 51299 ] Reactive Power phase 1 +/- : Q1 =
   var / 0.1
[ 51300 ] Reactive Power phase 2 +/- : Q2 =
   var / 0.1
[ 51301 ] Reactive Power phase 3 +/- : Q3 =
   var / 0.1
[ 51302 ] Apparent Power phase 1 : S1 =
  0 VA / 0.1
[ 51303 ] Apparent Power phase 2 : S2 =
  0 VA / 0.1
[ 51304 ] Apparent Power phase 3 : S3 =
  0 VA / 0.1
[ 51305 ] Power Factor phase 1 -: leading and + : =
   - / 1000
[ 51306 ] Power Factor phase 2 -: leading and + : =
   - / 1000
[ 51307 ] Power Factor phase 3 -: leading and + : =
   - / 1000
[ 51311 ] Total Positive Active Energy (no reseta =
  0 Wh / 1E-06
[ 768 ] Phase 1 Current =
  0 A / 1000
[ 770 ] Phase 2 Current =
  0 A / 1000
[ 772 ] Phase 3 Current =
  0 A / 1000
[ 774 ] Neutral Current =
  0 A / 1000
[ 776 ] Phase to Phase Voltage: U12 =
  0 V / 100
[ 778 ] Phase to Phase Voltage: U23 =
  0 V / 100
[ 780 ] Phase to Phase Voltage: U31 =
  0 V / 100
[ 782 ] Phase to Neutral voltage phase 1 =
  24701 V / 100
[ 784 ] Phase to Neutral voltage phase 2 =
  0 V / 100
[ 786 ] Phase to Neutral voltage phase 3 =
  0 V / 100
[ 788 ] Frequency =
  4999 Hz / 100
[ 790 ] Active Power +/- : P =
  0 W / 0.1
[ 792 ] Reactive Power +/- : Q =
  0 var / 0.1
[ 794 ] Apparent Power : S =
  0 VA / 0.1
[ 796 ] power factor : -: leadiing et + : laggi =
  1000 - / 1000
[ 798 ] Active Power phase1 +/- =
  0 W / 0.1
[ 800 ] Active Power phase2 +/- =
  0 W / 0.1
[ 802 ] Active Power phase3 +/- =
  0 W / 0.1
[ 804 ] Reactive Power phase1 +/- =
  0 var / 0.1
[ 806 ] Reactive Power phase2 +/- =
  0 var / 0.1
[ 808 ] Reactive Power phase3 +/- =
  0 var / 0.1
[ 810 ] Apparent Power phase1 =
  0 VA / 0.1
[ 812 ] Apparent Power phase2 =
  0 VA / 0.1
[ 814 ] Apparent Power phase3 =
  0 VA / 0.1
[ 816 ] Power factor phase 1 -:leading and +: l =
  1000 - / 1000
[ 818 ] Power factor phase 2 -:leading and +: l =
  1000 - / 1000
[ 820 ] Power factor phase 3 -:leading and +: l =
  1000 - / 1000
[ 838 ] Max/avg I1 =
  0 A / 1000
[ 840 ] Max/avg I2 =
  0 A / 1000
[ 842 ] Max/avg I3 =
  0 A / 1000
[ 844 ] max/avg active power + =
  0 W / 0.1
[ 848 ] max/avg reactive power + =
  0 var / 0.1
[ 852 ] max/avg apparent power =
  0 VA / 0.1
[ 854 ] Hour Meter =
  1808 h / 100
[ 856 ] Active Energy + =
  0 Wh / 0.001
[ 858 ] Reactive Energy + =
  0 varh / 0.00
[ 1792 ] Phase 1 Current =
  0 A / 1000
[ 1793 ] Phase 2 Current =
  0 A / 1000
[ 1794 ] Phase 3 Current =
  0 A / 1000
[ 1795 ] Neutral Current =
  0 A / 1000
[ 1796 ] Phase to Phase Voltage: U12 =
  0 V / 100
[ 1797 ] Phase to Phase Voltage: U23 =
  0 V / 100
[ 1798 ] Phase to Phase Voltage: U31 =
  0 V / 100
[ 1799 ] Phase to Neutral voltage phase 1 =
  24714 V / 100
[ 1800 ] Phase to Neutral voltage phase 2 =
  0 V / 100
[ 1801 ] Phase to Neutral voltage phase 3 =
  0 V / 100
[ 1802 ] Frequency =
  4999 Hz / 100
[ 1803 ] Active Power +/- : P =
   W / 0.1
[ 1804 ] Reactive Power +/- : Q =
   var / 0.1
[ 1805 ] Apparent Power : S =
  0 VA / 0.1
[ 1806 ] power factor : -: leadiing et + : laggi =
   - / 1000
[ 1807 ] Active Power phase1 +/- =
   W / 0.1
[ 1808 ] Active Power phase2 +/- =
   W / 0.1
[ 1809 ] Active Power phase3 +/- =
   W / 0.1
[ 1810 ] Reactive Power phase1 +/- =
   var / 0.1
[ 1811 ] Reactive Power phase2 +/- =
   var / 0.1
[ 1812 ] Reactive Power phase3 +/- =
   var / 0.1
[ 1813 ] Apparent Power phase1 =
  0 VA / 0.1
[ 1814 ] Apparent Power phase2 =
  0 VA / 0.1
[ 1815 ] Apparent Power phase3 =
  0 VA / 0.1
[ 1816 ] Power factor phase 1 -:leading and +: l =
   - / 1000
[ 1817 ] Power factor phase 2 -:leading and +: l =
   - / 1000
[ 1818 ] Power factor phase 3 -:leading and +: l =
   - / 1000
[ 1827 ] Max/avg I1 =
  0 A / 1000
[ 1828 ] Max/avg I2 =
  0 A / 1000
[ 1829 ] Max/avg I3 =
  0 A / 1000
[ 1830 ] max/avg active power + =
  0 W / 0.1
[ 1832 ] max/avg reactive power + =
  0 var / 0.1
[ 1834 ] max/avg apparent power =
  0 VA / 0.1
[ 1835 ] Active Energy +<10 000 =
  0 Wh / 0.001
[ 1836 ] Active Energy +>10 000 =
  0 Wh / 0.001
[ 1837 ] Reactive Energy+< 10 000 =
  0 varh / 0.00
[ 1838 ] Reactive Energy +>10 000 =
  0 varh / 0.00
[ 36820 ] Action Calib =
  0 A
[ 36821 ] Set Relais Impul =
  0 A
[ 36822 ] Clr Relais Impulsion =
  0 A
[ 36825 ] Coef Gnrale Courant =
  0, 0, 0, 0 A
[ 2816 ] Current : I1 =
  0 A / 1000
[ 2817 ] Current : I2 =
  0 A / 1000
[ 2818 ] Current : I3 =
  0 A / 1000
[ 2819 ] Neutral Current : In =
  0 A / 1000
[ 2820 ] Phase to Phase Voltage: U12 =
  0 V / 100
[ 2821 ] Phase to Phase Voltage: U23 =
  0 V / 100
[ 2822 ] Phase to Phase Voltage: U31 =
  0 V / 100
[ 2823 ] Simple voltage : V1 =
  24700 V / 100
[ 2824 ] Simple voltage : V2 =
  0 V / 100
[ 2825 ] Simple voltage : V3 =
  0 V / 100
[ 2826 ] Frequency : Fr =
  4999 Hz / 100
[ 2827 ] Total Active Power +/- : P =
   W / 0.1
[ 2828 ] Total Reactive Power +/- : Q =
   var / 0.1
[ 2829 ] Total apparent power : S =
  0 VA / 0.1
[ 2830 ] Power factor : -: capacitif et + : indu =
   - / 1000
[ 2831 ] I 1 AVG MAX =
  0 A / 1000
[ 2832 ] I 2 AVG MAX =
  0 A / 1000
[ 2833 ] I 3 AVG MAX =
  0 A / 1000
[ 2834 ] In AVG MAX =
  0 A / 1000
[ 2835 ] P+ AVG MAX =
  0 W / 0.1
[ 2836 ] Active Energy +<10 000 =
  0 Wh / 0.001
[ 2837 ] Active Energy +>10 000 =
  0 Wh / 0.001
[ 2838 ] Reactive Energy+< 10 000 =
  0 varh / 0.00
[ 2839 ] Reactive Energy +>10 000 =
  0 varh / 0.00
[ 2840 ] Hour Meter +<10000 =
  1808 h / 100
[ 2841 ] Hour Meter +>10000 =
  0 h / 100
[ 2842 ] Active Power phase1 +/- =
   W / 0.1
[ 2843 ] Active Power phase2 +/- =
   W / 0.1
[ 2844 ] Active Power phase3 +/- =
   W / 0.1
[ 2845 ] Reactive Power phase1 +/- =
   var / 0.1
[ 2846 ] Reactive Power phase2 +/- =
   var / 0.1
[ 2847 ] Reactive Power phase3 +/- =
   var / 0.1
[ 2848 ] Apparent Power phase1 =
  0 VA / 0.1
[ 513 ] Current Transformer secondary : 1: 1 A  =
  5 A
[ 514 ] Current Transformer primary =
  244 A
Message sent
[ 4098 ] TC list :TC 1 / TC2 / TC3 / TC4 / TC5 / =
  65535, 65535, 65535, 65535 A
[ 36353 ] Current Transformer secondary : 1: 1 A  =
  5 A
[ 36354 ] Current Transformer primary =
  244 A
[ 50512 ] Hour Meter =
  1808 h / 100
[ 50514 ] Phase to Phase Voltage: U12 =
  0 V / 100
[ 50516 ] Phase to Phase Voltage: U23 =
  0 V / 100
[ 50518 ] Phase to Phase Voltage: U31 =
  0 V / 100
[ 50520 ] Simple voltage : V1 =
  24678 V / 100
[ 50522 ] Simple voltage : V2 =
  0 V / 100
[ 50524 ] Simple voltage : V3 =
  0 V / 100
[ 50526 ] Frequency : F =
  4998 Hz / 100
[ 50528 ] Current : I1 =
  0 A / 1000
[ 50530 ] Current : I2 =
  0 A / 1000
[ 50532 ] Current : I3 =
  0 A / 1000
[ 50534 ] Neutral Current : In =
  0 A / 1000
[ 50536 ] Active Power +/- : P =
  0 W / 0.1
[ 50538 ] Reactive Power +/- : Q =
  0 var / 0.1
[ 50540 ] Apparent Power : S =
  0 VA / 0.1
[ 50542 ] Power Factor : -: leading et + : laggin =
  1000 - / 1000
[ 50544 ] Active Power phase 1 +/- : P1 =
  0 W / 0.1
[ 50546 ] Active Power phase 2 +/- : P2 =
  0 W / 0.1
[ 50548 ] Active Power phase 3 +/- : P3 =
  0 W / 0.1
[ 50550 ] Reactive Power phase 1 +/- : Q1 =
  0 var / 0.1
[ 50552 ] Reactive Power phase 2 +/- : Q2 =
  0 var / 0.1
[ 50554 ] Reactive Power phase 3 +/- : Q3 =
  0 var / 0.1
[ 50556 ] Apparent Power phase 1 : S1 =
  0 VA / 0.1
[ 50558 ] Apparent Power phase 2 : S2 =
  0 VA / 0.1
[ 50560 ] Apparent Power phase 3 : S3 =
  0 VA / 0.1
[ 50562 ] Power Factor phase 1 -: leading and + : =
  1000 - / 1000
[ 50564 ] Power Factor phase 2 -: leading and + : =
  1000 - / 1000
[ 50566 ] Power Factor phase 3 -: leading and + : =
  1000 - / 1000
[ 50768 ] Hour Meter =
  1808 h / 100
[ 50780 ] Partial Positive Active Energy: Ea+ =
  0 Wh / 0.001
[ 50782 ] Partial Positive Reactive Energy: Er + =
  0 varh / 0.00
[ 51070 ] Max/avg I1 =
  0 A / 1000
[ 51072 ] Max/avg I2 =
  0 A / 1000
[ 51074 ] Max/avg I3 =
  0 A / 1000
[ 51076 ] Max/avg In =
  0 A / 1000
[ 51078 ] Max/avg P+ =
  0 W / 0.1
[ 51082 ] Max/avg Q+ =
  0 var / 0.1
[ 51086 ] Max/avg S =
  0 VA / 0.1
[ 51280 ] Hour Meter =
  18 h
[ 51281 ] Phase to Phase Voltage: U12 =
  0 V / 100
[ 51282 ] Phase to Phase Voltage: U23 =
  0 V / 100
[ 51283 ] Phase to Phase Voltage: U31 =
  0 V / 100
[ 51284 ] Simple voltage : V1 =
  24665 V / 100
[ 51285 ] Simple voltage : V2 =
  0 V / 100
[ 51286 ] Simple voltage : V3 =
  0 V / 100
[ 51287 ] Frequency : F =
  4999 Hz / 100
[ 51288 ] Current : I1 =
  0 A / 1000
[ 51289 ] Current : I2 =
  0 A / 1000
[ 51290 ] Current : I3 =
  0 A / 1000
[ 51291 ] Neutral Current : In =
  0 A / 1000
[ 51292 ] Active Power +/- : P =
   W / 0.1
[ 51293 ] Reactive Power +/- : Q =
   var / 0.1
[ 51294 ] Apparent Power : S =
  0 VA / 0.1
[ 51295 ] power factor : -: leading and + : laggi =
   - / 1000
[ 51296 ] Active Power phase 1 +/- : P1 =
   W / 0.1
[ 51297 ] Active Power phase 2 +/- : P2 =
   W / 0.1
[ 51298 ] Active Power phase 3 +/- : P3 =
   W / 0.1
[ 51299 ] Reactive Power phase 1 +/- : Q1 =
   var / 0.1
[ 51300 ] Reactive Power phase 2 +/- : Q2 =
   var / 0.1
[ 51301 ] Reactive Power phase 3 +/- : Q3 =
   var / 0.1
[ 51302 ] Apparent Power phase 1 : S1 =
  0 VA / 0.1
[ 51303 ] Apparent Power phase 2 : S2 =
  0 VA / 0.1
[ 51304 ] Apparent Power phase 3 : S3 =
  0 VA / 0.1
[ 51305 ] Power Factor phase 1 -: leading and + : =
   - / 1000
[ 51306 ] Power Factor phase 2 -: leading and + : =
   - / 1000
[ 51307 ] Power Factor phase 3 -: leading and + : =
   - / 1000
[ 51311 ] Total Positive Active Energy (no reseta =
  0 Wh / 1E-06
[ 768 ] Phase 1 Current =
  0 A / 1000
[ 770 ] Phase 2 Current =
  0 A / 1000
[ 772 ] Phase 3 Current =
  0 A / 1000
[ 774 ] Neutral Current =
  0 A / 1000
[ 776 ] Phase to Phase Voltage: U12 =
  0 V / 100
[ 778 ] Phase to Phase Voltage: U23 =
  0 V / 100
[ 780 ] Phase to Phase Voltage: U31 =
  0 V / 100
[ 782 ] Phase to Neutral voltage phase 1 =
  24664 V / 100
[ 784 ] Phase to Neutral voltage phase 2 =
  0 V / 100
[ 786 ] Phase to Neutral voltage phase 3 =
  0 V / 100
[ 788 ] Frequency =
  4998 Hz / 100
[ 790 ] Active Power +/- : P =
  0 W / 0.1
[ 792 ] Reactive Power +/- : Q =
  0 var / 0.1
[ 794 ] Apparent Power : S =
  0 VA / 0.1
[ 796 ] power factor : -: leadiing et + : laggi =
  1000 - / 1000
[ 798 ] Active Power phase1 +/- =
  0 W / 0.1
[ 800 ] Active Power phase2 +/- =
  0 W / 0.1
[ 802 ] Active Power phase3 +/- =
  0 W / 0.1
[ 804 ] Reactive Power phase1 +/- =
  0 var / 0.1
[ 806 ] Reactive Power phase2 +/- =
  0 var / 0.1
[ 808 ] Reactive Power phase3 +/- =
  0 var / 0.1
[ 810 ] Apparent Power phase1 =
  0 VA / 0.1
[ 812 ] Apparent Power phase2 =
  0 VA / 0.1
[ 814 ] Apparent Power phase3 =
  0 VA / 0.1
[ 816 ] Power factor phase 1 -:leading and +: l =
  1000 - / 1000
[ 818 ] Power factor phase 2 -:leading and +: l =
  1000 - / 1000
[ 820 ] Power factor phase 3 -:leading and +: l =
  1000 - / 1000
[ 838 ] Max/avg I1 =
  0 A / 1000
[ 840 ] Max/avg I2 =
  0 A / 1000
[ 842 ] Max/avg I3 =
  0 A / 1000
[ 844 ] max/avg active power + =
  0 W / 0.1
[ 848 ] max/avg reactive power + =
  0 var / 0.1
[ 852 ] max/avg apparent power =
  0 VA / 0.1
[ 854 ] Hour Meter =
  1808 h / 100
[ 856 ] Active Energy + =
  0 Wh / 0.001
[ 858 ] Reactive Energy + =
  0 varh / 0.00
[ 1792 ] Phase 1 Current =
  0 A / 1000
[ 1793 ] Phase 2 Current =
  0 A / 1000
[ 1794 ] Phase 3 Current =
  0 A / 1000
[ 1795 ] Neutral Current =
  0 A / 1000
[ 1796 ] Phase to Phase Voltage: U12 =
  0 V / 100
[ 1797 ] Phase to Phase Voltage: U23 =
  0 V / 100
[ 1798 ] Phase to Phase Voltage: U31 =
  0 V / 100
[ 1799 ] Phase to Neutral voltage phase 1 =
  24658 V / 100
[ 1800 ] Phase to Neutral voltage phase 2 =
  0 V / 100
[ 1801 ] Phase to Neutral voltage phase 3 =
  0 V / 100
[ 1802 ] Frequency =
  4998 Hz / 100
[ 1803 ] Active Power +/- : P =
   W / 0.1
[ 1804 ] Reactive Power +/- : Q =
   var / 0.1
[ 1805 ] Apparent Power : S =
  0 VA / 0.1
[ 1806 ] power factor : -: leadiing et + : laggi =
   - / 1000
[ 1807 ] Active Power phase1 +/- =
   W / 0.1
[ 1808 ] Active Power phase2 +/- =
   W / 0.1
[ 1809 ] Active Power phase3 +/- =
   W / 0.1
[ 1810 ] Reactive Power phase1 +/- =
   var / 0.1
[ 1811 ] Reactive Power phase2 +/- =
   var / 0.1
[ 1812 ] Reactive Power phase3 +/- =
   var / 0.1
[ 1813 ] Apparent Power phase1 =
  0 VA / 0.1
[ 1814 ] Apparent Power phase2 =
  0 VA / 0.1
[ 1815 ] Apparent Power phase3 =
  0 VA / 0.1
[ 1816 ] Power factor phase 1 -:leading and +: l =
   - / 1000
[ 1817 ] Power factor phase 2 -:leading and +: l =
   - / 1000
[ 1818 ] Power factor phase 3 -:leading and +: l =
   - / 1000
[ 1827 ] Max/avg I1 =
  0 A / 1000
[ 1828 ] Max/avg I2 =
  0 A / 1000
[ 1829 ] Max/avg I3 =
  0 A / 1000
[ 1830 ] max/avg active power + =
  0 W / 0.1
[ 1832 ] max/avg reactive power + =
  0 var / 0.1
[ 1834 ] max/avg apparent power =
  0 VA / 0.1
[ 1835 ] Active Energy +<10 000 =
  0 Wh / 0.001
[ 1836 ] Active Energy +>10 000 =
  0 Wh / 0.001
[ 1837 ] Reactive Energy+< 10 000 =
  0 varh / 0.00
[ 1838 ] Reactive Energy +>10 000 =
  0 varh / 0.00
[ 36820 ] Action Calib =
  0 A
[ 36821 ] Set Relais Impul =
  0 A
[ 36822 ] Clr Relais Impulsion =
  0 A
[ 36825 ] Coef Gnrale Courant =
  0, 0, 0, 0 A
[ 2816 ] Current : I1 =
  0 A / 1000
[ 2817 ] Current : I2 =
  0 A / 1000
[ 2818 ] Current : I3 =
  0 A / 1000
[ 2819 ] Neutral Current : In =
  0 A / 1000
[ 2820 ] Phase to Phase Voltage: U12 =
  0 V / 100
[ 2821 ] Phase to Phase Voltage: U23 =
  0 V / 100
[ 2822 ] Phase to Phase Voltage: U31 =
  0 V / 100
[ 2823 ] Simple voltage : V1 =
  24660 V / 100
[ 2824 ] Simple voltage : V2 =
  0 V / 100
[ 2825 ] Simple voltage : V3 =
  0 V / 100
[ 2826 ] Frequency : Fr =
  4999 Hz / 100
[ 2827 ] Total Active Power +/- : P =
   W / 0.1
[ 2828 ] Total Reactive Power +/- : Q =
   var / 0.1
[ 2829 ] Total apparent power : S =
  0 VA / 0.1
[ 2830 ] Power factor : -: capacitif et + : indu =
   - / 1000
[ 2831 ] I 1 AVG MAX =
  0 A / 1000
[ 2832 ] I 2 AVG MAX =
  0 A / 1000
[ 2833 ] I 3 AVG MAX =
  0 A / 1000
[ 2834 ] In AVG MAX =
  0 A / 1000
[ 2835 ] P+ AVG MAX =
  0 W / 0.1
[ 2836 ] Active Energy +<10 000 =
  0 Wh / 0.001
[ 2837 ] Active Energy +>10 000 =
  0 Wh / 0.001
[ 2838 ] Reactive Energy+< 10 000 =
  0 varh / 0.00
[ 2839 ] Reactive Energy +>10 000 =
  0 varh / 0.00
[ 2840 ] Hour Meter +<10000 =
  1809 h / 100
[ 2841 ] Hour Meter +>10000 =
  0 h / 100
[ 2842 ] Active Power phase1 +/- =
   W / 0.1
[ 2843 ] Active Power phase2 +/- =
   W / 0.1
[ 2844 ] Active Power phase3 +/- =
   W / 0.1
[ 2845 ] Reactive Power phase1 +/- =
   var / 0.1
[ 2846 ] Reactive Power phase2 +/- =
   var / 0.1
[ 2847 ] Reactive Power phase3 +/- =
   var / 0.1
[ 2848 ] Apparent Power phase1 =
  0 VA / 0.1
[ 513 ] Current Transformer secondary : 1: 1 A  =
  5 A
[ 514 ] Current Transformer primary =
  244 A
M

*/

