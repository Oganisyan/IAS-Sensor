/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server_ that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE"
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server_ is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second.
*/
#include <MyBLEServer.h> 

/*
BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
//uint8_t buffer[] = "$PDGFTL1,2025,2000,250,-14,45,134,28,65,382,153*14\r\n";

//uint8_t buffer[] = "$LXWP0,,30,10,0,,,,,,,,*11\r\n";
uint8_t buffer[] = "$LXWP0,,31.5,,,,,,,,,,*3A\r\n";

uint8_t txValue = 0;
*/
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    //deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    //deviceConnected = false;
  }
};


class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
  }
};


MyBLEServer* MyBLEServer::create(const char *name) {

  MyBLEServer *rv = new MyBLEServer();
  // Create the BLE Device
  BLEDevice::init(name);

  // Create the BLE Server
  rv->server_ = BLEDevice::createServer();
  rv->server_->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *service = rv->server_->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  rv->pTxCharacteristic_ = service->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  rv->pTxCharacteristic_->addDescriptor(new BLE2902());

  rv->pRxCharacteristic_ = service->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

  rv->pRxCharacteristic_->setCallbacks(new MyCallbacks());

  // Start the service
  service->start();

  // Start advertising
  rv->server_->getAdvertising()->start();

  return rv;
}
  
