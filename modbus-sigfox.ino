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

#include <avr/pgmspace.h>
#include <String.h>
#include <SoftwareSerial.h>
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

//  Use D2 port as flow control for RS485 shield.
//  Set to high for the sending state, set to low for receiving state
const int rs485_transmit = 2;
const int slaveID = 5;  //  Default Slave ID of the Modbus device.
ModbusMaster node;  //  Instantiate ModbusMaster object
String debugOutput = "";  //  Debug output buffer.

void preTransmission() {
  debugOutput.concat(F("Set rs485_transmit=high before transmit\r\n"));
  digitalWrite(rs485_transmit, HIGH);    //  Set to high: RS485 shield waiting to transmit data
}

void postTransmission() {
  debugOutput.concat(F("Set rs485_transmit=low and delay before receive\r\n"));
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

//  Cache registers in pages of 24 bytes.  Reading more than 24 bytes may be inaccurate.
const uint16_t max_register_page_size = 24;  //  Read registers in pages of 24 bytes.
uint8_t register_page_data[max_register_page_size];  //  Read registers one page at a time.
uint16_t register_page_address = 0xffff;  //  Address of last page read.
uint16_t register_page_size = 0;  //  Size of last page read, in bytes.

uint8_t readHoldingRegisters(uint16_t address, uint16_t size, uint16_t data[]) {
  //  Read Modbus holding registers from specified address and size (in words)
  //  to data buffer.
  debugOutput.concat("Getting "); debugOutput.concat(size);
  debugOutput.concat(" words at "); debugOutput.concat(address); debugOutput.concat("\r\n");
  if (address != 0xffff && address >= register_page_address &&
      address + size <= register_page_address + register_page_size) {
    //  If exists in the register page, reuse it.
    debugOutput.concat("Return cache for "); debugOutput.concat(address);
    debugOutput.concat(" size "); debugOutput.concat(size);
    debugOutput.concat(" words\r\n");
  } else {
    //  Else read in the page.
    debugOutput.concat("Reading "); debugOutput.concat(max_register_page_size);
    debugOutput.concat(" words at "); debugOutput.concat(address); debugOutput.concat("\r\n");
    uint8_t result = node.readHoldingRegisters(address, max_register_page_size);
    debugSerial.println(debugOutput); debugOutput = "";
    if (result != node.ku8MBSuccess) {
      //  Return the error.
      debugSerial.print("Read failed: 0x");
      debugSerial.println(result, HEX);
      return result;
    }
    //  Sample response:
    //    05 03 08 00 00 5d b8 00 00 00 00 2d b1
    //    Slave ID
    //       Functon code
    //          Result size in bytes
    //             Result                  CRC
    register_page_address = address;
    register_page_size = node.getResponseBuffer(0);
    debugSerial.print("Received "); debugSerial.print(register_page_size);
    debugSerial.println(" bytes");
    for (uint16_t j = 0; j < register_page_size; j++)
      register_page_data[j] = node.getResponseBuffer(j + 1);
  }
  //  Copy from our buffer to caller's buffer.
  uint16_t size2 = size;
  if (size > register_page_address + register_page_size - address)
    size2 = register_page_address + register_page_size - address;
  for (uint16_t j = 0; j < size2; j++)
    data[j] = register_page_data[address - register_page_address + j];
  return node.ku8MBSuccess;
}

uint8_t readHoldingRegister(Register reg, uint16_t data[]) {
  //  Read Modbus holding register to data buffer.
  return readHoldingRegisters(reg.address, reg.size, data);
}

void concatHoldingRegister(String msg, Register reg, uint16_t data[]) {
  //  Decode Modbus holding register from data buffer and concat to msg
  //  if it should be transmitted to SIGFOX.
  String displayValue = "", transmitValue = "";
  parseType(reg, data, displayValue, transmitValue);
  debugSerial.print(reg.name); debugSerial.print(" = ");
  debugSerial.print(displayValue); debugSerial.print(reg.unit);
  debugSerial.println("");
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
  debugSerial.print("Communicating to Modbus Slave #"); debugSerial.println(slaveID);
  node.begin(slaveID, Serial);
  //  Callbacks allow us to configure the RS485 transceiver correctly (set rs485_transmit=high/low).
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  //  End Sensor Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Setup

  // Check SIGFOX Module.
  if (!akeru.begin()) {
    debugSerial.println("****TD1208 KO");
    ////while(1);
  }
  //  Must not show any debug output because it will interfere with RS485.
  //akeru.echoOn(); //  Comment this line to hide debug output.

  //  End SIGFOX Module Setup
  ////////////////////////////////////////////////////////////

  //  Wait a while before starting.
  postTransmission();
  delay(5000);
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
  String msg = "";
  debugOutput.concat("[ "); debugOutput.concat(loop_count++); debugOutput.concat(" ] ");
  for (uint16_t r = 0; r < all_registers_count; r++) {
    //  Read Modbus registers to data buffer.
    const Register reg = all_registers[r];
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

static const uint8_t   ModbusMaster::ku8MBIllegalFunction = 0x01
  Modbus protocol illegal function exception. More...
 
static const uint8_t  ModbusMaster::ku8MBIllegalDataAddress = 0x02
  Modbus protocol illegal data address exception. More...
 
static const uint8_t  ModbusMaster::ku8MBIllegalDataValue = 0x03
  Modbus protocol illegal data value exception. More...
 
static const uint8_t  ModbusMaster::ku8MBSlaveDeviceFailure = 0x04
  Modbus protocol slave device failure exception. More...
 
static const uint8_t  ModbusMaster::ku8MBSuccess = 0x00
  ModbusMaster success. More...
 
static const uint8_t  ModbusMaster::ku8MBInvalidSlaveID = 0xE0
  ModbusMaster invalid response slave ID exception. More...
 
static const uint8_t  ModbusMaster::ku8MBInvalidFunction = 0xE1
  ModbusMaster invalid response function exception. More...
 
static const uint8_t  ModbusMaster::ku8MBResponseTimedOut = 0xE2
  ModbusMaster response timed out exception. More...
 
static const uint8_t  ModbusMaster::ku8MBInvalidCRC = 0xE3
  ModbusMaster invalid response CRC exception. More...
 
Detailed Description

§ ku8MBIllegalFunction
const uint8_t ModbusMaster::ku8MBIllegalFunction = 0x01
static
Modbus protocol illegal function exception.

The function code received in the query is not an allowable action for the server (or slave). 
This may be because the function code is only applicable to newer devices, and was not 
implemented in the unit selected. It could also indicate that the server (or slave) is in the 
wrong state to process a request of this type, for example because it is unconfigured and is 
being asked to return register values.

§ ku8MBIllegalDataAddress
const uint8_t ModbusMaster::ku8MBIllegalDataAddress = 0x02
static
Modbus protocol illegal data address exception.

The data address received in the query is not an allowable address for the server (or slave). 
More specifically, the combination of reference number and transfer length is invalid. For a 
controller with 100 registers, the ADU addresses the first register as 0, and the last one as 
99. If a request is submitted with a starting register address of 96 and a quantity of registers 
of 4, then this request will successfully operate (address-wise at least) on registers 96, 97, 
98, 99. If a request is submitted with a starting register address of 96 and a quantity of 
registers of 5, then this request will fail with Exception Code 0x02 "Illegal Data Address" 
since it attempts to operate on registers 96, 97, 98, 99 and 100, and there is no register with address

Examples:
examples/PhoenixContact_nanoLC/PhoenixContact_nanoLC.pde.
§ ku8MBIllegalDataValue
const uint8_t ModbusMaster::ku8MBIllegalDataValue = 0x03
static
Modbus protocol illegal data value exception.

A value contained in the query data field is not an allowable value for server (or slave). 
This indicates a fault in the structure of the remainder of a complex request, such as that 
the implied length is incorrect. It specifically does NOT mean that a data item submitted 
for storage in a register has a value outside the expectation of the application program, 
since the MODBUS protocol is unaware of the significance of any particular value of any particular register.

§ ku8MBSlaveDeviceFailure
const uint8_t ModbusMaster::ku8MBSlaveDeviceFailure = 0x04
static
Modbus protocol slave device failure exception.

An unrecoverable error occurred while the server (or slave) was attempting to perform the 
requested action.

§ ku8MBSuccess
const uint8_t ModbusMaster::ku8MBSuccess = 0x00
static
ModbusMaster success.

Modbus transaction was successful; the following checks were valid:

slave ID
function code
response code
data
CRC
Examples:
examples/Basic/Basic.pde, and examples/RS485_HalfDuplex/RS485_HalfDuplex.ino.

§ ku8MBInvalidSlaveID
const uint8_t ModbusMaster::ku8MBInvalidSlaveID = 0xE0
static
ModbusMaster invalid response slave ID exception.

The slave ID in the response does not match that of the request.

§ ku8MBInvalidFunction
const uint8_t ModbusMaster::ku8MBInvalidFunction = 0xE1
static
ModbusMaster invalid response function exception.

The function code in the response does not match that of the request.

§ ku8MBResponseTimedOut
const uint8_t ModbusMaster::ku8MBResponseTimedOut = 0xE2
static
ModbusMaster response timed out exception.

The entire response was not received within the timeout period, ModbusMaster::ku8MBResponseTimeout.

§ ku8MBInvalidCRC
const uint8_t ModbusMaster::ku8MBInvalidCRC = 0xE3
static
ModbusMaster invalid response CRC exception.

The CRC in the response does not match the one calculated.
 */

/*
Expected output:

Demo sketch for Akeru library :)
Communicating to Modbus Slave #5
****TD1208 KO
Set EN=low and delay before receive
[ 0 ] Reading 4 words at 50520
Set EN=high before transmit
Send: 05 03 c5 58 00 04 f8 92
Set EN=low and delay before receive
Receive: 05 03 08 00 00 5d b8 00 00 00 00 2d b1
Read OK
simple_voltage_v1=0
Reading 2 words at 50526
Set EN=high before transmit
Send: 05 03 c5 5e 00 02 98 91
Set EN=low and delay before receive
Receive: 05 03 04 00 00 13 85 73 60
Read OK
frequency_f=0
Message not sent

*/

