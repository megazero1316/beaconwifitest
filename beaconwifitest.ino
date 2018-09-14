#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "BLEBeacon.h"
#include <ArduinoJson.h>
#include <WiFi.h>

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8))
String uuidBTN = "01122334-4556-6778-899a-abbccddeeff0";
int scanTime = 0; //In seconds
static int taskCore = 1;
unsigned long currentMillis = 0;
unsigned long currentMillis1 = 0;
unsigned long ReceivedMillis1 = 0;
unsigned long DiffMillis1 = 0;
const char* ssid     = "Xiaomi_57AF";
const char* password = "1234567890";

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (advertisedDevice.haveManufacturerData() == true) {
        std::string strManufacturerData = advertisedDevice.getManufacturerData();
        uint8_t cManufacturerData[100];
        strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);
        //        if (strManufacturerData.length() == 25 && String(advertisedDevice.getAddress().toString().c_str()).equals("d8:c5:43:18:ca:77"))

        if (strManufacturerData.length() == 25 )
        {
          BLEBeacon oBeacon = BLEBeacon();
          oBeacon.setData(strManufacturerData);
          String ss = oBeacon.getProximityUUID().toString().c_str();
          if (ss.equals(uuidBTN))
          {
            ReceivedMillis1 = millis();
            DiffMillis1 = ReceivedMillis1 - currentMillis1;
            Serial.printf("iBeacon Found Signal Received At %ld, and beacon scanned at %ld difference is %ld\n\r", currentMillis1, ReceivedMillis1, DiffMillis1);
          }
        }
      }
    }
};

void coreTask( void * pvParameters ) {

  String taskMessage = "Task running on core ";
  taskMessage = taskMessage + xPortGetCoreID();

  Serial.print("BLE Scanning...");
  Serial.print("\n\r");
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setWindow(48);
  pBLEScan->setInterval(58);
  pBLEScan->start(scanTime);
}



void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(
    coreTask,   /* Function to implement the task */
    "coreTask", /* Name of the task */
    10000,      /* Stack size in words */
    NULL,       /* Task input parameter */
    1,          /* Priority of the task */
    NULL,       /* Task handle. */
    taskCore);  /* Core where the task should run */
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.print("\n\r");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\n\r");
  Serial.print("WiFi connected");
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.print("\n\r");
currentMillis1 = millis();
}

void loop() {
  
  if ((millis()- currentMillis1 ) > 1800000)
  {
    Serial.print("Hello\n\r");
    currentMillis1 = millis();
  }
  delay(10);
}
