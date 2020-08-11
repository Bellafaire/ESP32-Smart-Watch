#define settingNumber 3
#define MAX_ON_SCREEN_SETTINGS_BUTTONS 5

#define SETTING_BUTTON_WIDTH 128
#define SETTING_BUTTON_HEIGHT 25

int settingScrollPosition = 0;
#define SETTING_OPTIONS 7

//define options positions
#define REUPDATE_TIME 0
#define BATTERY 1
#define ACCELTEST 2
#define NETWORK_SELECT 3
#define OBTAIN_NOTIFICATIONS 4
#define VIEW_RAW_NOTIFICATIONS 5
#define ABOUT 6

button settingButtons[SETTING_OPTIONS] = {
  {0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "Re-update Time"},
  {0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "Battery Stats"},
  {0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "AccelTest"},
  {0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "Select Network"},
  {0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "Obtain Notifications"},
  {0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "Raw Notifications"},
  {0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "About"}
};

String settingOptions[] = {
  "Re-update Time",
  "About"
};

void settingsLoop()
{
  //settings has a seperate timeout
  while (currentPage == SETTINGS)
  {
    handleTouch();
    delay(10);
  }
}

//not currently working
void changeNetwork()
{
  SelectionWindow w = SelectionWindow(0, 14, 160, 100);

  int networks[networkNumber];

  for (int a = 0; a < networkNumber; a++)  {
    w.addOption(ssidList[a]);
  }

  printDebug("Focusing network selection window");

  delay(50); //in some cases the touch can get read twice and exit the selection screen right away, this delay prevents it

  int selectedOption = w.focus() - 1; //the cancel option of the selection counts as option 0, since array is zero indexed we subtract

  printDebug("network selection window lost focus with selected option: " + String(selectedOption));

  if (selectedOption >= 0)  {
    ssid = ssidList[selectedOption];
    password = passwordList[selectedOption];
    testWifi();
  }
  else  {

    delay(100);
    printDebug("user exited selection window with cancel button");
  }
}

void drawSettings()
{
  //  frameBuffer -> fillScreen(BACKGROUND_COLOR);
  frameBuffer -> drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);
  //configuring buttons
  if (SETTING_OPTIONS > MAX_ON_SCREEN_SETTINGS_BUTTONS)
  {
    for (int a = 0; a < MAX_ON_SCREEN_SETTINGS_BUTTONS; a++)
    {
      settingButtons[a + settingScrollPosition]._x = 1;
      settingButtons[a + settingScrollPosition]._y = 1 + SETTING_BUTTON_HEIGHT * a;
      paintButton(settingButtons[a + settingScrollPosition]);
    }
  }
  else
  {
    for (int a = 0; a < SETTING_OPTIONS; a++)
    {
      settingButtons[a]._x = 1;
      settingButtons[a]._y = 1 + SETTING_BUTTON_HEIGHT * a;
      paintButton(settingButtons[a]);
    }
  }
  paintButtonFull(upArrowButton);
  paintButtonFull(downArrowButton);

  paintButton(homeButton);

  tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
}

void SettingsTouchHandler(struct point p)
{
  if (checkButtonPress(homeButton, p.xPos, p.yPos))
  {
    pressButton(homeButton);
    currentPage = HOME;
  }
  if (checkButtonPress(upArrowButton, p.xPos, p.yPos))
  {
    if (settingScrollPosition > 0)
    {
      settingScrollPosition--;
    }
  }
  if (checkButtonPress(downArrowButton, p.xPos, p.yPos))
  {
    if (settingScrollPosition < SETTING_OPTIONS - MAX_ON_SCREEN_SETTINGS_BUTTONS)
    {
      settingScrollPosition++;
    }
  }
  for (int a = 0; a < SETTING_OPTIONS; a++)
  {
    if (checkButtonPress(settingButtons[a], p.xPos, p.yPos))
    {
      switch (a)
      {
        case REUPDATE_TIME:
          reAdjustTime();
          break;
        case ABOUT:
          about();
          break;
        case NETWORK_SELECT:
          changeNetwork();
          break;
        case BATTERY:
          batterySettings();
          break;
        case OBTAIN_NOTIFICATIONS:
          getNotifications();
          break;
        case VIEW_RAW_NOTIFICATIONS:
          viewNotifications();
          break;
        case ACCELTEST:
          accelTest();
          break;
      }
    }
  }
}

void getNotifications() {

  Window w = Window(0, 14, 160, 100, true);
  w.println("Getting phone notifications");
  updateNotificationData();
  w.println("done");
  w.focus();
  drawSettings();
}


void accelTest()
{

  pinMode(X_ACCEL, INPUT);
  pinMode(Y_ACCEL, INPUT);
  pinMode(Z_ACCEL, INPUT);

  Window w = Window(0, 14, 160, 100, false);

  long lastTouchTime = millis();

  while (w.isFocused())
  {
    Window w = Window(0, 14, 160, 100, false);
    w.touch();
    w.println("X = " + String(analogRead(X_ACCEL)));
    w.println("Y = " + String(analogRead(Y_ACCEL)));
    w.println("Z = " + String(analogRead(Z_ACCEL)));
    for (int a = 0; a < 1000; a++)
    {
      if (!digitalRead(TOUCH_IRQ))
      {
        lastTouchTime = millis();
      }
      w.touch();
      delay(1);
    }
    if (!w.isFocused())
    {
      break;
    }
  }
  // digitalWrite(CHARGING_PIN, LOW);
  //  SweepClear();
  drawSettings();
}

void reAdjustTime()
{
  Window w = Window(14, 14, 132, 100, false);
  w.println("Getting Updated Time from Server");
  w.println(getInternetTime());
  delay(500);
  w.println("Done");
  delay(500);
  //  SweepClear();
  drawSettings();
}

void batterySettings()
{
  Window w = Window(0, 14, 160, 100, false);

  long lastTouchTime = millis();

  while (w.isFocused())
  {
    Window w = Window(0, 14, 160, 100, false);
    w.touch();

    // float getBatteryCurrent();
    // float getBatteryVoltage();
    // float getTotalCapacity();
    // float getRemainingCapacity();
    // int getBatteryPercentage();

    w.print("Voltage: ");
    w.print(String(getBatteryVoltage(), 3));
    w.println("V");

    w.print("Percentage: ");
    w.print(String(getBatteryPercentage()));
    w.println("%");

    w.print("Capacity: ");
    w.print(String(getRemainingCapacity() * 1000, 3));
    w.println("mAh");

    w.print("Estimated Life: ");
    w.print(String(getTimeUntilEmpty(), 2));
    w.println("H");

    for (int a = 0; a < 1000; a++)
    {
      if (!digitalRead(TOUCH_IRQ))
      {
        lastTouchTime = millis();
      }
      w.touch();
      delay(1);
    }
    if (!w.isFocused())
    {
      break;
    }
  }
  //  SweepClear();
  drawSettings();
}

void testWifi()
{
  Window w = Window(0, 14, 160, 100, false);
  w.print(String("Connecting to ") + ssid);

  char ssid0[60];
  char password0[60];

  ssid.toCharArray(ssid0, ssid.length() + 1);
  password.toCharArray(password0, password.length() + 1);

  WiFi.begin(ssid0, password0);

  int count = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(400);
    count++;
    w.print(".");
    if (count > 20)
    {
      w.println("CONNECTION FAILED");
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    w.println("Connected");
    w.focus();
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  w.println("Disconnected from wifi");

  //  SweepClear();
  drawSettings();
}

void viewNotifications() {
  Window w = Window(0, 14, 160, 100, true);
  w.println(String(notificationData));
  w.focus();
  //  SweepClear();
  drawSettings();
}

void about()
{
  Window w = Window(0, 14, 160, 100, true);
  w.println("SmartWatch OS 2");
  w.println("Created by Matthew James Bellafaire");
  w.println("hackaday.io/bellafaire");

  w.println("Smartwatch V4.1");

  w.focus();

  //  SweepClear();
  drawSettings();
}
