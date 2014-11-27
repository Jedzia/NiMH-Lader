/*
  Analog Input
 Demonstrates analog input by reading an analog sensor on analog pin 0 and
 turning on and off a light emitting diode(LED)  connected to digital pin 13.
 The amount of time the LED will be on and off depends on
 the value obtained by analogRead().

 The circuit:
 * Potentiometer attached to analog input 0
 * center pin of the potentiometer to the analog pin
 * one side pin (either one) to ground
 * the other side pin to +5V
 * LED anode (long leg) attached to digital output 13
 * LED cathode (short leg) attached to ground

 * Note: because most Arduinos have a built-in LED attached
 to pin 13 on the board, the LED is optional.


 Created by David Cuartielles
 modified 30 Aug 2011
 By Tom Igoe

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/AnalogInput

 */

#include <Bounce2.h>

#define sensorPin A0    // select the input pin for the potentiometer
#define ledPin 13      // select the pin for the LED
#define ledPWMPin 9    // select the pin for the LED
//#define PWMout 12

#define butDown  3
#define butUp    4

Bounce debDown  = Bounce();
Bounce debUp    = Bounce();

const long interval = 100;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long previousMillisBlink = 0;
int skipcounter=0;
//int sensorValue = 128;  // variable to store the value coming from the sensor
int sensorValue = 1;  // variable to store the value coming from the sensor
int battVoltage = 0;
bool ledState = LOW;
//bool pwmState = LOW;



/**
 * Divides a given PWM pin frequency by a divisor.
 *
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 *
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired on timer0
 *   - Pins 9 and 10 are paired on timer1
 *   - Pins 3 and 11 are paired on timer2
 *
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 *
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1235060559/0#4
 */
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

char *ftoa(char *a, double f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
  
  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
}


void setup() {

  setPwmFrequency(ledPWMPin, 1);
  pinMode(butDown, INPUT_PULLUP);
  debDown.attach(butDown);
  debDown.interval(25);

  pinMode(butUp, INPUT_PULLUP);
  debUp.attach(butUp);
  debUp.interval(25);

  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  //pinMode(PWMout, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  unsigned long currentMillis = millis();
  debDown.update();
  debUp.update();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    if (!debDown.read() && (sensorValue > 0))
    {
      sensorValue--;
    }
    else if (!debUp.read() && (sensorValue < 255))
    {
      sensorValue++;
    }

    
    if ( ( skipcounter > 32 ) && Serial.available()) {
      skipcounter=0;


    analogWrite(ledPWMPin, 0);
    delay(10);
    battVoltage = analogRead(sensorPin);
    float f = 5.0 / 1024 * battVoltage; 
    char buf[8];
    ftoa(buf, f, 3);

      Serial.print("sensorValue: ");
      Serial.print((long)sensorValue, DEC);
      Serial.print(" batt: ");
      //Serial.print((long)battVoltage, DEC);
      Serial.print(buf[0]);
      Serial.print(buf[1]);
      Serial.print(buf[2]);
      Serial.print(buf[3]);
      Serial.print(buf[4]);
      //Serial.print("%s", (char*)ftoa(buf, f, 3));
      //Serial.print(" A0: ");
      //Serial.print(sensorValue, DEC);
      Serial.println();
      

    }
    skipcounter++;
  }

  if (currentMillis - previousMillisBlink >= (( 256 + 20 - sensorValue ) * 4)) {
    previousMillisBlink = currentMillis;
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    if (sensorValue == 0)
      ledState = LOW;
    else if (sensorValue == 255)
      ledState = HIGH;

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
    //bool anzLED = currentMillis % sensorValue;
    //digitalWrite(ledPin, anzLED);
  }


  /* // read the value from the sensor:
  //sensorValue = analogRead(sensorPin);
  // turn the ledPin on
  digitalWrite(ledPin, HIGH);
  // stop the program for <sensorValue> milliseconds:
  delay(sensorValue);
  // turn the ledPin off:
  digitalWrite(ledPin, LOW);
  // stop the program for for <sensorValue> milliseconds:
  delay(sensorValue);*/
/*    if (pwmState == LOW)
      pwmState = HIGH;
    else
      pwmState = LOW;
    digitalWrite(PWMout, pwmState); */

  analogWrite(ledPWMPin, sensorValue);
  // wait for 30 milliseconds to see the dimming effect
  //delay(30);
}
