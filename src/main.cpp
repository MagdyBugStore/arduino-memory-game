#include <Arduino.h>

// Pins for shift register
const int clockPin = 10; // SH_CP
const int latchPin = 11; // ST_CP
const int dataPin = 12;  // DS
const int buzzerPin = 9; // Buzzer pin

// Buttons and LED states
const int buttonPins[] = {2, 3, 4, 5}; // Buttons for game input
const int startButtonPin = 6;          // Start button pin
const int increaseLevelButtonPin = 7;  // Increase level button pin
const int decreaseLevelButtonPin = 8;  // Decrease level button pin
byte ledStates = 0b00000000;           // Initial state for LEDs (all off)

// Memory game variables
int level = 0;             // Current game level
int sequence[10];          // Random LED sequence
int playerInput[10];       // Player's input
int sequenceLength = 0;    // Current sequence length
int currentInputIndex = 0; // Tracks the player's input progress
bool gameActive = false;   // Tracks if the game is active

void sendShiftRegisterData(byte dataRegister1, int8_t segment)
{
    byte segmentPattern;

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
}

void updateLEDs()
{
    sendShiftRegisterData(ledStates, level % 10);
}

void playWinnerTone()
{
    tone(buzzerPin, 1000, 200);
    delay(200);
    tone(buzzerPin, 1500, 200);
    delay(200);
    tone(buzzerPin, 2000, 400);
    delay(400);
}

void playFailureTone()
{
    tone(buzzerPin, 200, 400);
    delay(400);
    tone(buzzerPin, 150, 400);
    delay(400);
    tone(buzzerPin, 100, 600);
    delay(600);
}

void displaySequence()
{
    for (int i = 0; i < sequenceLength; i++)
    {
        ledStates = (1 << sequence[i]);
        updateLEDs();
        delay(500);
        ledStates = 0;
        updateLEDs();
        delay(300);
    }
}

bool checkInput()
{
    for (int i = 0; i < sequenceLength; i++)
    {
        if (playerInput[i] != sequence[i])
        {
            return false;
        }
    }
    return true;
}

void generateSequence()
{
    for (int i = 0; i < 10; i++)
    {
        sequence[i] = random(0, 4);
    }
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

    pinMode(startButtonPin, INPUT_PULLUP);
    pinMode(increaseLevelButtonPin, INPUT_PULLUP);
    pinMode(decreaseLevelButtonPin, INPUT_PULLUP);

    updateLEDs();
    generateSequence();
}

void handleButtons()
{
    // Check for start button press
    if (digitalRead(startButtonPin) == LOW)
    {
        gameActive = true;
        if (level != sequenceLength)
        {
            generateSequence();
            sequenceLength = level;
        }
        else
            displaySequence();
        delay(300);
    }

    // Check for level increase button press
    if (digitalRead(increaseLevelButtonPin) == LOW)
    {
        level++;
        if (level > 9)
            level = 9;
        updateLEDs();
        delay(300);
    }

    // Check for level decrease button press
    if (digitalRead(decreaseLevelButtonPin) == LOW)
    {
        level--;
        if (level < 1)
            level = 1;
        updateLEDs();
        delay(300);
    }
}

void loop()
{
    // Check for start button press
    handleButtons();
    if (!gameActive)
    {
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

                // Provide visual feedback for the button press
                ledStates = (1 << i);
                updateLEDs();
                delay(250);
                ledStates = 0;
                updateLEDs();

                if (playerInput[currentInputIndex] != sequence[currentInputIndex])
                {
                    playFailureTone();
                    gameActive = false;
                    return;
                }

                currentInputIndex++;
                delay(250);
            }
        }
        handleButtons();
    }
    if(level == 0) return;
    playWinnerTone();
    level++;
    sequenceLength++;
    if (sequenceLength > 10)
    {
        sequenceLength = 10;
    }
    generateSequence(); // Generate a new random sequence for the next level

    delay(1000);
}

// TODO: reset and start button
// TODO: can change level while play