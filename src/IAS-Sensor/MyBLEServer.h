#ifndef __BLE_SERVER_H__
#define __BLE_SERVER_H__

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UART UUIDs
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define BatteryService BLEUUID((uint16_t)0x180F)

class MyServerCallbacks;

class MyBLEServer {
	BLEServer *server_;
	BLECharacteristic *pTxCharacteristic_;
	BLECharacteristic *pRxCharacteristic_;
	std::string onConnectMsg_;
	boolean sendOnConnectMsg_;

	bool sendOnConnectMsg();

	MyBLEServer(const char *onConnectMsg) :
			onConnectMsg_(onConnectMsg), server_(NULL), pTxCharacteristic_(
					NULL), pRxCharacteristic_(NULL), sendOnConnectMsg_(false) {
	}

public:
	static MyBLEServer* create(const char *name, const char *onConnectMsg);

	bool isConnected();
	void send(const char *data);
	void startAdvertising();
	void sendBatteryLevel(double voltage);

	friend class MyServerCallbacks;
};

#endif
