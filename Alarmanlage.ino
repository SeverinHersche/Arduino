/* 
|| @version 4.0
|| @author Severin Hersche
|| @contact severin.hersche@ubs.com
||
|| @description
|| | A little password input thing.
|| #
*/
#include <Keypad.h>

const int INPUT_MAX = 4;

boolean isTriggered = false;

//Diese sind für die RGB Lampe
int redLightPin = 9;
int greenLightPin = 10;
int blueLightPin = 11;

int motionSensorOutput = A0;
int redLight = A5;
int blueLight = A1;
int soundPin = A3;

int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000;

char passwortInput[4];
char password[4] = {
  '1',
  '5',
  'D',
  'B'
};

int inputCount = 0;

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {12,8,7,6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5,4,3,2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {

  setUpPins();

  setUpRGB();
  Serial.begin(9600);
}

void loop() {
  if (isTriggered) {
    imitatePolice();
    tone(soundPin, 4000);
  }

  int motionSensor = digitalRead(13);
  if (motionSensor && !isTriggered) {
    isTriggered = true;
  }
  digitalWrite(motionSensorOutput, motionSensor);

  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      if (strncmp(passwortInput, password, sizeof(passwortInput)) == 0) {
        digitalWrite(greenLightPin, LOW);
        delay(4000);
        digitalWrite(greenLightPin, HIGH);

        noTone(soundPin);
        stopPolice();
        isTriggered = false;
        resetInput();

      } else {
        digitalWrite(redLightPin, LOW);
        delay(1000);
        digitalWrite(redLightPin, HIGH);
        resetInput();
      }
    } else if (inputCount < INPUT_MAX) {
      passwortInput[inputCount] = key;
      inputCount += 1;
    } else {
      resetInput();
      showError();

    }
  }
}

void setUpPins() {
  pinMode(motionSensorOutput, OUTPUT);
  pinMode(redLightPin, OUTPUT);
  pinMode(greenLightPin, OUTPUT);
  pinMode(blueLightPin, OUTPUT);
  pinMode(redLight, OUTPUT);
  pinMode(blueLight, OUTPUT);
}

void setUpRGB() {
  digitalWrite(redLightPin, HIGH);
  digitalWrite(greenLightPin, HIGH);
  digitalWrite(blueLightPin, HIGH);
}

void showError() {
  digitalWrite(redLightPin, LOW);
  delay(1000);
  digitalWrite(redLightPin, HIGH);
}

void resetInput() {
  inputCount = 0;
  memset(passwortInput, 0, sizeof passwortInput);
}
void imitatePolice() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(redLight, ledState);
    digitalWrite(blueLight, !ledState);
  }
}

void stopPolice() {
  digitalWrite(redLight, LOW);
  digitalWrite(blueLight, LOW);
}
