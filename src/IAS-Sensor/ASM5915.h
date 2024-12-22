#ifndef  __ASM5915_H__
#define  __ASM5915_H__
#include <Wire.h>

#define CALIBRATION_COUNT 100

class ASM5915 
{
    TwoWire &wire_;
    int     address_;

    double temperature_;
    double pressure_;
    double calibration_;
    unsigned long  lastInitTime_;
    boolean ready_;
    
        
  public:
    ASM5915(TwoWire &wire);
    double getTemperature();
    double getPressure();
    void begin();
    void loop();
    boolean isReady();

};

#endif // __ASM5915_H__
