/*****************************************************************************
  The MIT License (MIT)
  Copyright (c) 2020 Matthew James Bellafaire
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
******************************************************************************/

/* Callbacks */
//callback called when an advertised device is found
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.println("BLE Advertised Device found: " + String(advertisedDevice.toString().c_str()));
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        Serial.println("%%%%%%%%%%%% Device Found %%%%%%%%%%%%%%%%%");
      }
    }
};

//client client callback which responds to connection related events
class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
      connected = true;
    }

    void onDisconnect(BLEClient* pclient) {
      connected = false;
      //      myDevice = NULL;
      Serial.println("%%%%%%%%%% Device has Disconnected %%%%%%%%");
    }
};


/* BLE interfacing functions
*/
// Start by scanning for the device we want to communicate with a FreeRTOS task
void initBLE() {

  pRemoteCharacteristic = NULL;
  //  myDevice = NULL;
  pClient = NULL;
  registeredForCallback = false;


  if (!connected) {
    printDebug("Finding Device");
    xTaskCreatePinnedToCore( xFindDevice, "FIND_DEVICE", 4096, (void *) 1 , tskIDLE_PRIORITY, &xConnect, 0 );
  }
}

//Called from the
void xFindDevice(void * pvParameters ) {
  BLEDevice::init("");
  printDebug("%%% Find Device Task Launched %%%");

  if (!myDevice) {
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(40);           //I've found these interval and window values to work the best with android, but others may be better.
    pBLEScan->setWindow(39);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(8);
  }

  if (myDevice) {
    printDebug("%%% Device Found %%%");
    xTaskCreatePinnedToCore( formConnection, "FIND_DEVICE", 4096, (void *) 1 , tskIDLE_PRIORITY, &xConnect, 0 );
  } else {
    printDebug("%%%% Device Not Found %%%");
  }

  vTaskDelete(NULL);
}

/* Opens connection to the server and allows us to access the characteristic that data
    is transmitted to, this should be called after xFindDevice completes, however it
    cannot be run in a seperate thread due to I2C
*/
void formConnection(void * pvParameters) {
  //if for some this function is called before we find the device then we need to
  //do the scan again and make sure that we have a device

  printDebug("Form Connection Task Launched");
  if (!myDevice) {
    printDebug("...Device not recorded, scanning for device");
    xFindDevice((void*) 1);
  } else {
    printDebug("...Attempting to connect to device previously found");
  }

  //create a client to communicate to the server through
  pClient = BLEDevice::createClient();

  printDebug("...created client");

  //set callbacks for client, if it disconnects we need to know,
  //also we don't consider the device to be connected unless the client is connected
  pClient->setClientCallbacks(new MyClientCallback());

  printDebug("...set callback");

  //check that device is found again
  //attempting to connect to a null device will cause the device to crash
  if (myDevice) {
    printDebug("...forming connection to BLE device");
    if (pClient->connect(myDevice)) {
      printDebug("...client has formed connection");
    } else {
      printDebug("...ERROR: client has failed to form connection");
    }
  } else {
    printDebug("...ERROR: myDevice was found to be null when attempting to connect to server");
    vTaskDelete(NULL);
  }

  //obtain a reference to the desired service
  //and check for null reference, this indicates failure
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (!pRemoteService) {
    pClient->disconnect();
    printDebug("%%%% Could not obtain remote service");
    vTaskDelete(NULL);
  }

  //second verse same as the first, but for the characteristic
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (!pRemoteCharacteristic) {
    pClient->disconnect();
    printDebug("%%%% Could not obtain remote characteristic");
    vTaskDelete(NULL);
  }

  printDebug("...Bluetooth connection established");

  vTaskDelete(NULL);
}

String sendBLE(String command, bool hasReturnData) {
  String ret = "";

  //write our command to the remote characteristic
  if (pRemoteCharacteristic) {
    pRemoteCharacteristic->writeValue(command.c_str(), command.length());
    Serial.println("Wrote \"" + command + "\" to remote device");
  } else {
    return ret;
  }

  if (hasReturnData) {
    while (ret[ret.length() - 1] != '*' && ret[ret.length() - 2] != '*' && ret[ret.length() - 3] != '*') {
      String receivedData = String(pRemoteCharacteristic->readValue().c_str());
      if (!receivedData.equals("null")) {
        ret = ret + receivedData;
        receivedData = "";
      }
    }
    Serial.println("Data Obtained from BLE Device: " + ret);
  }
  return ret;
}
