#include <EEPROM.h>

const byte DS1621_ADDRESS = 0x48;
const byte DELAY_TIME = 10;

void setup() {
    Serial.begin(9600);

    EEPROM.update(0, DS1621_ADDRESS);
    EEPROM.update(1, DELAY_TIME);
}

byte value;

void loop() {
    Serial.println(F("Reading EEPROM:"));
    for (int i=0; i<2; i++) {
        value = EEPROM.read(i);
        Serial.print(F("\t["));
        Serial.print(i, HEX);
        Serial.print(F("] = "));
        if (value < 0x10) Serial.print(F("0"));
        Serial.println(value, HEX);
    }
    Serial.println();
    delay(5000);
}
