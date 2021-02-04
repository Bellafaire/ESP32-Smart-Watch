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


void addData(String data) {
  //  Serial.println("Adding:" + data);
  currentDataField += data;
}

class cb : public BLEServerCallbacks    {
    void onConnect(BLEServer* pServer) {
      connected = true;
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
    }
    void onRead(BLECharacteristic* pCharacteristic) {
      //      Serial.println("Characteristic Read");
      operationInProgress = false;
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


  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

String sendBLE(String command) {
  operationInProgress = true;
  commandCharacteristic->setValue(command.c_str());
  commandCharacteristic->notify();

  printDebug("Sent BLE Command: " + command);

  currentDataField = "";

  unsigned long startTime = millis();
  while (operationInProgress && (startTime + 2000 > millis()))
    delay(25);

  return currentDataField;

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
    currentSong = sendBLE("/currentSong");
    lastSongUpdate = millis();
  }
}
