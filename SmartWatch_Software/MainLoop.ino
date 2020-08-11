
/*  everything that happens in the normal operation of the smartwatch happens here. this is kind of the user-interface part of the code.
*/
void MainLoop()
{
  initBLE();
  while (lastTouchTime + screenOnTime > millis()) {
    if (touchDetected) {
       //handleTouch() operates in a similar manner to the MainLoop() 
       //and simply switches to the correct touch handler
       handleTouch(); 
    } else {
      if(myDevice && !connected){
        formConnection();
      }
      switch (currentPage)
      {
        case TEST_SCREEN:
          testScreen();
          break;
        case HOME:
          drawHome();
          break;
        case SETTINGS:
          drawSettings();
          break;
        case NOTIFICATIONS: 
          drawNotifications();
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
