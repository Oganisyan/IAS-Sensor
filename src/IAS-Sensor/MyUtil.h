#ifndef __MY_UTIL_H__
#define __MY_UTIL_H__
#include <Arduino.h> 

#define calcRho true

double altitude(double p, const double qnh = 1013.25) {
    double A = p / qnh;
    double B = 0.1902630958088845; //1./5.25588;
    double C = pow(A,B);
    C = 1 - C;
    C = C /0.0000225577;
    return C; 
}

double calcIAS(double dp, double t) {
  double rv;

  double rho = calcRho ? 1013.25/(t+273.15)*0.348564471 : 1.225;  
  rv =(dp < 0)? 0 : sqrt(2*dp*100./rho);
  // Recalc m/s -> km/h
  rv *= 3.6; 
  rv *= 1.5;
  return rv;  
}

uint8_t calcCheckSum(char *data) {
  uint8_t check = 0;
  for(char* p=(data+1); *p != '\0' && *p != '*'; p++) {
    check^=(uint8_t) *p;
  }
  return check;  
}



class Calculator {
  uint32_t lastTime_;
  double   lastAlt_;
  
public:
  Calculator() : lastTime_(0), lastAlt_(0.0) {};

  void calc(double p, double &alt, double &vario) {
    uint32_t time = millis();
    alt = altitude(p);
    if(lastTime_ != 0) {
      vario = (alt - lastAlt_) * 1000 / (time-lastTime_);
    }
    lastAlt_ = alt;
    lastTime_ = time;
  }
};

#endif  //__MY_UTIL_H__
