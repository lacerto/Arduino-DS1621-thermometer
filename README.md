# Arduino DS1621 thermometer

## Description
A simple thermometer using a [DS1621 digital thermometer](https://pdfserv.maximintegrated.com/en/ds/DS1621.pdf).
An Arduino connects to the DS1621 via I<sup>2</sup>C, reads out the currently measured temperature and displays it.

## First setup
### Schematic
![First setup](https://github.com/lacerto/Arduino-DS1621-thermometer/blob/master/01_Thermometer_Serial_output.png)

This setup uses the built-in LED, which is lit while the conversion in the DS1621 is running.
### Device address
The DS1621 uses a [7-bit address](https://en.wikipedia.org/wiki/I%C2%B2C#7-bit_addressing). Its predefined 4-bit control code is set to 1001, the 3 least significant bits can be set using the device select pins A2, A1, A0 (A0 is LSB).  

In this setup all device select pins are on the ground, which gives us the following address:  
`Control code:       1001`  
`Device select bits:  000`  

`1001|000 = 0x48`

### How it works
#### setup()
* Initialize the built-in LED, the DS1621 to work in one-shot mode and the serial communication.
#### loop()
* Check if there was an error during transmission. If yes then let the built-in LED blink rapidly and print the error code to serial. Do nothing else.
* No error, so give the command to the DS1621 to start the temperature conversion. While waiting for the conversion to finish the built-in LED is on.
* Read and print the temperature based on the 2-byte value read from the DS1621.
* Calculate and print the high resolution temperature value as described in the datasheet.
