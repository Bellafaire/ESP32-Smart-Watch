void switchToHome()
{
  SweepClear();
  tft.drawFastHLine(0, 5, tft.width(), INTERFACE_COLOR);
  drawTime(13, 10, 2);
  tft.drawFastHLine(0, 29, tft.width(), INTERFACE_COLOR);
  drawDateCentered(26, 1);
  tft.setTextSize(1);

  writeNotifications();

  paintButtonFull(homeNotificationsButton);
  paintButtonFull(homeSettingsButton);
  paintButtonFull(homeAppsButton);

  currentPage = HOME;
  homeLoop();
}

void writeNotifications() {
  tft.setCursor(0, 40);
  tft.setTextSize(0);
  tft.setTextWrap(false);

  int lines = getNotificationLines();
  tft.println("Notifications");

  if (lines > 1) {
    for (int a = 1; a < lines - 3; a++) {
      tft.println(parseFromNotifications(a, 0) + "-" + parseFromNotifications(a, 1));
    }
  } else {
    tft.println("No Notifications");
  }
  tft.setTextWrap(true);

  paintButtonFull(homeNotificationsButton);
  paintButtonFull(homeAppsButton);
  paintButtonFull(homeSettingsButton);

}

int getNotificationLines() {
  int lineCount = 0;
  for (int a = 0; a < 2048; a++) {
    if (notificationData[a] == '\n') {
      lineCount++;
    }
  }
  return lineCount;
}

//find the information contained in a given line and given position in a comma seperated value
String parseFromNotifications(int line, int field) {
  String lineData = getValue(String(notificationData), '\n', line);
  return getValue(lineData, ',', field);
}

//duplicated in settings.ino
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void drawHome()
{
  tft.drawFastHLine(0, 5, tft.width(), INTERFACE_COLOR);
  drawTime(13, 10, 2);
  tft.drawFastHLine(0, 29, tft.width(), INTERFACE_COLOR);
  drawDateCentered(26, 1);
  tft.setTextSize(1);
  tft.setCursor(0, 40);

  writeNotifications() ;

  tft.fillRect(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH - 33, 10, BACKGROUND_COLOR);
  tft.setCursor(0, SCREEN_HEIGHT - 10);
  tft.print("Battery ");
  tft.print(String(getBatteryPercentage()));
  tft.print("%");
  if (getBatteryCurrent() > 0.0) {
    tft.print(" Charging");
  }

  paintButton(homeNotificationsButton);
  paintButton(homeAppsButton);
  paintButton(homeSettingsButton);
}

void HomeTouchHandler(struct point p)
{
  if (checkButtonPress(homeNotificationsButton, p.xPos, p.yPos))
  {
    pressButton(homeNotificationsButton);
    // switchToNotifications();
  }
  if (checkButtonPress(homeSettingsButton, p.xPos, p.yPos))
  {
    pressButton(homeSettingsButton);
    switchToSettings();
  }
}

void homeLoop()
{
  lastTouchTime = millis();
  while (lastTouchTime + screenOnTime > millis())
  {
    for (int a = 0; a < 100; a++)
    {
      handleTouch();
      delay(10);
    }
    drawHome();
  }
}
