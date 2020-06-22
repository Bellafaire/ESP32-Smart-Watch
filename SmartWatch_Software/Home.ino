void switchToHome()
{
    SweepClear();
    tft.drawFastHLine(0, 5, tft.width(), INTERFACE_COLOR);
    drawTime(13, 10, 2);
    tft.drawFastHLine(0, 29, tft.width(), INTERFACE_COLOR);
    drawDateCentered(26, 1);
    tft.setTextSize(1);
    tft.setCursor(0, 40);
    tft.print("No Notifications...");

    paintButtonFull(homeNotificationsButton);
    paintButtonFull(homeSettingsButton);
    paintButtonFull(homeAppsButton);

    currentPage = HOME;
    homeLoop();
}

void drawHome()
{
    tft.drawFastHLine(0, 5, tft.width(), INTERFACE_COLOR);
    drawTime(13, 10, 2);
    tft.drawFastHLine(0, 29, tft.width(), INTERFACE_COLOR);
    drawDateCentered(26, 1);
    tft.setTextSize(1);
    tft.setCursor(0, 40);
    tft.print("No Notifications...");

    tft.fillRect(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH - 33, 10, BACKGROUND_COLOR);
    tft.setCursor(0, SCREEN_HEIGHT - 10);
    tft.print("Battery ");
    tft.print(String(getBatteryPercentage()));
    tft.print("%");
    if(getBatteryCurrent() > 0.0){
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
