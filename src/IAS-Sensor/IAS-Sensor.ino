/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE"
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second.
*/
#define CONFIG_LOG_BOOTLOADER_LEVEL_NONE

#include <MyBLEServer.h> 
#include <ASM5915.h>
#include <KalmanFilter.h>
#include <PowerManager.h>


#define PIN_SDA 32
#define PIN_SCL 33
#define PIN_POW 22
#define PIN_BTN 19


#define calcRho true

#if 1
#define DBG(x)  x
#else
#define DBG(x)  
#endif

PowerManager *powerManager;

MyBLEServer *pServer = NULL;
ASM5915     diffBarometer(Wire, 0x28);
KalmanFilter   kfDiffPressure;
KalmanFilter   kfTemperature;

#define LPWX0_FOTMATER  "$LXWP0,,%.1f,,,,,,,,,,*%02X\n\r"

const char *getLXWP(double ias) {
  static char buffer[128];
  sprintf(buffer, LPWX0_FOTMATER, ias, 0x00);
  uint8_t check = 0;
  for(int i=1; i < 128 && buffer[i] != '*'; i++) {
    check^=(uint8_t) buffer[i];
  }
  sprintf(buffer, LPWX0_FOTMATER, ias, check);
  return buffer;
}

double calcIAS(double dp, double t) {
  double rv;

  double rho = calcRho ? 1013.25/(t+273.15)*0.348564471 : 1.225;  
  rv =(dp < 0)? 0 : sqrt(2*dp*100./rho);
  // Recalc m/s -> km/h
  rv *= 3.6; 
  return rv;  
}


void setup() {
  powerManager = PowerManager::create(PIN_POW, PIN_BTN);

  DBG(Serial.begin(115200);)
  // Create the BLE Device
  pServer = MyBLEServer::create("IAS Sensor");
  diffBarometer.begin(PIN_SDA, PIN_SCL);
  DBG(Serial.println("Waiting a client connection to notify...");)  
}

void loop() {
  powerManager->loop();
  DBG(Serial.printf("Diff: %08f Temp: %08f IAS: %02f\n", kfDiffPressure.get(), kfTemperature.get(),calcIAS(kfDiffPressure.get(), kfTemperature.get()));)
  
  if (pServer->isConnected()) {
    pServer->send(getLXWP(calcIAS(kfDiffPressure.get(), kfTemperature.get())));
  } else {
    pServer->startAdvertising();
  }
  for(int i=0; i < 50; i++) {
    diffBarometer.loop();
    if(diffBarometer.isReady()) {
      kfDiffPressure.update(diffBarometer.getPressure());
      kfTemperature.update(diffBarometer.getTemperature());
    } 

    delay(10);
  }
}
