#include <Keypad.h>

// Using Keypad setup code from: https://dronebotworkshop.com/keypads-arduino/

// Setting up the rows and columns for the keypad.
const byte ROWS = 4;
const byte COLS = 4;

// 2D Array for Keypad Mapping.
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Keypad Pin Connections on the Arduino.
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

// Creating a keypad object.
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const int buzzPin = 10; // Pin where Buzzer is connected.
const int potPin = A1; // Pin where Potentiometer is connected.

const int greenLed = 11; // Pin where green LED is connected.
const int blueLed = 12; // Pin where blue LED is connected.
const int redLed = 13; // Pin where red LED is connected.

int potVal = 0; // Variable for storing potentiometer value.
int toneVal = 0; // Variable for storing the potentiometer adjusting value.

unsigned long previousMillis = 0;        // Will store last time LED was updated

int loudCounter = 0; // Counter to set a delay before sending the notification to phone. Needed to avoid spam.

// Interval values will control LED sequences later. These are arbitrary defaults.
unsigned long greenLEDinterval = 100;
unsigned long redLEDinterval = 100;
unsigned long blueLEDinterval = 100;

// Timers needed to use the millis() function for parallel functioning of the lights.
unsigned long greenLEDtimer;
unsigned long redLEDtimer;
unsigned long blueLEDtimer;

// Just initializing all LED's as off.
int blueLedState = LOW;
int redLedState = LOW;
int greenLedState = LOW;

// This notes whether the sequence is in progress or not.
bool startLightSequence = false;

// Boolean to check for individual lights.
bool startGreenLedSequence = false;
bool startRedLedSequence = false;
bool startBlueLedSequence = false;

void setup() {
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  pinMode(potPin, INPUT);

  // Setting up the millis() timers for parallel LED lighting.
  // So that other code can continue working without being blocked by delay() function.
  greenLEDtimer = millis();
  redLEDtimer = millis();
  blueLEDtimer = millis();

  Serial.begin(9600);
}

void toggleGreenLEDParallel() {
  digitalRead(greenLed) == LOW ? digitalWrite(greenLed, HIGH) : digitalWrite(greenLed, LOW);
  // remember the time when we toggled it
  greenLEDtimer = millis();
}

void toggleRedLEDParallel() {
  digitalRead(redLed) == LOW ? digitalWrite(redLed, HIGH) : digitalWrite(redLed, LOW);
  // remember the time when we toggled it
  redLEDtimer = millis();
}

void toggleBlueLEDParallel() {
  digitalRead(blueLed) == LOW ? digitalWrite(blueLed, HIGH) : digitalWrite(blueLed, LOW);
  // remember the time when we toggled it
  blueLEDtimer = millis();
}


void lightSequenceParallelGreen() {
  if ( (millis () - greenLEDtimer) >= greenLEDinterval)
    toggleGreenLEDParallel();
}

void lightSequenceParallelRed() {
  if ( (millis () - redLEDtimer) >= redLEDinterval)
    toggleRedLEDParallel();
}

void lightSequenceParallelBlue() {
  if ( (millis () - blueLEDtimer) >= blueLEDinterval)
    toggleBlueLEDParallel();
}


void lightSequenceParallel() {
  // Essentially these conditions mean that if more than xyzLEDinterval time has passed since the LED has been toggled,
  // it is time to toggle it. So its the same as delay() but with extra steps.

  lightSequenceParallelGreen();
  lightSequenceParallelRed();
  lightSequenceParallelBlue();
  //  if ( (millis () - greenLEDtimer) >= greenLEDinterval)
  //    toggleGreenLEDParallel();
  //
  //  if ( (millis () - redLEDtimer) >= redLEDinterval)
  //    toggleRedLEDParallel();
  //
  //  if ( (millis () - blueLEDtimer) >= blueLEDinterval)
  //    toggleBlueLEDParallel();
}


//void lightSequenceSequential() {
//
//  digitalWrite(greenLed, HIGH);
//  delay(200);
//  digitalWrite(blueLed, HIGH);
//  delay(200);
//  digitalWrite(redLed, HIGH);
//  delay(200);
//
//  digitalWrite(greenLed, LOW);
//  delay(300);
//  digitalWrite(blueLed, LOW);
//  delay(300);
//  digitalWrite(redLed, LOW);
//  delay(300);
//
//  digitalWrite(greenLed, HIGH);
//  delay(200);
//  digitalWrite(blueLed, LOW);
//  delay(200);
//  digitalWrite(redLed, HIGH);
//
//  delay(300);
//
//  digitalWrite(greenLed, LOW);
//  delay(200);
//  digitalWrite(blueLed, HIGH);
//  delay(200);
//  digitalWrite(redLed, LOW);
//
//  delay(300);
//
//  digitalWrite(greenLed, LOW);
//  delay(100);
//  digitalWrite(blueLed, LOW);
//  delay(100);
//  digitalWrite(redLed, HIGH);
//
//  delay(300);
//
//  digitalWrite(greenLed, HIGH);
//  delay(200);
//  digitalWrite(blueLed, HIGH);
//  delay(200);
//  digitalWrite(redLed, HIGH);
//
//  delay(300);
//
//  digitalWrite(greenLed, LOW);
//  digitalWrite(blueLed, LOW);
//  digitalWrite(redLed, LOW);
//
//}

void loop() {
  char customKey = customKeypad.getKey();

  potVal = analogRead(potPin);
  // Formula from Paul McWhorter's calculations for passive buzzer at https://youtu.be/dHYKRrlPNzM.
  // I added the / 30 at the end to further decrease the sensitivity.
  toneVal = ((9940. / 1023.) * potVal + 60) / 30;

  if (toneVal < 100) {
    loudCounter += 1;
    if (loudCounter > 100) {
      Serial.println((String)"Loud-" + toneVal);
      loudCounter = 0;
    }
  }

  if (startLightSequence)
    lightSequenceParallel();

  if (startGreenLedSequence)
    lightSequenceParallelGreen();

  if (startRedLedSequence)
    lightSequenceParallelRed();

  if (startBlueLedSequence)
    lightSequenceParallelBlue();

  //    analogWrite(buzzPin, HIGH);
  //    delayMicroseconds(toneVal);
  //    analogWrite(buzzPin, LOW);
  //    delayMicroseconds(toneVal);

  digitalWrite(buzzPin, HIGH);
  delay(toneVal);
  digitalWrite(buzzPin, LOW);
  delay(toneVal);

  if (customKey) {
    if (customKey == '1' && !startLightSequence) { // green. turn off sequence for individual lighting.
      //      digitalWrite(greenLed, HIGH);
      startGreenLedSequence = true;
    }
    else if (customKey == '2' && !startLightSequence) { // blue. turn off sequence for individual lighting.
      //      digitalWrite(blueLed, HIGH);
      startRedLedSequence = true;
    }
    else if (customKey == '3' && !startLightSequence) { // red. turn off sequence for individual lighting.
      //      digitalWrite(redLed, HIGH);
      startBlueLedSequence = true;

    }
    else if (customKey == 'A') {
      startLightSequence = true;
      greenLEDinterval = 1000;
      redLEDinterval = 1250;
      blueLEDinterval = 1800;
      Serial.println("Seq-A");
    }
    else if (customKey == 'B') {
      startLightSequence = true;
      greenLEDinterval = 300;
      redLEDinterval = 400;
      blueLEDinterval = 600;
      Serial.println("Seq-B");
    }
    else if (customKey == 'C') {
      startLightSequence = true;
      greenLEDinterval = 1800;
      redLEDinterval = 2550;
      blueLEDinterval = 3000;
      Serial.println("Seq-C");
    }
    //    if (customKey == '9') {
    //      play();
    //    }
    else if (customKey == '*') {
      startLightSequence = false;
      startGreenLedSequence = false;
      startRedLedSequence = false;
      startBlueLedSequence = false;
      digitalWrite(greenLed, LOW);
      digitalWrite(blueLed, LOW);
      digitalWrite(redLed, LOW);
    }
  }
}
