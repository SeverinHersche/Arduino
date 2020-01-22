#include <Wire.h>

#include <SparkFun_APDS9960.h>

// Pins
#define APDS9960_INT 2 // Needs to be an interrupt pin

// Constants

// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

int lightPin1 = 8;
int lightPin2 = 9;
int lightPin3 = 10;
int lightPin4 = 11;
int lightPin5 = 12;
int lightPin6 = 13;

void setup() {

  // Set interrupt pin as input
  pinMode(APDS9960_INT, INPUT);

  setUpPins();

  // Initialize Serial port
  Serial.begin(9600);

  // Initialize interrupt service routine
  attachInterrupt(0, interruptRoutine, FALLING);

  // Initialize APDS-9960 (configure I2C and initial values)
  apds.init();

  // Start running the APDS-9960 gesture sensor engine
  apds.enableGestureSensor(true);
}

void loop() {
  if (isr_flag == 1) {
    detachInterrupt(0);
    handleGesture();
    isr_flag = 0;
    attachInterrupt(0, interruptRoutine, FALLING);
  }
}

void interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
  if (apds.isGestureAvailable()) {
    switch (apds.readGesture()) {
    case DIR_UP:
      rollTheDice();
      break;
    case DIR_DOWN:
      rollTheDice();
      break;
    case DIR_LEFT:
      rollTheDice();
      break;
    case DIR_RIGHT:
      rollTheDice();
      break;
    case DIR_NEAR:
      rollTheDice();
      break;
    case DIR_FAR:
      rollTheDice();
      break;
    default:
      Serial.println("NONE");
    }
  }
}

void rollTheDice() {
  int randomNumber = rand() % 6 + 1;
  Serial.println(randomNumber);
  switch (randomNumber) {
  case 6:
    digitalWrite(lightPin6, HIGH);
  case 5:
    digitalWrite(lightPin5, HIGH);
  case 4:
    digitalWrite(lightPin4, HIGH);
  case 3:
    digitalWrite(lightPin3, HIGH);
  case 2:
    digitalWrite(lightPin2, HIGH);
  case 1:
    digitalWrite(lightPin1, HIGH);
    break;
  default:
    Serial.println("ERROR");
  }
  delay(2000);
  allLightsOff();
}
void allLightsOff() {
  digitalWrite(lightPin1, LOW);
  digitalWrite(lightPin2, LOW);
  digitalWrite(lightPin3, LOW);
  digitalWrite(lightPin4, LOW);
  digitalWrite(lightPin5, LOW);
  digitalWrite(lightPin6, LOW);
}

//setup light pins
void setUpPins() {
  pinMode(lightPin1, OUTPUT);
  pinMode(lightPin2, OUTPUT);
  pinMode(lightPin3, OUTPUT);
  pinMode(lightPin4, OUTPUT);
  pinMode(lightPin5, OUTPUT);
  pinMode(lightPin6, OUTPUT);
}
