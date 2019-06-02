byte bcdPins[] = { 2, 3, 4, 5 };
byte enablePin1 = 8;
byte enablePin2 = 9;

void setup() {
  pinMode(enablePin1, OUTPUT);
  digitalWrite(enablePin1, HIGH);
  pinMode(enablePin2, OUTPUT);
  digitalWrite(enablePin2, LOW);

  for (int i=0; i<4; i++) {
    pinMode(bcdPins[i], OUTPUT);
    digitalWrite(bcdPins[i], LOW);
  }
}

byte i = 0;
byte counter = 0;

void loop() {
  digitalWrite(enablePin1, HIGH);
  digitalWrite(enablePin2, LOW);

  byte digit = i % 10;
  display(digit);
  delay(5);

  digitalWrite(enablePin1, LOW);
  digitalWrite(enablePin2, HIGH);

  digit = i / 10;
  display(digit);
  delay(5);

  counter++;
  if (counter == 100) {
    counter = 0;
    i++;
    if (i == 100) i = 0;
  }
}

void display(byte digit) {
  for (byte i=0; i<4; i++) {
    digitalWrite(bcdPins[i], bitRead(digit, i));
  }
}
