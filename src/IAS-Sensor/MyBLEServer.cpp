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

BLECharacteristic BatteryLevelCharacteristic(BLEUUID((uint16_t) 0x2A19),
		BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor BatteryLevelDescriptor(BLEUUID((uint16_t) 0x2901));

class MyServerCallbacks: public BLEServerCallbacks {
	MyBLEServer *myServer_;
public:

	void onConnect(BLEServer *pServer) {
		myServer_->sendOnConnectMsg_ = true;
	}
	;

	void onDisconnect(BLEServer *pServer) {
	}
	MyServerCallbacks(MyBLEServer *myServer) :
			myServer_(myServer) {
	}

};

class MyCallbacks: public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic *pCharacteristic) {
		String rxValue = pCharacteristic->getValue();

		if (rxValue.length() > 0) {
			for (int i = 0; i < rxValue.length(); i++) {
				Serial.print(rxValue[i]);
			}

		}
	}
};

bool MyBLEServer::sendOnConnectMsg() {
	boolean rv = sendOnConnectMsg_;
	sendOnConnectMsg_ = false;
	if (rv)
		delay(2000);
	return rv;
}

MyBLEServer* MyBLEServer::create(const char *name, const char *onConnectMsg) {

	MyBLEServer *rv = new MyBLEServer(onConnectMsg);
	// Create the BLE Device
	BLEDevice::init(name);

	// Create the BLE Server
	rv->server_ = BLEDevice::createServer();
	rv->server_->setCallbacks(new MyServerCallbacks(rv));

	// BatteryService
	BLEService *batteryService = rv->server_->createService(BatteryService);
	batteryService->addCharacteristic(&BatteryLevelCharacteristic);
	BatteryLevelDescriptor.setValue("Percentage 0 - 100");
	BatteryLevelCharacteristic.addDescriptor(&BatteryLevelDescriptor);
	//BatteryLevelCharacteristic.addDescriptor(new BLE2902());

	rv->server_->getAdvertising()->addServiceUUID(BatteryService);
	batteryService->start();

	// Create the BLE Service
	BLEService *service = rv->server_->createService(SERVICE_UUID);

	// Create a BLE Characteristic
	rv->pTxCharacteristic_ = service->createCharacteristic(
			CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

	rv->pTxCharacteristic_->addDescriptor(new BLE2902());

	rv->pRxCharacteristic_ = service->createCharacteristic(
			CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

	rv->pRxCharacteristic_->setCallbacks(new MyCallbacks());

	// Start the service
	service->start();

	// Start advertising
	rv->server_->getAdvertising()->start();

	return rv;
}

bool MyBLEServer::isConnected() {
	return server_ ? server_->getConnectedCount() > 0 : false;
}

void MyBLEServer::send(const char *data) {
	if (isConnected()) {
		const char *p = ((sendOnConnectMsg()) ? onConnectMsg_.c_str() : data);
		pTxCharacteristic_->setValue((uint8_t*) p, strlen(p));
		pTxCharacteristic_->notify();
	}
}

void MyBLEServer::startAdvertising() {
	//loop();
	server_->startAdvertising();
}

void MyBLEServer::sendBatteryLevel(double voltage) {
	static long nextUpdateTime = millis();
	if(nextUpdateTime > millis()) {
		return;
	}

	nextUpdateTime = millis() + 5000L;
	uint8_t data =  voltage > 4.15 ? 0x64 :
					voltage > 3.85 ? 0x5A :
					voltage > 3.82 ? 0x50 :
					voltage > 3.67 ? 0x46 :
					voltage > 3.65 ? 0x3C :
					voltage > 3.64 ? 0x32 :
					voltage > 3.61 ? 0x28 :
					voltage > 3.55 ? 0x1E :
					voltage > 3.50 ? 0x14 :
					voltage > 3.40 ? 0x0A :
									 0x00 ;
	printf("Battery Level: %d %% <-> Voltage: %f\n", (int) data, voltage);
	BatteryLevelCharacteristic.setValue(&data, 1);
	BatteryLevelCharacteristic.notify();
}
