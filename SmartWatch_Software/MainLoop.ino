
/*  everything that happens in the normal operation of the smartwatch happens here. this is kind of the user-interface part of the code.  
 */
void MainLoop()
{
  switch (currentPage)
  {
  case TEST_SCREEN:
    testScreen();
    break;
  case HOME:
#ifdef DEBUG
    Serial.println("Switched to Home");
#endif
    switchToHome();
    break;
  case SETTINGS:
#ifdef DEBUG
    Serial.println("Switched to Settings");
#endif
    switchToSettings();
    break;
  default:
    currentPage = 0;
    break;
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

  for (int a = 0; a < 10000; a++)
  {
    // readTouch();
    if (touchDetected)
    {
      readTouch();
      touchDetected = false;
    }
    delay(1);
  }
}