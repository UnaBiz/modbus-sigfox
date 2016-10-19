//  Connect to a Modbus device via RS485 and send Modbus data to the SIGFOX cloud.
//  Needs a UnaBiz SIGFOX adapter, DFRobot RS485 shield and FTDI 5V UART-to-USB adapter (e.g. uUSB-PA5)
//  https://www.dfrobot.com/index.php?route=product/product&product_id=1024
//  https://www.dfrobot.com/wiki/index.php/RS485_Shield
//  http://www.4dsystems.com.au/product/uUSB_PA5/

//  Need a debug serial port, because the USB port UART is used by RS485.
//  To download the sketch, turn the RS485 shield off using the ON-OFF switch on the shield.
//  Then restart the board and switch the RS485 shield on.

#include <SoftwareSerial.h>
#include "unabiz-arduino-master/Akeru.h"
#include "ModbusMaster/src/ModbusMaster.h"

////////////////////////////////////////////////////////////
//  Begin Sensor Declaration

//  Need a debug serial port, because the USB port UART is used by RS485.
//  Connect an FTDI 5V UART-to-USB adapter (e.g. uUSB-PA5) to Arduino D6, D7 and GND.
//  FTDI TX -> Arduino D6, FTDI RX -> Arduino D7, FTDI GND -> Arduino GND. 
//  Don't use D2 because it's used by RS485 shield in manual transmission mode.
SoftwareSerial debugSerial(6, 7); // RX, TX

//  Parameters for DIRIS A20 based on:
//  http://www.socomec.com/range-single-circuit-multifunction-meters_en.html?product=/diris-a20_en.html&view=documentation
//  http://www.socomec.com/files/live/sites/systemsite/files/SCP/6_gestion_energie/diris/diris_a20/535746a_A20.pdf
//  http://www.socomec.com/files/live/sites/systemsite/files/SCP/6_gestion_energie/diris/diris_a20/PRO_880103_DirisA20.html
//  See diris-a20.html, diris-a20.png, diris-a20.pdf.

const int slaveID = 5;  //  Default Slave ID of the Modbus device.
const uint16_t measurement_common_table_start = 50512;  //  In 16-bit words.
////const uint16_t measurement_common_table_start = 768;  //  In 16-bit words.

////const uint16_t measurement_common_table_size = 62;  //  In 16-bit words.
const uint16_t measurement_common_table_size = 2;  //  In 16-bit words.

const uint16_t simple_voltage_v1 = 50520; //  50520: Simple voltage : V1 (V / 100, U32)
const uint16_t frequency_f = 50526; //  50526: Frequency : F (Hz / 100, U32)

//  RS485 shield enable terminal (D2 port)
//  Set to high for the sending state, set to low for receiving state
const int EN = 2;  

//  Instantiate ModbusMaster object
ModbusMaster node;

void preTransmission()
{
  debugSerial.println("Set EN=high before transmit");
  digitalWrite(EN, HIGH);    //  Set EN=high, RS485 shield waiting to transmit data
}
void postTransmission()
{
  debugSerial.println("Set EN=low before receive");
  digitalWrite(EN, LOW);    //  Set EN=low, RS485 shield waiting to receive data
}

//  End Sensor Declaration
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//  Begin SIGFOX Module Declaration

// TD1208 Sigfox module IO definition
#define TX 5  //  For UnaBiz / Akene
#define RX 4  //  For UnaBiz / Akene

// Sigfox instance management 
Akeru akeru(RX, TX);

//  End SIGFOX Module Declaration
////////////////////////////////////////////////////////////

void setup()
{
  ////////////////////////////////////////////////////////////
  //  Begin General Setup
  
  //  Initialize serial communication at 9600 bits per second:
  debugSerial.begin(9600);
  debugSerial.println("Demo sketch for Akeru library :)");

  //  End General Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin Sensor Setup

  //  Init the transmission enable pin for the RS485 shield.
  pinMode(EN, OUTPUT);
  //  Use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(9600);
  //  Communicate with Modbus slave over Serial (port 0).
  debugSerial.print("Communicating to Modbus Slave #"); debugSerial.println(slaveID);
  node.begin(slaveID, Serial);
  //  Callbacks allow us to configure the RS485 transceiver correctly (set EN=high/low).
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  //  End Sensor Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Setup

  // Check SIGFOX Module.
  if (!akeru.begin())
  {
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

  //  Prepare sensor data.  Must not exceed 12 characters.
  String msg = "t:0,h:0";

  //  Based on https://github.com/4-20ma/ModbusMaster
  //  and https://www.cooking-hacks.com/documentation/tutorials/modbus-module-shield-tutorial-for-arduino-raspberry-pi-intel-galileo/  static uint32_t i;
  static uint32_t loop_count = 0;
  uint8_t j, result;
  int data_size = measurement_common_table_size;
  uint16_t data[data_size];
  loop_count++;

  //  Read Measurement Common table to RX buffer.
  debugSerial.print("[ "); debugSerial.print(loop_count); debugSerial.print(" ] "); 
  debugSerial.print("Reading "); debugSerial.print(measurement_common_table_size);
  debugSerial.print(" bytes at "); debugSerial.println(measurement_common_table_start);
  result = node.readHoldingRegisters(measurement_common_table_start, measurement_common_table_size);

  //  Do something with data if read is successful
  if (result == node.ku8MBSuccess)
  {
    debugSerial.println("Read OK");
    for (j = 0; j < measurement_common_table_size; j++)
      data[j] = node.getResponseBuffer(j);
    msg = ""; uint16_t value;

    //  Send selected parameters.
    ////value = data[simple_voltage_v1 - measurement_common_table_start];
    value = data[0];
    msg = msg + akeru.toHex(value);
    debugSerial.print("simple_voltage_v1="); debugSerial.println(value);

    ////value = data[frequency_f - measurement_common_table_start];
    value = data[0];
    msg = msg + akeru.toHex(value);
    debugSerial.print("frequency_f="); debugSerial.println(value);
  }
  else
  {
    debugSerial.print("Read failed: 0x");
    debugSerial.println(result, HEX);
    msg = akeru.toHex(result);
  }
  //  End Sensor Loop
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Loop

  //  Send sensor data.
  if (akeru.sendString(msg))
  {
    debugSerial.println("Message sent");
  }
  else
  {
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

// set word 0 of TX buffer to least-significant word of counter (bits 15..0)
//node.setTransmitBuffer(0, lowWord(i));
// set word 1 of TX buffer to most-significant word of counter (bits 31..16)
//node.setTransmitBuffer(1, highWord(i));
// slave: write TX buffer to (2) 16-bit registers starting at register 0
//result = node.writeMultipleRegisters(0, 2);

/*
Expected output:
*/

