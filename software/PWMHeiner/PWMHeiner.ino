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
#define sensorPin2 A1    // select the input pin for the potentiometer
#define ledPin 13      // select the pin for the LED
#define ledPWMPin 3    // select the pin for the LED
//#define PWMout 12

#define butDown     4
#define butUp       5
#define butStart    6

Bounce debDown  = Bounce();
Bounce debUp    = Bounce();
Bounce debStart    = Bounce();

const long interval = 100;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long previousMillisBlink = 0;
int skipcounter = 0;
//int sensorValue = 128;  // variable to store the value coming from the sensor
int sensorValue = 0;  // variable to store the value coming from the sensor
int battVoltage = 0;
int battLoadVoltage = 0;
int battCurrent = 0;
float currentSet = 1.0;

bool ledState = LOW;

bool runCharge = false;
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
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if (pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if (pin == 3 || pin == 11) {
    switch (divisor) {
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
  long p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

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

  //setPwmFrequency(ledPWMPin, 1);
  TCCR2A = 0x23;
  TCCR2B = 0x09;  // select timer2 clock as unscaled 16 MHz I/O clock
  OCR2A = 159;  // top/overflow value is 159 => produces a 100 kHz PWM
  pinMode(ledPWMPin, OUTPUT);  // enable the PWM output (you now have a PWM signal on digital pin 3)

  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);

  pinMode(butDown, INPUT_PULLUP);
  debDown.attach(butDown);
  debDown.interval(25);

  pinMode(butUp, INPUT_PULLUP);
  debUp.attach(butUp);
  debUp.interval(25);

  pinMode(butStart, INPUT_PULLUP);
  debStart.attach(butStart);
  debStart.interval(25);

  Serial.begin(9600);
}

void loop() {

  unsigned long currentMillis = millis();
  debDown.update();
  debUp.update();
  debStart.update();

  if ((runCharge == true) && debStart.fell())
  {
    sensorValue = 0;
    runCharge = false;
    Serial.println(F("STOP"));
  } else if ((runCharge == false) && debStart.fell())
  {
    runCharge = true;
    Serial.println(F("START"));
  }

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    if (runCharge == true)
    {

      if (!debDown.read() && (currentSet >= 0.05))
      {
        currentSet -= 0.05;
        Serial.print(F("ISet: "));
        Serial.println(currentSet, 3);
      }
      else if (!debUp.read() && (currentSet <= 4.80))
      {
        currentSet += 0.05;
        Serial.print(F("ISet: "));
        Serial.println(currentSet, 3);
      }
    }
    else
    {
      if (!debDown.read() && (sensorValue > 0))
      {
        sensorValue--;
        Serial.print(F("PWM: "));
        Serial.println(sensorValue, DEC);
      }
      else if (!debUp.read() && (sensorValue < 255))
      {
        sensorValue++;
        Serial.print(F("PWM: "));
        Serial.println(sensorValue, DEC);
      }

    }

    if ( ( skipcounter > 64 ) /*&& Serial.available()*/) {
      skipcounter = 0;

      delay(30);
      battLoadVoltage = analogRead(sensorPin);
      delay(30);
      battCurrent = analogRead(sensorPin2);

      analogWrite(ledPWMPin, 0);
      delay(300);
      battVoltage = analogRead(sensorPin);
      float f = 5.0 / 1024 * battVoltage;

      if (f >= 1.45 || f < 0.7)
      {
        runCharge = false;
        sensorValue = 0;
      }

      //char buf[8];
      //ftoa(buf, f, 3);

      if (runCharge == true)
      {
        Serial.print(F("ON |"));

      }
      else
      {
        Serial.print(F("OFF|"));
      }

      Serial.print(F("PWM: "));
      Serial.print((long)sensorValue, DEC);
      Serial.print(F(", Batt: "));
      Serial.print(f, 3);
      f = 5.0 / 1024 * battLoadVoltage;
      Serial.print(F(", Load: "));
      Serial.print(f, 3);
      //Serial.print((long)battVoltage, DEC);
      //Serial.print(buf[0]);
      //Serial.print(buf[1]);
      //Serial.print(buf[2]);
      //Serial.print(buf[3]);
      //Serial.print(buf[4]);
      //Serial.print("%s", (char*)ftoa(buf, f, 3));
      f = 5.0 / 1024 * battCurrent;
      if (runCharge == true)
      {
        float diff = f - currentSet;
        Serial.print(F(", diff: "));
        Serial.print(diff, 3);
        float abso = abs(diff);
        /*if (abso > 0.3)
        {
          if (diff >= 0.3 )
          {
            sensorValue -= abso * 20;
          }
          else
          {
            sensorValue += abso * 20;
          }
        }
        else*/
        {
          if (diff >= 0.1 )
          {
            sensorValue--;
          }
          else
          {
            sensorValue++;
          }
        }

        if (sensorValue < 0)
        {
          sensorValue = 0;
        }
        else if (sensorValue > 255)
        {
          sensorValue = 255;
        }

      }
      Serial.print(F(", ISet: "));
      Serial.print(currentSet, 3);
      Serial.print(F(", I: "));
      Serial.print(f, 3);
      Serial.println();


    }
    else
    {
      //delay(30 * 3);
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

    if (sensorValue == 0 /*|| runCharge == false*/)
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

  if (runCharge == true)
  {
    analogWrite(ledPWMPin, sensorValue);
  }
  else
  {
    //analogWrite(ledPWMPin, 0);
    analogWrite(ledPWMPin, sensorValue);
  }


  // wait for 30 milliseconds to see the dimming effect
  //delay(30);
}
