#include <Wire.h>

#define ACCESS_CONFIG 0xAC
#define START_CONVERT 0xEE
#define READ_TEMP     0xAA
#define ONE_SHOT      0x01
#define DONE          0x80

const int ADDRESS = 0x48;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.begin(9600);
  Wire.begin();

  initThermometer(ADDRESS);
}

byte msb = 0;
byte lsb = 0;

void loop() {
  startConversion(ADDRESS);
  
  digitalWrite(LED_BUILTIN, HIGH);
  while (!isConversionDone(ADDRESS)) {
    delay(100);
  }
  digitalWrite(LED_BUILTIN, LOW);  

  readTemperature(ADDRESS, &msb, &lsb);
  float temperature = float(msb);
  if (bitRead(lsb, 7)) temperature += 0.5;

  Serial.print(F("Raw data: "));
  Serial.print(msb, HEX);
  Serial.print(" ");
  Serial.println(lsb, HEX);
  
  Serial.print(F("Temperature: "));
  Serial.println(temperature, 1);
  
  delay(1000);
}

void initThermometer(int address) {
  Wire.beginTransmission(address);
  Wire.write(ACCESS_CONFIG);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS, 1);
  byte config = Wire.read();
  config |= ONE_SHOT;

  Wire.beginTransmission(address);
  Wire.write(ACCESS_CONFIG);
  Wire.write(config);
  Wire.endTransmission();  
}

void startConversion(int address) {
  Wire.beginTransmission(address);
  Wire.write(START_CONVERT);
  Wire.endTransmission();  
}

bool isConversionDone(int address) {
  Wire.beginTransmission(address);
  Wire.write(ACCESS_CONFIG);
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  byte config = Wire.read();
  return bitRead(config, 7); 
}

void readTemperature(int address, byte* msb, byte* lsb) {
  Wire.beginTransmission(address);
  Wire.write(READ_TEMP);
  Wire.endTransmission();
  Wire.requestFrom(address, 2);
  *msb = Wire.read();
  *lsb = Wire.read();
}
