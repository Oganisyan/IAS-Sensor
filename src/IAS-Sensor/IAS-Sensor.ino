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
#include <MS5611.h>
#include <MyUtil.h>
#include <KalmanFilter.h>
#include <PowerManager.h>


#define DEVICE_NAME   "IAS-Sensor"
#define SV_VERSION    1.0
#define HV_VERSION    1.0

#define LPWX0_FOTMATER  "$LXWP0,,%s,%s,%s,,,,,,,,*%02X\r\n"
#define LPWX1_FOTMATER  "$LXWP1,%s,%d,%.2f,%.2f*%02X\r\n"

#define PIN_SDA GPIO_NUM_32
#define PIN_SCL GPIO_NUM_33
#define PIN_POW GPIO_NUM_19
#define PIN_BTN GPIO_NUM_34


#if 1
#define DBG(x)  x
#else
#define DBG(x)  
#endif

PowerManager   *powerManager = NULL;
MyBLEServer    *pServer = NULL;
ASM5915        diffBarometer(Wire);
MS5611         barometer(Wire);
Calculator     calculator;
KalmanFilter   dKfP;
KalmanFilter   dKfT;
KalmanFilter   kfP;
KalmanFilter   kfT;

String d2s(double d) {
  return isnan(d) ? String() : String(d,1);
}

const char *getLXWP0(double p, double ias) {
  static char buffer[129];
  double alt;
  double vario;
  calculator.calc(p, alt, vario);
  snprintf(buffer, 128, LPWX0_FOTMATER, d2s(ias).c_str(), d2s(alt).c_str(), d2s(vario).c_str(), 0x00);
  snprintf(buffer, 128, LPWX0_FOTMATER, d2s(ias).c_str(), d2s(alt).c_str(), d2s(vario).c_str(), calcCheckSum(buffer));
  DBG(Serial.print(buffer));
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
  DBG(Serial.print(buffer));
  return buffer;
}

void setup() {
  Serial.begin(115200);
  powerManager = PowerManager::create(PIN_POW, PIN_BTN);
  // Create the BLE Device
  pServer = MyBLEServer::create("IAS Sensor", getLXWP1());
  Wire.begin(PIN_SDA, PIN_SCL);
  diffBarometer.begin();
  barometer.begin();
  DBG(Serial.println("Waiting a client connection to notify...");)
  uint8_t chipid[6];
}

void loop() {
  powerManager->loop();  
  for(int i=0; i < 25; i++) {
    diffBarometer.loop();
    if(diffBarometer.isReady()) {
      dKfP.update(diffBarometer.getPressure());
      dKfT.update(diffBarometer.getTemperature());
    } 
    barometer.loop();
    if(barometer.isReady()) {
      kfP.update(barometer.getPressure());
      kfT.update(barometer.getTemperature());
    } 
    delay(10);
  }
  if (pServer->isConnected()) {
    pServer->send(getLXWP0(kfP.get(), calcIAS(dKfP.get(), dKfT.get())));
  } else {
    pServer->startAdvertising();
  }

}
