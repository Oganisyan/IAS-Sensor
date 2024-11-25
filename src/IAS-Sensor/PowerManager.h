#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_
#include <Arduino.h>

RTC_DATA_ATTR boolean switchOnOff = true;
RTC_DATA_ATTR int lastBtnState = LOW;

class PowerManager {
  int pow_;
  int btn_;
  
  PowerManager(int pow, int btn) : pow_(pow), btn_(btn)
  {
    init();
  }

  void init(){
    pinMode(btn_, INPUT);
    loop();
    pinMode(pow_, OUTPUT);
    digitalWrite(pow_, HIGH);
  }


  void shutdown() {
    esp_sleep_enable_timer_wakeup(1000000);
    esp_deep_sleep_start();
  }


public:
  static PowerManager *create(int pow, int btn) {
    return new PowerManager(pow, btn);
  }
  
  void loop()
  {
    int btnState = digitalRead(btn_);
    if(btnState == LOW && lastBtnState == HIGH) {
      switchOnOff = ! switchOnOff;
    }
    lastBtnState = btnState;
    if(!switchOnOff)
      shutdown();
  }
};


#endif //_POWER_MANAGER_H_
