int numberOfNotifications;
int selectedNotification = 0;

void NotificationsTouchHandler(struct point p) {
  if (checkButtonPress(homeButton, p.xPos, p.yPos))
  {
    pressButton(homeButton);
    switchToHome();
  }
  else if (checkButtonPress(upArrowButton, p.xPos, p.yPos))
  {
    if (selectedNotification > 0)
    {
      selectedNotification--;
      drawNotifications();
    }
  }
  else if (checkButtonPress(downArrowButton, p.xPos, p.yPos))
  {
    if (selectedNotification < numberOfNotifications - 2)
    {
      selectedNotification++;
      drawNotifications();
    }
  }
  else if (p.xPos > 0 && p.xPos < SCREEN_WIDTH - 32 && p.yPos > 0 && p.yPos < SCREEN_HEIGHT) {
    openNotification(selectedNotification);
    switchToNotifications();
  }
#ifdef DEBUG
  Serial.println("notification selected index: " + String(selectedNotification));
#endif
}

void openNotification(int sel) {
  Window w = Window(0, 14, 160, 100, true);

  w.println(parseFromNotifications(sel, 0)); //app name

  //EXTRA_TITLE
  if (parseFromNotifications(sel, 1).length() > 0) {
    w.println("Title");
    w.println(parseFromNotifications(sel, 1));
  }

  //EXTRA_TEXT
  if (parseFromNotifications(sel, 2).length() > 0) {
    w.println("Text");
    w.println( parseFromNotifications(sel, 2));

  }

  //EXTRA_INFO_TEXT
  if (parseFromNotifications(sel, 3).length() > 0) {
    w.println("Info");
    w.println(parseFromNotifications(sel, 3));
  }

  //EXTRA_SUB_TEXT
  if (parseFromNotifications(sel, 4).length() > 0) {
    w.println("Subtext");
    w.println(parseFromNotifications(sel, 4));
  }

  //EXTRA_TITLE
  if (parseFromNotifications(sel, 5).length() > 0) {
    w.println("extratitle");
    w.println(parseFromNotifications(sel, 5));
  }

  //EXTRA_TITLE
  if (parseFromNotifications(sel, 6).length() > 0) {
    w.println(parseFromNotifications(sel, 6));
  }


  w.focus();
}


void notificationsLoop()
{
  delay(300);
  selectedNotification = 1;
  //settings has a seperate timeout
  while (currentPage == NOTIFICATIONS)
  {
    handleTouch();
  }

}

void switchToNotifications()
{
#ifdef DEBUG
  Serial.println("Switched to Notifications");
#endif
  numberOfNotifications = getNotificationLines();
  selectedNotification = 0;
  drawNotifications();
  currentPage = NOTIFICATIONS;
  notificationsLoop();
}


void drawNotifications() {
  //fill in the background
  frameBuffer -> drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

  int y_pos = 0;
  numberOfNotifications = getNotificationLines();
  if (numberOfNotifications > 0) {
    frameBuffer->setTextWrap(false);
    //for whatever reason line 1 is a blank line and the last line contains the time
    for (int a = 1; a < numberOfNotifications - 1; a++) {
      if (selectedNotification == a) {
        frameBuffer->fillRect(0, y_pos - 1, SCREEN_WIDTH, 8, INTERFACE_COLOR);
        frameBuffer->setTextColor(BACKGROUND_COLOR);
        frameBuffer->setCursor(0, y_pos);
        frameBuffer->print(parseFromNotifications(a, 0));
      } else {
        frameBuffer->setTextColor(INTERFACE_COLOR);
        frameBuffer->setCursor(0, y_pos);
        frameBuffer->print(parseFromNotifications(a, 0));
      }
      y_pos += 10;
    }
    frameBuffer->setTextWrap(true);
  } else {
    frameBuffer->setCursor(0, 0);
    frameBuffer->print("No Notifications");
  }

  //just setting this back to prevent any issues later.
  frameBuffer->setTextColor(TEXT_COLOR);

  paintButtonFull(upArrowButton);
  paintButtonFull(downArrowButton);
  paintButtonFull(homeButton);
  tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
}


int getNotificationLines() {
  int lineCount = 0;
  for (int a = 0; a < 2048; a++) {
    if (notificationData[a] == '\n') {
      lineCount++;
    }
    if (notificationData[a] == '*' &&
        notificationData[a - 1] == '*' &&
        notificationData[a - 2] == '*') {
      break;
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
