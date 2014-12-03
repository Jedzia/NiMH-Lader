#ifndef HEADER_TREND
#define HEADER_TREND

class Average {
  public:
    void begin( void );
    void fill( int value );
    void clear( void );
    void update( int value );
    int average( void );
    float averagef( void );
  private:
    void increment( void );
    
    static const int AVGSIZE = 8;
    int avg[AVGSIZE];
    int avgp = 0;
    
    bool firstrun = true;
};

class Trend {
  public:
    void fill( int value );
    void clear( void );
    void update( int value );
    //float average( void );
  private:
    void increment( void );
    
    static const int TRENDSIZE = 64;
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
