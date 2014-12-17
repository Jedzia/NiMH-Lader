
/*


 Created by Jedzia

 v01b0c831ec2e85947132d056214242f15cb9296d
 This example code is in the public domain.
 http://www.forum64.de/wbb3/board36-sonstige-themen/board40-laberecke/board76-forum-64-spezial/board78-das-erweiterte-forum/59147-chaosloader/

 This code is dedicated to Dita von Theese.

 The comments may lead you in the wrong direction, this is an early bird
 */
#define DEBUG_I


#include <Time.h>
#include <Bounce2.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "trend.h"

#define PWMMAX 90        // restrict pwm max-value for testing.

#define sensorPin A0    // Battery Voltage
#define sensorPin2 A1    // Battery charge current
#define sensorPin3 A2    // Battery discharge current

#define chargeLed1Pin 13      // select the pin for the LED
#define chargePWM1Pin 3    // select the pin for the LED
#define dischargePWM1Pin 10    // select the pin for the LED
#define chargeLed2Pin 5      // select the pin for the LED
#define chargePWM2Pin 11    // select the pin for the LED
#define dischargePWM2Pin 9    // select the pin for the LED
//#define PWMout 12

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
// UI buttons
#define butDown     4
#define butUp       7
#define butStart    8
#define butTest    12


const float CHARGE_ENDVOLTAGE = 1.54;
const float DELTACHECK_STARTVOLTAGE = 1.40;
const float DISCHARGE_ENDVOLTAGE = 1.02;
//const float TREND_DELTA_MAX = -0.0005;
const float TREND_DELTA_MAX = 0.0012;
const int TREND_DELTA_MAX_COUNT = 20;
const float TEMP_DELTA_MAX = 0.59;

//const float REF = 5.00;
const float REF = 2.245;
const long interval = 139;

//                            display / real
const float realCurrentNorm = 0.280 / 0.250;
const float realCurrentDisNorm = 0.360 / 0.698;

float currentSet = 0.5;
float maxVoltage = 0;
float fbattDisCurrent = 0;

unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long previousMillisBlink = 0;
unsigned long previousMeasureTime = 0;
unsigned long lowMeasureTime = 0;
float chargeCapacity = 0;

int skipcounter = 0;
int trendDeltaMaxCounter = 0;
//int sensorValue = 128;  // variable to store the value coming from the sensor
int sensorValue = 0;  // variable to store the value coming from the sensor
int battVoltage = 0;
int battLoadVoltage = 0;
int battCurrent = 0;
int battDisCurrent = 0;

//const int TRENDSIZE = 8;
//int trend[TRENDSIZE];
//int trendp = 0;

// Debouncers
Bounce debDown  = Bounce();
Bounce debUp    = Bounce();
Bounce debStart    = Bounce();
Bounce debTest    = Bounce();
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

Average average;
Trend trend;
Trend trendTemp;

bool ledState = LOW;
//bool dischargeState = LOW;

//bool runCharge = false;
//bool pwmState = LOW;


enum AppState {
  Startup,
  Running,
  Charging,
  Discharging,
  Finished
} appState;

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

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void printDigitsWithColon(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  printDigits(digits);
}

void setup() {

  analogReference(EXTERNAL);

  //setPwmFrequency(chargePWM1Pin, 1);
  TCCR2A = 0x23;
  TCCR2B = 0x09;  // select timer2 clock as unscaled 16 MHz I/O clock
  OCR2A = 159;  // top/overflow value is 159 => produces a 100 kHz PWM
  pinMode(chargePWM1Pin, OUTPUT);  // enable the PWM output (you now have a PWM signal on digital pin 3)

  
  // declare the chargeLed1Pin as an OUTPUT:
  pinMode(chargeLed1Pin, OUTPUT);
  pinMode(dischargePWM1Pin, OUTPUT);


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

  average.clear();
  trend.clear();
  trendTemp.clear();

  // Start up the library
  sensors.begin();
  sensors.setResolution(11);

  Serial.begin(9600);
  Serial.println(F("One Wire Bus Mode is:"));
  if(sensors.isParasitePowerMode()== true)
  Serial.println(F("in ParasitePower Mode."));
  else
  Serial.println(F("in normal Powered Mode."));
  
}

void loop() {

  unsigned long currentMillis = millis();
  debDown.update();
  debUp.update();
  debStart.update();
  debTest.update();

  if (debTest.fell())
  {
    if (appState != Discharging)
    {
      //dischargeState = true;
      appState = Discharging;
      //runCharge = false;
      sensorValue = 0;
      average.clear();
      trend.clear();
      trendTemp.clear();
      chargeCapacity = 0;
      previousMeasureTime = millis();
      Serial.println(F("Discharging..."));
    }
    else
    {
      //dischargeState = false;
      appState = Running;
      Serial.println(F("Stop Discharge"));
    }
  }

  if ((appState == Charging) && debStart.fell())
  {
    sensorValue = 0;
    //runCharge = false;
    appState = Running;
    Serial.println(F("STOP"));
  }
  else if (!(appState == Charging) && debStart.fell())
  {
    appState = Charging;
    Serial.println(F("START"));
    average.clear();
    trend.clear();
    trendTemp.clear();
    maxVoltage = 0;
    chargeCapacity = 0;
    previousMeasureTime = millis();
  }

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    if (appState == Charging)
    {
      if (!debDown.read() && (currentSet >= 0.05))
      {
        currentSet -= 0.05;
#ifdef DEBUG_I
        Serial.print(F("ISet: "));
        Serial.println(currentSet, 3);
#endif
      }
      else if (!debUp.read() && (currentSet <= (REF - 0.20)))
      {
        currentSet += 0.05;
#ifdef DEBUG_I
        Serial.print(F("ISet: "));
        Serial.println(currentSet, 3);
#endif
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
      else if (!debUp.read() && (sensorValue < PWMMAX))
      {
        sensorValue++;
        Serial.print(F("PWM: "));
        Serial.println(sensorValue, DEC);
      }

    }

    float f = 0;
    delay(1);
    battLoadVoltage = analogRead(sensorPin);
    delay(1);
    battCurrent = analogRead(sensorPin2);
    float fbattCurrent = REF / 1024 * battCurrent;

    if ( ( skipcounter % 32 == 0 ) /*&& Serial.available()*/) {
      delay(1);
      battDisCurrent = analogRead(sensorPin3);
      fbattDisCurrent = REF / 1024 * battDisCurrent;
    }

    if ( ( skipcounter % 4 == 0 ) /*&& Serial.available()*/) {

      f = REF / 1024 * battCurrent;
      if (appState == Charging)
      {
        float diff = f - currentSet;
        //float abso = abs(diff);
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
          if (diff >= 0.05 )
          {
            sensorValue--;
#ifdef DEBUG_PWM
            Serial.print(F("PWM--: "));
            Serial.print((long)sensorValue, DEC);
            Serial.print(F("|diff: "));
            Serial.println(diff, 3);
#endif
          }
          else if (diff <= -0.05 )
          {
            sensorValue++;
#ifdef DEBUG_PWM
            Serial.print(F("PWM++: "));
            Serial.print((long)sensorValue, DEC);
            Serial.print(F("|diff: "));
            Serial.println(diff, 3);
#endif
          }
          else
          {
            //Serial.println(F("No Change"));
          }
        }

        if (sensorValue < 0)
        {
          sensorValue = 0;
        }
        else if (sensorValue > PWMMAX)
        {
          sensorValue = PWMMAX;
        }

      }
    }

    if ( ( skipcounter >= 64 ) /*&& Serial.available()*/) {
      skipcounter = 0;

      delay(1);

      lowMeasureTime = millis();
      digitalWrite(dischargePWM1Pin, LOW);
      analogWrite(chargePWM1Pin, 0);
      sensors.requestTemperatures();
      float temp1 = sensors.getTempCByIndex(0);
      float temp2 = sensors.getTempCByIndex(1);
      if (appState == Discharging)
        delay(100);
      else
        delay(300 + sensorValue * 2);
      battVoltage = analogRead(sensorPin);
      average.update(battVoltage);
      float averageBatt = REF / 1024 * average.averagef();
      trend.update(averageBatt);
      float vtrend = trend.gettrend();
      delay(1);

      lowMeasureTime = millis() - lowMeasureTime;
      
      /*Serial.print(F("trendp="));
      Serial.print(trendp, DEC);
      Serial.print(F(" trend[0]="));
      Serial.print(trend[0], DEC);
      Serial.print(F(" trend[1]="));
      Serial.print(trend[1], DEC);
      Serial.println("");*/

      float fbattLoadVoltage = REF / 1024 * battLoadVoltage;
      float realCurrent = fbattCurrent * realCurrentNorm; // in A
      float realDisCurrent = fbattDisCurrent * realCurrentDisNorm; // in A
      //Serial.print(F("rc: "));
      //Serial.print(realCurrent, 3);
      f = REF / 1024 * battVoltage;

      if (appState == Charging)
      {

        unsigned long curMeasTime = millis();
        unsigned long difftime = curMeasTime - previousMeasureTime - lowMeasureTime;

        float curCapCount = /*fbattLoadVoltage **/ realCurrent * ( difftime / 1000.0 ); // in As
        //Serial.print(F(" CapC: "));
        //Serial.print(curCapCount, 3);

        chargeCapacity += curCapCount * 1000.0 / 60.0 / 60.0; // mAh

        previousMeasureTime = curMeasTime;

        if (f >= CHARGE_ENDVOLTAGE )
        {
          //runCharge = false;
          appState = Running;
          //dischargeState = false;
          sensorValue = 0;
          Serial.println(F("Reached End-Voltage."));
        }

        if (trend.isValid())
        {
          if (averageBatt > maxVoltage )
          {
            maxVoltage = averageBatt;
          }
          if (averageBatt < (maxVoltage - 0.005) )
          {
            //runCharge = false;
            appState = Running;
            //dischargeState = false;
            sensorValue = 0;
            Serial.println(F("Max End-Voltage and 5mv back."));
          }

          if ((f > DELTACHECK_STARTVOLTAGE) && ( vtrend < TREND_DELTA_MAX ))
          {
            trendDeltaMaxCounter++;
            if (trendDeltaMaxCounter > TREND_DELTA_MAX_COUNT)
            {
              //runCharge = false;
              appState = Running;
              //dischargeState = false;
              sensorValue = 0;
              Serial.println(F("Delta Voltage shut off."));
            }
          }
          else
          {
            trendDeltaMaxCounter = 0;
          }
        }

        if (( temp2 - temp1) > 7.0 )
        {
          //runCharge = false;
          appState = Running;
          //dischargeState = false;
          sensorValue = 0;
          Serial.println(F("Max Temperature shut off."));
        }
        if (trendTemp.isValid() && ( trendTemp.gettrend() > TEMP_DELTA_MAX ))
        {
          //runCharge = false;
          appState = Running;
          //dischargeState = false;
          sensorValue = 0;
          Serial.println(F("Delta Temperature shut off."));
        }

      }
      if (appState == Discharging)
      {

        unsigned long curMeasTime = millis();
        unsigned long difftime = curMeasTime - previousMeasureTime - lowMeasureTime;

        float curCapCount = /*fbattLoadVoltage **/ realDisCurrent * ( difftime / 1000.0 ); // in As
        //Serial.print(F(" CapC: "));
        //  Serial.print(curCapCount, 3);

        chargeCapacity += curCapCount * 1000.0 / 60.0 / 60.0; // mAh

        previousMeasureTime = curMeasTime;




        if (f < DISCHARGE_ENDVOLTAGE)
        {
          appState = Running;
          //runCharge = false;
          //dischargeState = false;
          Serial.println(F("Discharge LOW-Voltage reached."));
          sensorValue = 0;
        }
      }
      //char buf[8];
      //ftoa(buf, f, 3);


      // digital clock display of the time
      //Serial.print(hour(),2);
      printDigits(hour());
      printDigitsWithColon(minute());
      printDigitsWithColon(second());
      Serial.print(",");
      Serial.print(day());
      Serial.print(",");
      Serial.print(month());
      Serial.print(",");
      Serial.print(year());
      Serial.print("|");
      //Serial.println();


      if (appState == Discharging)
      {
        Serial.print(F("DIS|"));
      }
      else if (appState == Charging)
      {
        Serial.print(F("CHG|"));
      }
      else
      {
        Serial.print(F("OFF|"));
      }

      Serial.print(F("PWM: "));
      Serial.print((long)sensorValue, DEC);
      Serial.print(F("|Batt: "));
      Serial.print(f, 3);
      Serial.print(F("|Load: "));
      Serial.print(fbattLoadVoltage, 3);
      //Serial.print((long)battVoltage, DEC);
      //Serial.print(buf[0]);
      //Serial.print(buf[1]);
      //Serial.print(buf[2]);
      //Serial.print(buf[3]);
      //Serial.print(buf[4]);
      //Serial.print("%s", (char*)ftoa(buf, f, 3));
      //if (appState == Charging)
//      {
//        float diff = fbattCurrent - currentSet;
//        Serial.print(F("|diff: "));
//        Serial.print(diff, 3);
//        //float abso = abs(diff);
//      }
      Serial.print(F("|Iset: "));
      Serial.print(currentSet, 3);
      Serial.print(F("|Ichg: "));
      Serial.print(fbattCurrent, 3);
      //Serial.print(F("|Ichg: "));
      //Serial.print(battCurrent, DEC);
      //Serial.print(F("|IchgR: "));
      Serial.print(F("|"));
      Serial.print(fbattCurrent*realCurrentNorm, 3);


      Serial.print(F("|Idis: "));
      Serial.print(fbattDisCurrent, 3);
      //Serial.print(F("|Idis: "));
      //Serial.print(fbattDisCurrent*realCurrentDisNorm, 3);

      Serial.print(F("|Bavg: "));
      Serial.print(averageBatt, 3);

      Serial.print(F("|Trnd: "));
      Serial.print(vtrend, 4);
      Serial.print(F("|Tmax: "));
      Serial.print(trendDeltaMaxCounter, DEC);

      Serial.print(F("|Temp1: "));
      Serial.print(temp1);
      Serial.print(F("|Temp2: "));
      Serial.print(temp2);

      trendTemp.update(temp2 - temp1);
      Serial.print(F("|TempT: "));
      Serial.print(trendTemp.gettrend());

      Serial.print(F("|Capa: "));
      Serial.print(chargeCapacity, 1);

      Serial.print(F("|lowM: "));
      Serial.print(lowMeasureTime, DEC);

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
    else if (sensorValue == PWMMAX)
      ledState = HIGH;

    // set the LED with the ledState of the variable:
    digitalWrite(chargeLed1Pin, ledState);
    //bool anzLED = currentMillis % sensorValue;
    //digitalWrite(chargeLed1Pin, anzLED);
  }


  /* // read the value from the sensor:
  //sensorValue = analogRead(sensorPin);
  // turn the chargeLed1Pin on
  digitalWrite(chargeLed1Pin, HIGH);
  // stop the program for <sensorValue> milliseconds:
  delay(sensorValue);
  // turn the chargeLed1Pin off:
  digitalWrite(chargeLed1Pin, LOW);
  // stop the program for for <sensorValue> milliseconds:
  delay(sensorValue);*/
  /*    if (pwmState == LOW)
        pwmState = HIGH;
      else
        pwmState = LOW;
      digitalWrite(PWMout, pwmState); */

  if (appState == Charging)
  {
    analogWrite(chargePWM1Pin, sensorValue);
    //dischargeState = false;
    //appState=Running;
  }
  else
  {
    //analogWrite(chargePWM1Pin, 0);
    analogWrite(chargePWM1Pin, sensorValue);
  }


  // security check pwm output
  if (sensorValue < 0)
  {
    sensorValue = 0;
  }
  else if (sensorValue > PWMMAX)
  {
    sensorValue = PWMMAX;
  }

  if (sensorValue > 0 && (appState == Discharging))
  {
    //dischargeState = false;
    appState = Running;
  }

  if (appState == Discharging)
  {
    digitalWrite(dischargePWM1Pin, HIGH);
  }
  else
  {
    digitalWrite(dischargePWM1Pin, LOW);
  }


  // wait for 30 milliseconds to see the dimming effect
  //delay(30);
}
