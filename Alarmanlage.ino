/* 
|| @version 4.0
|| @author Severin Hersche
|| @contact severin.hersche@ubs.com
||
|| @description
|| | Eine kleine Alarmanlage die mit einem Bewegungsesnor funktioniert.
|| #
*/
#include <Keypad.h>

const int INPUT_MAX = 4; //passwort eingabe auf 4 festgelegt

boolean isTriggered = false; //ist um zu ueberpruefen ob der Bewegungsensor los ging

//Diese sind für die RGB Lampe
int redLightPin = 10;
int greenLightPin = 11;
int blueLightPin = 12;

//für den Bewegungsensor
int motionSensorOutput = A0;

//Hier ist der Bewegungsensor verbunden
int motionSensorPin = 13;

//für den piezo buzzer
int soundPin = A3;

//Das sind die einfachen Lichten für das blaurot
int redLight = A5;
int blueLight = A1;

//Das wird gebrauch um das balu rotlicht zu blinken
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000;

//Hier wird die Passwort eingabe gespeichert
char passwortInput[INPUT_MAX];
//Hier ist das Passwort definiert
char password[INPUT_MAX] = {'1','5','D','B'};

//Hier zaehlt man die eingabe des Passworts
int inputCount = 0;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9,8,7,6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5,4,3,2}; //connect to the column pinouts of the keypad
//Hier wird das keypad eingerichted
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  //Die pins werden hier gesetuped 
  setUpPins();

  //Hier wird das RGB licht gesetuped 
  setUpRGB();
  Serial.begin(9600);
}

void loop() {
  int motionSensor = digitalRead(motionSensorPin);
  if (motionSensor && !isTriggered) {
    isTriggered = true;
  }
  //Hier wird das Blaulicht ausgegeben, wenn der Bewegungsensor ausgelöst wird
  digitalWrite(motionSensorOutput, motionSensor);

  if (isTriggered) {
    //wenn der Bewegnungsensor eine Bewegung bemerkt wird das Blincken und der ton ausgegeben
    imitatePolice();
    tone(soundPin, 4000);
  }

  char key = keypad.getKey();
  if (key) {
    //Wenn der input = # ist wird überprüft, ob die eingabe korrekt war
    if (key == '#') {
      if (strncmp(passwortInput, password, sizeof(passwortInput)) == 0) {
        //Wenn ja wird das Grüne licht gezeigt
        digitalWrite(greenLightPin, LOW);
        delay(4000);
        digitalWrite(greenLightPin, HIGH);
        //Und der ausgelöste Alarm ausgestellt
        noTone(soundPin);
        stopPolice();
        isTriggered = false;
        resetInput();

      } else {
        //Wenn die eingabe nicht korrekt war wird das Rotelicht gezeigt
        digitalWrite(redLightPin, LOW);
        delay(1000);
        digitalWrite(redLightPin, HIGH);
        //der Input wird zurückgesetzt
        resetInput();
      }
    } else if (inputCount < INPUT_MAX) {
      //Hier wird überprüft, ob die eingabe unter 4 Zeichen ist und wenn ja wird sie gespeichert
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
//Das blinken wird eingestellt
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
//das Blinken wird ausgestellt
void stopPolice() {
  digitalWrite(redLight, LOW);
  digitalWrite(blueLight, LOW);
}
