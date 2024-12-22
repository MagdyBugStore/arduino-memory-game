#include <Arduino.h>

const int clockPin = 10; // SH_CP
const int latchPin = 11; // ST_CP
const int dataPin = 12;  // DS

void sendShiftRegisterData(byte dataRegister1, int8_t segment)
{
    byte segmentPattern; // To store the bit pattern for the 7-segment display

    // Map the segment number to its corresponding bit pattern
    switch (segment)
    {
    case 0:
        segmentPattern = 0b00111111;
        break; // 0
    case 1:
        segmentPattern = 0b00000110;
        break; // 1
    case 2:
        segmentPattern = 0b01011011;
        break; // 2
    case 3:
        segmentPattern = 0b01001111;
        break; // 3
    case 4:
        segmentPattern = 0b01100110;
        break; // 4
    case 5:
        segmentPattern = 0b01101101;
        break; // 5
    case 6:
        segmentPattern = 0b01111101;
        break; // 6
    case 7:
        segmentPattern = 0b00000111;
        break; // 7
    case 8:
        segmentPattern = 0b01111111;
        break; // 8
    case 9:
        segmentPattern = 0b01101111;
        break; // 9
    default:
        segmentPattern = 0b00000000;
        break; // Default (all off)
    }

    digitalWrite(latchPin, LOW); // Prepare to send data

    // Send data for the second shift register (segmentPattern)
    for (int i = 7; i >= 0; i--)
    {
        digitalWrite(dataPin, !((segmentPattern >> i) & 1)); // Set data pin to the current bit
        digitalWrite(clockPin, HIGH);                        // Pulse clock to shift the bit
        digitalWrite(clockPin, LOW);
    }

    // Send data for the first shift register (dataRegister1)
    for (int i = 7; i >= 0; i--)
    {
        digitalWrite(dataPin, (dataRegister1 >> i) & 1); // Set data pin to the current bit
        digitalWrite(clockPin, HIGH);                    // Pulse clock to shift the bit
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
    for (int8_t segment = 0; segment <= 9; segment++) // Loop through numbers 0 to 9
    {
        sendShiftRegisterData(0b00000000, segment); // Example: dataRegister1 = 0b10101010
        delay(1000);                                // Wait 1 second before updating
    }
}