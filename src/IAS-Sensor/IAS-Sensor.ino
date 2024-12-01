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


#define DEVICE_NAME   "IAS-Sensor"
#define SV_VERSION    1.0
#define HV_VERSION    1.0

#define PIN_SDA 32
#define PIN_SCL 33
#define PIN_POW 22
#define PIN_BTN 34


#define calcRho true

#if 0
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
#define LPWX1_FOTMATER  "$LXWP1,%s,%d,%.2f,%.2f*%02X\n\r"



uint8_t calcCheckSum(char *data) {
  uint8_t check = 0;
  for(char* p=(data+1); *p != '\0' && *p != '*'; p++) {
    check^=(uint8_t) *p;
  }
  return check;  
}

const char *getLXWP0(double ias) {
  static char buffer[129];
  snprintf(buffer, 128, LPWX0_FOTMATER, ias, 0x00);
  snprintf(buffer, 128, LPWX0_FOTMATER, ias, calcCheckSum(buffer));
  return buffer;
}

const char *getLXWP1() {
  static char buffer[129];
  uint32_t    serialNumber = 0;
  for (int i = 0; i < 17; i = i + 8) {
    serialNumber |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  snprintf(buffer, 128, LPWX1_FOTMATER, DEVICE_NAME, serialNumber, SV_VERSION, HV_VERSION, 0x00);
  snprintf(buffer, 128, LPWX1_FOTMATER, DEVICE_NAME, serialNumber, SV_VERSION, HV_VERSION, calcCheckSum(buffer));
  return buffer;
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




void setup() {
  Serial.begin(115200);
  powerManager = PowerManager::create(PIN_POW, PIN_BTN);
  // Create the BLE Device
  pServer = MyBLEServer::create("IAS Sensor", getLXWP1());
  diffBarometer.begin(PIN_SDA, PIN_SCL);
  DBG(Serial.println("Waiting a client connection to notify...");)
  uint8_t chipid[6];
}

void loop() {
  powerManager->loop();
  DBG(Serial.printf("Volt: %f Diff: %08f Temp: %08f IAS: %02f\n", powerManager->get(), \
   kfDiffPressure.get(), kfTemperature.get(),calcIAS(kfDiffPressure.get(), kfTemperature.get()));)
  
  /*if (pServer->isNewDeviceConnected()) {
    pServer->send(getLXWP1());
  } else */if (pServer->isConnected()) {
    pServer->send(getLXWP0(calcIAS(kfDiffPressure.get(), kfTemperature.get())));
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
