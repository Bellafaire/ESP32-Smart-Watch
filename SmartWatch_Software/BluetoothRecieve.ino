#include "BLEDevice.h"

//Most of this is taken from the BLE_client example provided in Examples > ESP32 BLE Arduino > BLE_client



//  w.println("Getting phone notifications");

//timer variable used to force exit the bluetooth recieve functions if data can't be obtained
static unsigned long bluetoothStart = 0;
static unsigned long bluetooth_timeout = 10000; //default 10 second time out

//UUID's for the services used by the android app (change as you please if you're building this yourself, just match them in the android app)
static BLEUUID serviceUUID("d3bde760-c538-11ea-8b6e-0800200c9a66");
static BLEUUID    charUUID("d3bde760-c538-11ea-8b6e-0800200c9a67");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

//static void notifyCallback(  BLERemoteCharacteristic* pBLERemoteCharacteristic,  uint8_t* pData,  size_t length,  bool isNotify) {
//  Serial.print("Notify callback for characteristic ");
//  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
//  Serial.print(" of data length ");
//  Serial.println(length);
//  Serial.print("data: ");
//  Serial.println((char*)pData);
//}

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
    }

    void onDisconnect(BLEClient* pclient) {
      connected = false;
#ifdef DEBUG
      Serial.println("onDisconnect");
#endif
      //screen is off so go back to sleep since we can't obtain notifications
      if (!deviceActive) {
#ifdef DEBUG
        Serial.println("device disconnected going to sleep");
#endif
        esp_deep_sleep_start();
      } else {
        //otherwise if the device disconnects go back to the home screen
        switchToHome();
      }
    }
};

String connectToServer(int timeout) {
  //manually clear the touchDetected flag. otherwise this function will exit (flag doesn't always clear)
  touchDetected = false;
  unsigned long startTime = millis();
  BLEClient*  pClient  = BLEDevice::createClient();

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  if (myDevice != NULL) {
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  } else {
    return "device not found";
  }
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {

    pClient->disconnect();
    return "connection error";
  }


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    pClient->disconnect();
    return "could not obtain characteristic";
  }
  //aquire a full 2048 bytes of data from the BLE device
  int byteCount = 0;

  //the android app requires a write to reset the cursor position of the notification feed.
  //this also causes the app to get the most current notifications available
  pRemoteCharacteristic->writeValue('a', 1);

  //we then have to wait until the app has obtained the current notifications. if the user
  //touches the screen exit the process right away since this is meant to happen in the background
  for (int a = 0; a < 100; a++) {
    delay(10);
    if (touchDetected) {
      return "touch detected";
    }
  }

  String ret = "";

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead()) {
    touchDetected = false;
    while (byteCount < 2048 && !touchDetected && startTime + timeout > millis()) {
      std::string value = pRemoteCharacteristic->readValue();
#ifdef DEBUG
      Serial.print(value.c_str());
#endif
      ret += value.c_str();
      byteCount += 16;
      //if we detect "***" then we know that the end of the data has been reached. no need in getting anything else
      if (ret[ret.length() - 1] == '*' && ret[ret.length() - 2] == '*' && ret[ret.length() - 3] == '*') {
        break;
      }
    }
  }

  connected = true;
  return ret;
}
/**
   Scan for BLE servers and find the first one that advertises the service we are looking for.
*/
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    /**
        Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (bluetoothStart + bluetooth_timeout < millis()) {
#ifdef DEBUG
        Serial.println("Search timeout reached, ending bluetooth scan");
#endif
        BLEDevice::getScan()->stop();
      }

#ifdef DEBUG
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());
#endif
      // We have found a device, let us now see if it contains the service we are looking for.
      //      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      if (advertisedDevice.isAdvertisingService(serviceUUID)) {

        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
        doScan = true;

      } // Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks


String getPhoneNotifications(int timeout) {
  
  bluetoothStart = millis();
  bluetooth_timeout = timeout;

  int currentPosition = 0;

  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 8 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1000);
  pBLEScan->setWindow(1000);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(8);


#ifdef DEBUG
  Serial.println("Scanning Complete");
#endif

 

  String rdata = connectToServer(timeout);

  //check that the message ends with *** otherwise we assume there was a timeout or something else went wrong
  if (rdata[rdata.length() - 1] == '*' && rdata[rdata.length() - 2] == '*' && rdata[rdata.length() - 3] == '*') {
    for (int a = 0; a < 2048; a++) {
      notificationData[a] = rdata[a];
    }
    return "Success";
  } else {
#ifdef DEBUG
    Serial.println("Data not complete: " + rdata);
#endif
    return "Data not complete: " + rdata;
  }

}
