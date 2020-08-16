/*  everything that happens in the normal operation of the smartwatch happens here. this is kind of the user-interface part of the code.
*/
bool newNotificationData = false;
bool dontSleep = false;

void MainLoop()
{
  initBLE();
  while (lastTouchTime + screenOnTime > millis() || dontSleep) {
    if (!newNotificationData && pRemoteCharacteristic) {
      updateNotificationData();
      newNotificationData = true;
    }
    if (touchDetected) {
      //handleTouch() operates in a similar manner to the MainLoop()
      //and simply switches to the correct touch handler
      handleTouch();
    } else {
      switch (currentPage)
      {
        case TEST_SCREEN:
          dontSleep = false;
          testScreen();
          break;
        case HOME:
          dontSleep = false;
          drawHome();
          break;
        case SETTINGS:
          dontSleep = false;
          drawSettings();
          break;
        case NOTIFICATIONS:
          dontSleep = false;
          drawNotifications();
          break;
        case CALCULATOR:
          dontSleep = true;
          drawCalculator();
          break;
        default:
          currentPage = 0;
          break;
      }
    }
  }
  digitalWrite(LCD_LED, LOW);
}

void testScreen()
{
  tft.setTextColor(TEXT_COLOR);
  tft.setCursor(0, 0);
  tft.println("ESP32 Smartwatch");

  tft.print("Battery Percentage: ");
  tft.print(getBatteryPercentage());
  tft.println("%");

  tft.print("Battery Voltage: ");
  tft.print(getBatteryVoltage());
  tft.println("V");

  pinMode(CHG_STAT, INPUT);
  tft.print("Charging Status: ");
  tft.println(digitalRead(CHG_STAT));

  tft.println("Starting device....");

}
