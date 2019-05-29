/*
	Arduino DS1621 Thermometer

  Read the temperature from a DS1621 digital thermometer and
  send the results as human readable text through the serial interface,
  so it can be checked in the serial monitor.
  The built-in LED is lit while the Arduino is waiting for the conversion
  in the DS1621 to finish.
  The built-in LED blinks rapidly if the device is not found on the bus or
  any other transmission error occurs. The error code is written to serial.

  The circuit:
  * 2x 4.7K resistors
  * 1x DS1621

  The DS1621 SDA(1) and SCL(2) pins are connected via to the 5V coming from
  the Arduino's 5V pin via the two 4.7K pull-up resistors. The SDA and SCL pins
  are also connected directly to the SDA and SCL pins of the Arduino (pins marked
  as A4[SDA] and A5[SCL]; they can also be found duplicated and labeled as 
  SDA and SCL [beside the AREF pin] on the REV 3 layout boards).
  The chip select pins of the DS1621 A2(5), A1(6), A0(7) are on the ground.

*/

#include <Wire.h>

// Commands of the DS1621. See the COMMAND SET section in its documentation.
#define ACCESS_CONFIG 0xAC  // reads or writes the configuration register
#define START_CONVERT 0xEE  // starts the temperature conversion
#define READ_TEMP     0xAA  // reads the result of the last conversion (2 bytes)
#define READ_COUNTER  0xA8  // reads the COUNT_REMAIN value
#define READ_SLOPE    0xA9  // reads the COUNT_PER_C value

// Bits in the DS1621 configuration register.
#define ONE_SHOT      0
#define DONE          7

// The address of the connected DS1621 on the two-wire serial bus.
// The DS1621 uses a 7-bit address and the upper 4 bits are fixed as 1001.
// The remaining 3 bits can be configured using the 3 chip select pins of
// the device. As these are on the ground (==0) in the configuration explained above,
// so we get the address 1001000 = 0x48.
const int ADDRESS = 0x48;

// This is set to the error code returned by endTransmission.
byte transmissionError = 0;

void setup() {
  // Set built-in LED pin to output and low.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Initialize the serial and 2-wire serial bus communication.
  Serial.begin(9600);
  Wire.begin();

  // Initialize the DS1621.
  initThermometer(ADDRESS);
}

byte msb = 0; // MSB & LSB of the temperature reading
byte lsb = 0;

void loop() {
  if (transmissionError == 0) {
    readAndDisplayTemperature(LED_BUILTIN);
  } else {
    Serial.print(F("Transmission error: "));
    Serial.println(transmissionError);
    blinkLED(LED_BUILTIN, 50);
  }
}

void blinkLED(byte ledPin, unsigned long delayMillis) {
  digitalWrite(ledPin, HIGH);
  delay(delayMillis);
  digitalWrite(ledPin, LOW);
  delay(delayMillis);
}

void readAndDisplayTemperature(byte ledPin) {
  // Start the temperature conversion.
  startConversion(ADDRESS);
  
  // Turn on the built-in LED while waiting for the conversion to finish.
  digitalWrite(ledPin, HIGH);
  while (!isConversionDone(ADDRESS)) {
    delay(100);
  }
  digitalWrite(ledPin, LOW);  

  // Get the 2-byte temperature reading and convert the MSB to
  // floating point. Only bit 7 of the LSB has any meaning, if
  // set it indicates that we have to add 0.5 degrees to the 
  // value in MSB. Negative values are stored as 2's complement,
  // we neglect negative values here.
  readTemperature(ADDRESS, &msb, &lsb);
  float temperature = float(msb);
  if (bitRead(lsb, 7)) temperature += 0.5;

  // Print the raw data read from the thermometer and the
  // floating point temperature value.
  Serial.print(F("Raw data: "));
  Serial.print(msb, HEX);
  Serial.print(" ");
  Serial.println(lsb, HEX);
  
  Serial.print(F("Temperature: "));
  Serial.println(temperature, 1);
  
  temperature = getHighResolutionTemp(ADDRESS, msb);
  Serial.print(F("Higher resolution value: "));
  Serial.println(temperature, 3);

  delay(1000);
}

void initThermometer(int address) {
  // Send the access config command.
  Wire.beginTransmission(address);
  Wire.write(ACCESS_CONFIG);
  transmissionError = Wire.endTransmission();
  if (transmissionError != 0) return;
  // Read the answer (1 byte).
  Wire.requestFrom(ADDRESS, 1);
  byte config = Wire.read();

  // Set the one shot mode in the config byte.
  bitSet(config, ONE_SHOT);

  // Write the config to the device.
  Wire.beginTransmission(address);
  Wire.write(ACCESS_CONFIG);
  Wire.write(config);
  transmissionError = Wire.endTransmission();  
  if (transmissionError != 0) return;
}

void startConversion(int address) {
  // Send the start convert command.
  Wire.beginTransmission(address);
  Wire.write(START_CONVERT);
  transmissionError = Wire.endTransmission();
}

bool isConversionDone(int address) {
  // Get the config byte.
  Wire.beginTransmission(address);
  Wire.write(ACCESS_CONFIG);
  transmissionError = Wire.endTransmission();  
  if (transmissionError != 0) return true;
  Wire.requestFrom(address, 1);
  byte config = Wire.read();
  // The DONE bit in the config byte signalizes that
  // the conversion finished.
  return bitRead(config, DONE); 
}

void readTemperature(int address, byte* msb, byte* lsb) {
  // Issue the read temp command and read the 2-byte answer.
  Wire.beginTransmission(address);
  Wire.write(READ_TEMP);
  transmissionError = Wire.endTransmission();  
  if (transmissionError != 0) return;
  Wire.requestFrom(address, 2);
  *msb = Wire.read();
  *lsb = Wire.read();
}

float getHighResolutionTemp(int address, byte msb) {
  // Get the slope and the counter values and calculate
  // the high resolution value as described in the datasheet.
  Wire.beginTransmission(address);
  Wire.write(READ_COUNTER);
  transmissionError = Wire.endTransmission();  
  if (transmissionError != 0) return 0.0;
  Wire.requestFrom(address, 1);
  float countRemain = Wire.read();

  Wire.beginTransmission(address);
  Wire.write(READ_SLOPE);
  transmissionError = Wire.endTransmission();  
  if (transmissionError != 0) return 0.0;
  Wire.requestFrom(address, 1);
  float countPerC = Wire.read();

  return (float(msb) - 0.25) + ((countPerC - countRemain) / countPerC);
}
