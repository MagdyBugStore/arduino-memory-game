#include <Arduino.h>

// Arduino code to toggle Q1, Q2, Q3, Q4 of the 74HC595 HIGH and LOW alternately

const int dataPin = 10;  // DS
const int clockPin = 11; // SH_CP
const int latchPin = 12; // ST_CP

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  // Set Q1-Q4 to HIGH (00001111 in binary)
  byte q1ToQ4High = 0x0F; // Q1-Q4 HIGH, Q5-Q8 LOW
  byte q1ToQ4Low = 0x00;  // Q1-Q4 LOW, Q5-Q8 LOW

  // Print HIGH state to the Serial Monitor
  // Output Q1-Q4 HIGH to the 74HC595
  
//   digitalWrite(latchPin, LOW); // Begin sending data
  digitalWrite(latchPin, HIGH); // Output the data to the pins
  Serial.println(q1ToQ4High);
  shiftOut(dataPin, clockPin, MSBFIRST, q1ToQ4High); // Send the 8-bit data
  digitalWrite(latchPin, LOW); // Begin sending data
  delay(500); // Wait for 1 second

  digitalWrite(latchPin, HIGH); // Output the data to the pins
  shiftOut(dataPin, clockPin, MSBFIRST, q1ToQ4Low); // Send the 8-bit data
  digitalWrite(latchPin, LOW); // Begin sending data

  delay(1000); // Wait for 1 second
//   // Print LOW state to the Serial Monitor
//   Serial.println("Setting Q1-Q4 LOW");

//   // Output Q1-Q4 LOW to the 74HC595
//   digitalWrite(latchPin, LOW); // Begin sending data
//   shiftOut(dataPin, clockPin, MSBFIRST, q1ToQ4Low); // Send the 8-bit data
//   digitalWrite(latchPin, HIGH); // Output the data to the pins

//   delay(1000); // Wait for 1 second
}
