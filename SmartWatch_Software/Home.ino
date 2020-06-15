PROGMEM iconButton homeNotificationsButton = {128, 32, 32, 32, INTERFACE_COLOR, BACKGROUND_COLOR, {(0b00100001 << 8) | 0b10000100, (0b01000111 << 8) | 0b11100010, (0b10001100 << 8) | 0b00110001, (0b10011000 << 8) | 0b00011001, (0b10010000 << 8) | 0b00001001, (0b00110000 << 8) | 0b00001100, (0b00110000 << 8) | 0b00001100, (0b00110000 << 8) | 0b00001100, (0b00110000 << 8) | 0b00001100, (0b00110000 << 8) | 0b00001100, (0b00100000 << 8) | 0b00000100, (0b01100000 << 8) | 0b00000110, (0b11000000 << 8) | 0b00000011, (0b11000000 << 8) | 0b00000011, (0b11111111 << 8) | 0b11111111, (0b00000011 << 8) | 0b11000000}};

PROGMEM iconButton homeSettingsButton = {128, 64, 32, 32, INTERFACE_COLOR, BACKGROUND_COLOR, {(0b00000000 << 8) | 0b00001110, (0b00000000 << 8) | 0b00011111, (0b11111111 << 8) | 0b11011111, (0b11111111 << 8) | 0b11011111, (0b00000000 << 8) | 0b00001110, (0b00001100 << 8) | 0b00000000, (0b11011111 << 8) | 0b01111111, (0b11011111 << 8) | 0b01111111, (0b00011111 << 8) | 0b00000000, (0b00001110 << 8) | 0b00001110, (0b00000000 << 8) | 0b00011111, (0b11111111 << 8) | 0b11011111, (0b11111111 << 8) | 0b11011111, (0b00000000 << 8) | 0b00001110, (0b00000000 << 8) | 0b00000000, (0b00000000 << 8) | 0b00000000}};

PROGMEM iconButton homeAppsButton = {128, 96, 32, 32, INTERFACE_COLOR, BACKGROUND_COLOR, {
                                                                                             (0b00000000 << 8) | 0b00000000,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b00000000 << 8) | 0b00000000,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b00000000 << 8) | 0b00000000,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b01111011 << 8) | 0b11011110,
                                                                                             (0b00000000 << 8) | 0b00000000,
                                                                                         }};
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
    tft.print("Battery Voltage ");
    tft.print(getBatteryVoltage());

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
        // switchToSettings();
    }
}

void homeLoop()
{
    lastTouchTime = millis();
    while (lastTouchTime + screenOnTime > millis())
    {
        for (int a = 0; a < 10; a++)
        {
            handleTouch();
            delay(100);
        }
        drawHome();
    }
}