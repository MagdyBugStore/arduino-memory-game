#include <Arduino.h>

const int clockPin = 10; // SH_CP
const int latchPin = 11; // ST_CP
const int dataPin = 12;  // DS

void sendShiftRegisterData(byte dataRegister1, byte dataRegister2)
{
  digitalWrite(latchPin, LOW); // Prepare to send data

  // Send data for the second shift register first
  for (int i = 7; i >= 0; i--)
  {
    digitalWrite(dataPin, (dataRegister2 >> i) & 1); // Set data pin to the current bit
    digitalWrite(clockPin, HIGH);                   // Pulse clock to shift the bit
    digitalWrite(clockPin, LOW);
  }

  // Send data for the first shift register
  for (int i = 7; i >= 0; i--)
  {
    digitalWrite(dataPin, (dataRegister1 >> i) & 1); // Set data pin to the current bit
    digitalWrite(clockPin, HIGH);                   // Pulse clock to shift the bit
    digitalWrite(clockPin, LOW);
  }

  digitalWrite(latchPin, HIGH); // Latch data to output pins
}

void setup()
{
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
}

void loop()
{
  byte dataRegister1 = 0b00000000; // Data for the first shift register
  byte dataRegister2 = 0b00001010; // Data for the second shift register

  sendShiftRegisterData(dataRegister1, dataRegister2); // Call the function with data

  delay(1000); // Wait for 1 second before repeating
}
