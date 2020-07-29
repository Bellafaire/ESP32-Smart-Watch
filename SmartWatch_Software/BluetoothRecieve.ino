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
static boolean scanComplete = false;
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


bool connectToServer() {

#ifdef DEBUG
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());
#endif

  BLEClient*  pClient  = BLEDevice::createClient();

#ifdef DEBUG
  Serial.println(" - Created client");
#endif
  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  if (myDevice != NULL) {
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  } else {
    return false;
  }
#ifdef DEBUG
  Serial.println(" - Connected to server");
#endif
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
#ifdef DEBUG
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
#endif
    pClient->disconnect();
    return false;
  }
#ifdef DEBUG
  Serial.println(" - Found our service");
#endif

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
#ifdef DEBUG
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
#endif
    pClient->disconnect();
    return false;
  }

#ifdef DEBUG
  Serial.println(" - Found our characteristic");
#endif
  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
#ifdef DEBUG
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
#endif
  }

  connected = true;
  return true;
}


/*Please avoid using this function at all costs. it is required to start the BLE server
   But other than that it is only effective in obtaining notification data, under normal conditions use connectToServer()
*/
String connectToServer(int timeout, String command, boolean readDataBack, boolean touchInterruptable) {
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

#ifdef DEBUG
  Serial.println("Device found, connecting to BLE Server");
#endif

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {

#ifdef DEBUG
    Serial.println("Null device found, exiting connectToServer()");
#endif

    pClient->disconnect();
    return "connection error";
  }


  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
#ifdef DEBUG
    Serial.println("Null Characteristic, exiting connectToServer()");
#endif

    pClient->disconnect();
    return "could not obtain characteristic";
  }
  //aquire a full 2048 bytes of data from the BLE device
  int byteCount = 0;

  //the android app requires a write to reset the cursor position of the notification feed.
  //this also causes the app to get the most current notifications available
  pRemoteCharacteristic->writeValue(command.c_str(), command.length());

#ifdef DEBUG
  Serial.println("Wrote Characteristic: " + command);
#endif

  //if we don't actually need to read any data then we're done here
  if (!readDataBack) {
#ifdef DEBUG
    Serial.println("No Data Read Required");
#endif
    return "";
  }

  unsigned long dataReceiveStart = millis();

  String ret = "";

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead()) {
    touchDetected = false;
    while (byteCount < 2048 && startTime + timeout > millis()) {
      std::string value = pRemoteCharacteristic->readValue();
#ifdef DEBUG
      Serial.print(value.c_str());
#endif
      String strVal = value.c_str();
      if (strVal.substring(0, 4).equals("null")) {
        //do nothing while
        delay(10);
      } else {
        ret += strVal;
        byteCount += 16;
        //if we detect "***" then we know that the end of the data has been reached. no need in getting anything else
        if (ret[ret.length() - 1] == '*' && ret[ret.length() - 2] == '*' && ret[ret.length() - 3] == '*') {
          break;
        }
      }
    }
  }


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
//        connected = true;
      } // Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks

void initBluetooth() {
  //attempt to connect to the device on startup
  xTaskCreate( xFindDevice, "FIND_DEVICE", 4096, (void *) 1 , tskIDLE_PRIORITY, &xConnect );
  configASSERT( xConnect );
}


void findDevice() {
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
  scanComplete = true;
}

void xFindDevice(void * pvParameters ) {
  BLEDevice::init("");
#ifdef DEBUG
      Serial.println("%%% Find Device Task Launched %%%");
#endif
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 8 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1000);
  pBLEScan->setWindow(1000);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(8);
  scanComplete = true;
  connectToServer();

#ifdef DEBUG
      Serial.println("%%% Fully connected to BLE GATT Server %%%");
#endif

    vTaskDelete(NULL);
}


String writeBLE(int timeout, String command, boolean readDataBack) {

  bluetoothStart = millis();
  bluetooth_timeout = timeout;

  int currentPosition = 0;

  if (!connected) {
    findDevice();
  }

  String rdata = connectToServer(timeout, command, readDataBack, false);
  return rdata;
}

String getPhoneNotifications(int timeout) {

  bluetoothStart = millis();
  bluetooth_timeout = timeout;

  int currentPosition = 0;

  if (!connected) {
    findDevice();
  }


#ifdef DEBUG
  Serial.println("Scanning Complete");
#endif

  String rdata = connectToServer(timeout, "/notifications", true, true);

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
