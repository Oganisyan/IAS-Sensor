#include <Arduino.h>
#include <math.h> 
#include "ASM5915.h"



ASM5915::ASM5915(TwoWire &wire) : 
lastInitTime_(0), wire_(wire), address_(0x28), ready_(false), calibration_(0.0)
{
}


void ASM5915::begin()
{
  double calibration = 0.;
  for(int i=0; i < CALIBRATION_COUNT; i++) {
    while(!isReady()){
      loop();
      delay(10);
    } 
    double p = getPressure();
    calibration += p;
  }
  if(calibration < 0.25 * CALIBRATION_COUNT)
    calibration_ = calibration/CALIBRATION_COUNT;
  Serial.printf("Calibration p =%f\n",calibration_);
}

void ASM5915::loop()
{
  if(millis() - lastInitTime_  < 25)
    return;
  lastInitTime_ =   millis();

  wire_.beginTransmission(address_);
  wire_.requestFrom(address_, (int)4);
  for(int i=0; i < 1000 && wire_.available() < 4; i++)
    delay(1);
  uint8_t data[4];
  for(int i = 0; i < 4; i++){
    data[i] = wire_.read();
  }

  uint16_t rawP=(data[0] & 0x3F) << 8 | data[1];
  uint16_t rawT=(data[2] << 8) | data[3];


  pressure_ = (rawP - 1638.0)/13107*5;
  temperature_ = ((rawT * 200.0) / 65536) - 50;
  ready_ = true;
}

double ASM5915::getTemperature()
{
  ready_ = false;
  return temperature_;
}



double  ASM5915::getPressure()
{
  ready_ = false;
  return pressure_ - calibration_;
}

boolean ASM5915::isReady()
{
  return ready_;
}
