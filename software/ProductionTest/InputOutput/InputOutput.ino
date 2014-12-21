
#include <Bounce2.h>



#define sensorPin A0    // Battery Voltage
#define sensorPin2 A1    // Battery charge current
#define sensorPin3 A2    // Battery discharge current

#define chargeLed1Pin 13      // select the pin for the LED
#define chargePWM1Pin 3    // select the pin for the LED
#define dischargePWM1Pin 10    // select the pin for the LED
#define chargeLed2Pin 5      // select the pin for the LED
#define chargePWM2Pin 11    // select the pin for the LED
#define dischargePWM2Pin 9    // select the pin for the LED
#define LED5 1
#define LED6 SCL

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
// UI buttons
#define butDown     4
#define butUp       7
#define butStart    8
#define butTest    12
#define butS5       6
#define butS6     SDA

// Debouncers
Bounce debDown  = Bounce();
Bounce debUp    = Bounce();
Bounce debStart    = Bounce();
Bounce debTest    = Bounce();
Bounce debS5    = Bounce();
Bounce debS6    = Bounce();

bool ledState = false;
const long interval = 2000;
unsigned long previousMillis = 0;        // will store last time LED was updated

void setup() {
  // put your setup code here, to run once:
  pinMode(chargePWM1Pin, OUTPUT);
  pinMode(chargeLed1Pin, OUTPUT);
  pinMode(dischargePWM1Pin, OUTPUT);
  pinMode(chargePWM2Pin, OUTPUT);
  pinMode(chargeLed2Pin, OUTPUT);
  pinMode(dischargePWM2Pin, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);


  pinMode(butDown, INPUT_PULLUP);
  debDown.attach(butDown);
  debDown.interval(25);

  pinMode(butUp, INPUT_PULLUP);
  debUp.attach(butUp);
  debUp.interval(25);

  pinMode(butStart, INPUT_PULLUP);
  debStart.attach(butStart);
  debStart.interval(25);

  pinMode(butTest, INPUT_PULLUP);
  debTest.attach(butTest);
  debTest.interval(25);

  pinMode(butS5, INPUT_PULLUP);
  debS5.attach(butS5);
  debS5.interval(25);

  pinMode(butS6, INPUT_PULLUP);
  debS6.attach(butS6);
  debS6.interval(25);

  Serial.begin(9600);
  Serial.println(F("Input Output Test is starting up..."));
}

void loop() {
  unsigned long currentMillis = millis();
  debDown.update();
  debUp.update();
  debStart.update();
  debTest.update();
  debS5.update();
  debS6.update();



  if (!debDown.read())
  {
    Serial.println(F("butDown pressed."));
  }

  if (!debUp.read())
  {
    Serial.println(F("butUp pressed."));
  }

  if (!debStart.read())
  {
    Serial.println(F("butStart pressed."));
  }

  if (!debTest.read())
  {
    Serial.println(F("butTest pressed."));
  }

  if (!debS5.read())
  {
      Serial.println(F("butS5 pressed."));
  }

  if (!debS6.read())
  {
      Serial.println(F("butS6 pressed."));
  }

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    ledState = !ledState;


    digitalWrite(chargeLed1Pin, ledState);
    digitalWrite(chargePWM1Pin, ledState);
    digitalWrite(dischargePWM1Pin, ledState);
    digitalWrite(chargeLed2Pin, ledState);
    digitalWrite(chargePWM2Pin, ledState);
    digitalWrite(dischargePWM2Pin, ledState);
    digitalWrite(LED5, ledState);
    digitalWrite(LED6, ledState);
  }
}
