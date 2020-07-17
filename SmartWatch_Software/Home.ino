bool firstHomeSwitch = true;

void switchToHome()
{

  //  SweepClear();
  frameBuffer->drawFastHLine(0, 5, frameBuffer->width(), INTERFACE_COLOR);
  drawTime(13, 10, 2);
  frameBuffer->drawFastHLine(0, 29, frameBuffer->width(), INTERFACE_COLOR);
  drawDateCentered(26, 1);
  frameBuffer->setTextSize(1);

  writeNotifications();

  paintButtonFull(homeNotificationsButton);
  paintButtonFull(homeSettingsButton);
  paintButtonFull(homeAppsButton);

  currentPage = HOME;
  //  if (firstHomeSwitch) {
  //    tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  //    firstHomeSwitch = false;
  //  }
  homeLoop();


}

void writeNotifications() {
  frameBuffer->setCursor(0, 40);
  frameBuffer->setTextSize(0);
  frameBuffer->setTextWrap(false);

  int lines = getNotificationLines();
  frameBuffer->println("Notifications");

  if (lines > 1) {
    for (int a = 0; a < lines - 1; a++) {
      frameBuffer->println(parseFromNotifications(a, 0) + "-" + parseFromNotifications(a, 1));
    }
  } else {
    frameBuffer->println("No Notifications");
  }
  frameBuffer->setTextWrap(true);

#ifdef SHOW_LAST_NOTIFICATION_TIME
  frameBuffer->setTextColor(GRAYED);
  frameBuffer->setCursor(2, SCREEN_HEIGHT - 20);
  frameBuffer->println(parseFromNotifications(lines - 1, 0));
  frameBuffer->setTextColor(INTERFACE_COLOR);
#endif

  paintButtonFull(homeNotificationsButton);
  paintButtonFull(homeAppsButton);
  paintButtonFull(homeSettingsButton);

}

int circlePosition = 0;

void drawHome()
{

  frameBuffer -> fillScreen(BACKGROUND_COLOR);

  frameBuffer -> drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

  //it's here if you want it
//    drawCircularAnimation1(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2 + 30);
  //  drawArc(80, 60, 50, 10, circlePosition++, 30,  0x0011);

  frameBuffer->drawFastHLine(0, 5, frameBuffer->width(), INTERFACE_COLOR);
  drawTime(13, 10, 2);
  frameBuffer->drawLine(0, 29, 10, 29, INTERFACE_COLOR);
  frameBuffer->drawLine(SCREEN_WIDTH, 29, SCREEN_WIDTH - 10, 29, INTERFACE_COLOR);
  drawDateCentered(26, 1);
  frameBuffer->setTextSize(1);
  frameBuffer->setCursor(0, 40);

  writeNotifications() ;

  //since we use the framebuffer now we don't really need to fill in the background
  //  frameBuffer->fillRect(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH - 33, 10, BACKGROUND_COLOR);
  frameBuffer->setCursor(0, SCREEN_HEIGHT - 10);
  frameBuffer->print("Battery ");
  frameBuffer->print(String(getBatteryPercentage()));
  frameBuffer->print("%");
  if (getBatteryCurrent() > 0.0) {
    frameBuffer->print(" Charging");
  }

  paintButton(homeNotificationsButton);
  paintButton(homeAppsButton);
  paintButton(homeSettingsButton);

  tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
}

void HomeTouchHandler(struct point p)
{
  if (checkButtonPress(homeNotificationsButton, p.xPos, p.yPos))
  {
    pressButton(homeNotificationsButton);
    switchToNotifications();
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
    drawHome();
    if (touchDetected) {
      handleTouch();
      touchDetected = false;
    }

  }
}
