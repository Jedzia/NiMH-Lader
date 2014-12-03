#ifndef HEADER_TREND
#define HEADER_TREND

class Trend {
  public:
    void begin( void );
    void fill( int value );
    void erase( void );
    void update( int value );
    float average( void );
  private:
    void increment( void );
    static const int TRENDSIZE = 8;
    int trend[TRENDSIZE];
    int trendp = 0;
    bool firstrun = true;

};


//#include <stdarg.h>
// include the library code:
//#include <LiquidCrystal.h>

//char *ftoa(char *a, double f, int precision);

//void p(char *fmt, ... );
//void p(const __FlashStringHelper *fmt, ... );


#endif // HEADER_TREND
