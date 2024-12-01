#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_
#include <Arduino.h>



class PowerManager {
  uint8_t pow_;     // Power LED
  uint8_t btn_;     // switch button
  uint8_t pin_;     // BAT_ADC
  int vref_;
  
  PowerManager(int pow, int btn);
  void init();
  void shutdown();


public:
  static PowerManager *create(int pow, int btn);
  double  get();
  void   loop();
};


#endif //_POWER_MANAGER_H_
