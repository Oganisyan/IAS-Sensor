#ifndef __BLE_SERVER_H__
#define __BLE_SERVER_H__

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


class MyBLEServer {
  BLEServer *server_; 
  BLECharacteristic *pTxCharacteristic_;
  BLECharacteristic *pRxCharacteristic_;
  
  int nextBufferPos(int pos, int max) {
    if(++pos > max) {
      pos%=(max);
    }
    return pos;
  }

  MyBLEServer() : server_(NULL), pTxCharacteristic_(NULL)
  {}
  
public:
  static MyBLEServer *create(const char *name);
  

  bool isConnected() {
    return server_ ? server_->getConnectedCount() > 0 : false;
  }

  void send(const char *data) {
    if (isConnected()) {
      uint8_t *p=(uint8_t * )data;
      pTxCharacteristic_->setValue(p, strlen(data));
      pTxCharacteristic_->notify();
    }
  }


  void startAdvertising() {
    server_->startAdvertising();
  }
};



#endif
