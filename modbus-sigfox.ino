//  Connect to a Modbus device via RS485 and send Modbus data to the SIGFOX cloud.
//  Needs a UnaBiz SIGFOX adapter, DFRobot RS485 shield and FTDI 5V UART-to-USB adapter (e.g. uUSB-PA5)
//  https://www.dfrobot.com/index.php?route=product/product&product_id=1024
//  https://www.dfrobot.com/wiki/index.php/RS485_Shield
//  http://www.4dsystems.com.au/product/uUSB_PA5/

//  Need a debug serial port, because the USB port UART is used by RS485.
//  To download the sketch, turn the RS485 shield off using the ON-OFF switch on the shield.
//  Then restart the board and switch the RS485 shield on.

#include <SoftwareSerial.h>
#include <Akeru.h>
#include <ModbusMaster.h>

////////////////////////////////////////////////////////////
//  Begin Sensor Declaration

//  Need a debug serial port, because the USB port UART is used by RS485.
//  Connect an FTDI 5V UART-to-USB adapter (e.g. uUSB-PA5) to Arduino D2, D3 and GND.
//  FTDI TX -> Arduino D2, FTDI RX -> Arduino D3, FTDI GND -> Arduino GND. 
SoftwareSerial debugSerial(2, 3); // RX, TX

//  Parameters for DIRIS A20 based on:
//  http://www.socomec.com/range-single-circuit-multifunction-meters_en.html?product=/diris-a20_en.html&view=documentation
//  http://www.socomec.com/files/live/sites/systemsite/files/SCP/6_gestion_energie/diris/diris_a20/535746a_A20.pdf
//  http://www.socomec.com/files/live/sites/systemsite/files/SCP/6_gestion_energie/diris/diris_a20/PRO_880103_DirisA20.html
//  See diris-a20.html, diris-a20.png, diris-a20.pdf.

const int slaveID = 5;  //  Default Slave ID of the Modbus device.
const int measurement_common_table_start = 50512;  //  In 16-bit words.
const int measurement_common_table_size = 62;  //  In 16-bit words.

const int simple_voltage_v1 = 50520; //  50520: Simple voltage : V1 (V / 100, U32)
const int frequency_f = 50526; //  50526: Frequency : F (Hz / 100, U32)

//  Instantiate ModbusMaster object
ModbusMaster node;

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

  //  Use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(9600);
  //  Communicate with Modbus slave over Serial (port 0).
  node.begin(slaveID, Serial);
  
  //  End Sensor Setup
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  //  Begin SIGFOX Module Setup

  // Check SIGFOX Module.
  if (!akeru.begin())
  {
    debugSerial.println("TD1208 KO");
    while(1);
  }

  //  Must not show any debug output because it will interfere with RS485.
  //akeru.echoOn(); //  Comment this line to hide debug output.

  //  End SIGFOX Module Setup
  ////////////////////////////////////////////////////////////
}

void loop()
{
  ////////////////////////////////////////////////////////////
  //  Begin Sensor Loop

  //  Prepare sensor data.  Must not exceed 12 characters.
  String msg = "t:0,h:0";

  //  Based on https://github.com/4-20ma/ModbusMaster
  //  and https://www.cooking-hacks.com/documentation/tutorials/modbus-module-shield-tutorial-for-arduino-raspberry-pi-intel-galileo/  static uint32_t i;
  static uint32_t i;
  uint8_t j, result;
  int data_size = measurement_common_table_size;
  uint16_t data[data_size];
  i++;

  //  Read Measurement Common table to RX buffer.
  debugSerial.print("Reading "); debugSerial.print(measurement_common_table_size);
  debugSerial.print(" bytes at "); debugSerial.println(measurement_common_table_start);
  result = node.readHoldingRegisters(measurement_common_table_start, measurement_common_table_size);

  //  Do something with data if read is successful
  if (result == node.ku8MBSuccess)
  {
    debugSerial.println("Read OK");
    for (j = 0; j < measurement_common_table_size; j++)
      data[j] = node.getResponseBuffer(j);

    //  Print selected parameters.
    debugSerial.print("simple_voltage_v1=");
    debugSerial.println(data[simple_voltage_v1 - measurement_common_table_start]);

    debugSerial.print("frequency_f=");
    debugSerial.println(data[frequency_f - measurement_common_table_start]);
  }
  else
  {
    debugSerial.print("Read failed: 0x");
    debugSerial.println(result, HEX);
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

  //  Wait a while before looping. 10000 milliseconds = 10 seconds.
  delay(10000);
}

// set word 0 of TX buffer to least-significant word of counter (bits 15..0)
//node.setTransmitBuffer(0, lowWord(i));
// set word 1 of TX buffer to most-significant word of counter (bits 31..16)
//node.setTransmitBuffer(1, highWord(i));
// slave: write TX buffer to (2) 16-bit registers starting at register 0
//result = node.writeMultipleRegisters(0, 2);

/*
Expected output:
*/

