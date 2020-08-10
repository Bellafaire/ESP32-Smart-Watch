
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
      printDebug("Switched to Home");
      switchToHome();
      break;
    case SETTINGS:
      printDebug("Switched to Settings");
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

  tft.println("Starting device....");

}
