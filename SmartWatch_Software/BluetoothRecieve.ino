#define BLUETOOTH_TIMEOUT 30000
void getPhoneNotifications(int timeout) {

  long startTime = millis();
  char recievedData[2048];
  int currentPosition = 0;

  SerialBT.begin("ESPWatch", false); //Open up bluetooth and wait for data to come in
  SerialBT.connect(deviceBTAddr); //your device name here
#ifdef DEBUG
  Serial.println("Connected to phone, disconnecting as master");
#endif

  //recieve data until either 30 seconds has passed (timeout) or the "***" has been recieved indicating the end of the transmission.
  //also allow user touching the screen to interrup the process
  touchDetected = false;
  while (startTime + timeout > millis() && !touchDetected)
  {

    if (SerialBT.available()) {
      recievedData[currentPosition] = SerialBT.read();
#ifdef DEBUG
      Serial.print( recievedData[currentPosition]);
#endif

      if (currentPosition > 3) {
        if (recievedData[currentPosition] == '*' &&
            recievedData[currentPosition - 1] == '*' &&
            recievedData[currentPosition - 2] == '*') {
#ifdef DEBUG
          Serial.print( "Message Terminator Recieved");
#endif
          break;
        }
      }
      currentPosition++;
    }
  }

  //  SerialBT.end(); //apparently this causes a crash (https://github.com/espressif/arduino-esp32/issues/2718)
#ifdef DEBUG
  Serial.println("***BT Closed***");
#endif

  //  //if we recieved the entire transmission then update the RTC data
  if (currentPosition > 3) {
    if (recievedData[currentPosition] == '*' &&
        recievedData[currentPosition - 1] == '*' &&
        recievedData[currentPosition - 2] == '*') {
#ifdef DEBUG
      Serial.println("Recieved Notifications Data in getPhoneNotifications():");
#endif
      for (int a = 0; a < 2048; a++) {
        notificationData[a] = recievedData[a];
      }
#ifdef DEBUG
      Serial.println("End of Stream");
#endif
    }
  }

}
