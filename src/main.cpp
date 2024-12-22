#include <Arduino.h>

// Pins for shift register
const int clockPin = 10; // SH_CP
const int latchPin = 11; // ST_CP
const int dataPin = 12;  // DS
const int buzzerPin = 9; // Buzzer pin

// Buttons and LED states
const int buttonPins[] = {2, 3, 4, 5}; // Buttons connected to these pins
byte ledStates = 0b00000000;           // Initial state for 4 LEDs (all off)

// Memory game variables
int level = 1;                   // Current game level
int sequence[10];                // Random LED sequence
int playerInput[10];             // Player's input
int sequenceLength = 1;          // Current sequence length
int currentInputIndex = 0;       // Tracks the player's input progress
bool gameActive = true;          // Tracks if the game is active

void sendShiftRegisterData(byte dataRegister1, int8_t segment)
{
    byte segmentPattern; 

    switch (segment)
    {
    case 0: segmentPattern = 0b00111111; break; // 0
    case 1: segmentPattern = 0b00000110; break; // 1
    case 2: segmentPattern = 0b01011011; break; // 2
    case 3: segmentPattern = 0b01001111; break; // 3
    case 4: segmentPattern = 0b01100110; break; // 4
    case 5: segmentPattern = 0b01101101; break; // 5
    case 6: segmentPattern = 0b01111101; break; // 6
    case 7: segmentPattern = 0b00000111; break; // 7
    case 8: segmentPattern = 0b01111111; break; // 8
    case 9: segmentPattern = 0b01101111; break; // 9
    default: segmentPattern = 0b00000000; break; // Default (all off)
    }

    digitalWrite(latchPin, LOW); 

    for (int i = 7; i >= 0; i--)
    {
        digitalWrite(dataPin, !((segmentPattern >> i) & 1)); 
        digitalWrite(clockPin, HIGH);                        
        digitalWrite(clockPin, LOW);
    }

    for (int i = 7; i >= 0; i--)
    {
        digitalWrite(dataPin, (dataRegister1 >> i) & 1); 
        digitalWrite(clockPin, HIGH);                    
        digitalWrite(clockPin, LOW);
    }

    digitalWrite(latchPin, HIGH);
    Serial.println("Shift register updated.");
}

void updateLEDs()
{
    sendShiftRegisterData(ledStates, level % 10); 
    Serial.print("LEDs updated: ");
    Serial.println(ledStates, BIN);
}

void playWinnerTone()
{
    tone(buzzerPin, 1000, 200);
    delay(200);
    tone(buzzerPin, 1500, 200);
    delay(200);
    tone(buzzerPin, 2000, 400);
    delay(400);
    Serial.println("Winner tone played.");
}

void playFailureTone()
{
    tone(buzzerPin, 200, 400);
    delay(400);
    tone(buzzerPin, 150, 400);
    delay(400);
    tone(buzzerPin, 100, 600);
    delay(600);
    Serial.println("Failure tone played.");
}

void displaySequence()
{
    Serial.print("Displaying sequence: ");
    for (int i = 0; i < sequenceLength; i++)
    {
        ledStates = (1 << sequence[i]); 
        updateLEDs();
        delay(500);
        ledStates = 0; 
        updateLEDs();
        delay(300);

        Serial.print(sequence[i]);
        Serial.print(" ");
    }
    Serial.println();
}

bool checkInput()
{
    Serial.print("Checking input: ");
    for (int i = 0; i < sequenceLength; i++)
    {
        Serial.print(playerInput[i]);
        Serial.print(" ");
        if (playerInput[i] != sequence[i])
        {
            Serial.println("Input mismatch.");
            return false; 
        }
    }
    Serial.println("Input matched.");
    return true; 
}

void generateSequence()
{
    Serial.print("Generating sequence: ");
    for (int i = 0; i < 10; i++)
    {
        sequence[i] = random(0, 4); 
        Serial.print(sequence[i]);
        Serial.print(" ");
    }
    Serial.println();
}

void setup()
{
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);

    for (int i = 0; i < 4; i++)
    {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }

    Serial.begin(9600);
    updateLEDs(); 
    generateSequence();
    Serial.println("Setup complete.");
}

void loop()
{
    if (!gameActive)
    {
        Serial.println("Game inactive.");
        return; 
    }

    displaySequence();

    currentInputIndex = 0;
    while (currentInputIndex < sequenceLength)
    {
        for (int i = 0; i < 4; i++)
        {
            if (digitalRead(buttonPins[i]) == LOW)
            {
                playerInput[currentInputIndex] = i; 
                Serial.print("Button ");
                Serial.print(i);
                Serial.println(" pressed.");
                currentInputIndex++;
                delay(200);

                ledStates = (1 << i);
                updateLEDs();
                delay(200);
                ledStates = 0;
                updateLEDs();
            }
        }
    }

    if (checkInput())
    {
        playWinnerTone();
        level++;
        sequenceLength++;
        if (sequenceLength > 10)
        {
            sequenceLength = 10; 
        }
        Serial.print("Level up! Current level: ");
        Serial.println(level);
    }
    else
    {
        playFailureTone();
        Serial.println("Repeating sequence...");
    }

    delay(1000);
}
