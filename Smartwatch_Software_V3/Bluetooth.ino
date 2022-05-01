/*****************************************************************************
  The MIT License (MIT)
  Copyright (c) 2021 Matthew James Bellafaire
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

unsigned long last_ble_data_rec = 0; 


void addData(String data) {
  printDebug("Received:" + data);
  currentDataField += data;
  if (!blockingCommandInProgress) {
    *bleReturnString = currentDataField;
  }
  //this is kind of a fringe case where the app can sometimes bug out
  //not sure what causes it but either way if we're not receiving data 
  //the operation is over anyway.
  if(data.length() == 0){
//    operationInProgress = false;
  }
}

class cb : public BLEServerCallbacks    {
    void onConnect(BLEServer* pServer) {
      connected = true;
      onConnectEvent();
      printDebug("BLE Device Connected");
    }
    void onDisconnect(BLEServer* pServer) {
      connected = false;
      printDebug("BLE Device Disconnected");
    }
};

class ccb : public BLECharacteristicCallbacks  {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      addData(String( pCharacteristic->getValue().c_str()));
        last_ble_data_rec = millis();
    }
    void onRead(BLECharacteristic* pCharacteristic) {
      //      Serial.println("Characteristic Read");
      operationInProgress = false;
      printDebug("Complete Received String:\n" + currentDataField);
    }
};

void initBLE() {
  BLEDevice::init("ESP32 Smartwatch");
  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);

  //define the characteristics and how they can be used
  commandCharacteristic = pService->createCharacteristic(
                            COMMAND_UUID,
                            BLECharacteristic::PROPERTY_READ   |
                            BLECharacteristic::PROPERTY_WRITE  |
                            BLECharacteristic::PROPERTY_NOTIFY
                          );


  //set all the callbacks
  commandCharacteristic->setCallbacks(new ccb());
  commandCharacteristic->setValue("");

  //add server callback so we can detect when we're connected.
  pServer->setCallbacks(new cb());

  pService->start();
  startBLEAdvertising();
}

//BLE tends to malfunction between light sleep cycles. It's better to shut it down completely 
//and restart it at the beginning of each startup.
//this basically will crash on boot 31. since we're not freeing the memory. but currently the 
//call to esp_bt_controller_mem_release() that happens when deinit(true) is called locks the 
//system up so pick your poison. 
void deinitBLE() {
  BLEDevice::deinit(false); 
  connected = false;
  printDebug("Shutting down BLE"); 
}

void startBLEAdvertising() {
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

//sends BLE command and returns data to a specific string. This function can be blocking (if you need it to perform a specific action) or non-blocking
//if you don't mind the data being used as its received.
boolean sendBLE(String command, String* returnString, boolean blocking) {
  if (connected && !operationInProgress) {
    blockingCommandInProgress = blocking;
    operationInProgress = true;
    commandCharacteristic->setValue(command.c_str());
    commandCharacteristic->notify();

    printDebug("Sent BLE Command: " + command);

    if (blockingCommandInProgress) {

      currentDataField = "";

      last_ble_data_rec = millis();
      while (operationInProgress && (last_ble_data_rec + 2000 > millis()))
        delay(25);

      operationInProgress = false;
      if (currentDataField.length() == 0) {
        return false;
      }  else {
        *returnString = currentDataField;
        return true;
      }

    } else {
      currentDataField = "";
      bleReturnString = returnString;
      *returnString = currentDataField;

      unsigned long startTime = millis();
      while ((currentDataField.length() == 0) && (startTime + 1000 > millis()))
        delay(25);

      if (currentDataField.length() == 0) {
        operationInProgress = false;
        return false;
      }  else {
        return true;
      }
    }
  }

  return false;
}

boolean sendBLE(String command) {
  if (connected && !operationInProgress) {
    operationInProgress = true;
    commandCharacteristic->setValue(command.c_str());
    commandCharacteristic->notify();

    printDebug("Sent BLE Command: " + command);
    unsigned long startTime = millis();
    while (operationInProgress && (startTime + 200 > millis()))
      delay(25);
    return !operationInProgress;
  }
  return false;
}


//just an extra variable to make sure we don't end up sending the same command multiple times
unsigned long lastMediaCommandIssued = 0;

void playMusic() {
  if (lastMediaCommandIssued + 500 < millis()) {
    sendBLE("/play");
    lastMediaCommandIssued = millis();
  }
}

void pauseMusic() {
  if (lastMediaCommandIssued + 500 < millis()) {
    sendBLE("/pause");
    lastMediaCommandIssued = millis();
  }
}

void nextSong() {
  if (lastMediaCommandIssued + 500 < millis()) {
    sendBLE("/nextSong");
    lastMediaCommandIssued = millis();
  }
}

void lastSong() {
  if (lastMediaCommandIssued + 500 < millis()) {
    sendBLE("/lastSong");
    lastMediaCommandIssued = millis();
  }
}

void updateCurrentSong() {
  if (lastSongUpdate + 500 < millis()) {
    sendBLE("/currentSong", &currentSong, false);
    lastSongUpdate = millis();
  }
}