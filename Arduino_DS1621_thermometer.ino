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
  
  Wire.beginTransmission(ADDRESS);
  Wire.write(ACCESS_CONFIG);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS, 1);
  byte config = Wire.read();
  config |= ONE_SHOT;

  Wire.beginTransmission(ADDRESS);
  Wire.write(ACCESS_CONFIG);
  Wire.write(config);
  Wire.endTransmission();
}

void loop() {
  Wire.beginTransmission(ADDRESS);
  Wire.write(START_CONVERT);
  Wire.endTransmission();
  digitalWrite(LED_BUILTIN, HIGH);

  while (true) {
    Wire.beginTransmission(ADDRESS);
    Wire.write(ACCESS_CONFIG);
    Wire.endTransmission();
    Wire.requestFrom(ADDRESS, 1);
    byte config = Wire.read();
    if (config & DONE == DONE) break;
    delay(100);
  }
  digitalWrite(LED_BUILTIN, LOW);  

  Wire.beginTransmission(ADDRESS);
  Wire.write(READ_TEMP);
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS, 2);
  byte msb = Wire.read();
  byte lsb = Wire.read();
  Serial.println(msb);
  delay(500);
}
