
#include "trend.h"
#include "Arduino.h"

void Average::begin( void ) {
  Serial.println( "Called MyClass::begin" );
}

void Average::clear( void ) {
  fill(0);
}

void Average::fill( int value ) {
  for (int i = 0; i < AVGSIZE; i++)
  {
    avg[i] = value;
  }
}


void Average::increment( void ) {
  if (avgp < AVGSIZE - 1)
    avgp++;
  else
    avgp = 0;
}

void Average::update( int value ) {

  if (firstrun)
  {
    fill(value);
    firstrun = false;
  }
  avg[avgp] = value;
  increment();
}


int Average::average( void ) {
  long trsum = 0;
  for (int i = 0; i < AVGSIZE; i++)
  {
    trsum = trsum + avg[i];
    //Serial.print(F("|trend: "));
    //Serial.print(trend[i], DEC);
  }

  return (trsum) / (AVGSIZE);
}

float Average::averagef( void ) {
  long trsum = 0;
  for (int i = 0; i < AVGSIZE; i++)
  {
    trsum = trsum + avg[i];
  }

  return (trsum) / ((float)AVGSIZE);
}


void Trend::clear( void ) {
  fill(0);
}

void Trend::fill( int value ) {
  for (int i = 0; i < TRENDSIZE; i++)
  {
    trend[i] = value;
  }
}

void Trend::update( int value ) {

  if (firstrun)
  {
    fill(value);
    firstrun = false;
  }
  trend[trendp] = value;
  increment();
}

void Trend::increment( void ) {
  if (trendp < TRENDSIZE - 1)
    trendp++;
  else
    trendp = 0;
}


/*int bla(int x)
{
    return 2*x;
}*/

/*extern LiquidCrystal lcd;

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


void p(char *fmt, ... ){
        char buf[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(buf, 128, fmt, args);
        va_end (args);
        lcd.print(buf);
}

void p(const __FlashStringHelper *fmt, ... ){
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt);
#ifdef __AVR__
  vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
#else
  vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
#endif
  va_end(args);
  lcd.print(buf);
}*/


