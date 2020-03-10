#define settingNumber 3
#define MAX_ON_SCREEN_SETTINGS_BUTTONS 5

#define SETTING_BUTTON_WIDTH 128
#define SETTING_BUTTON_HEIGHT 25

int settingScrollPosition = 0;
#define SETTING_OPTIONS 5

PROGMEM iconButton upArrowButton =   { 128, 0, 32, 48, INTERFACE_COLOR, BACKGROUND_COLOR,  {(0b00000001 << 8) | 0b10000000,  (0b00000011 << 8) | 0b11000000,  (0b00000111 << 8) | 0b11100000,  (0b00001111 << 8) | 0b11110000,  (0b00011111 << 8) | 0b11111000,  (0b00111111 << 8) | 0b11111100,  (0b01111111 << 8) | 0b11111110,  (0b11111111 << 8) | 0b11111111,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000}};
PROGMEM iconButton downArrowButton =   { 128, 48, 32, 48, INTERFACE_COLOR, BACKGROUND_COLOR,   {(0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b11111111 << 8) | 0b11111111,  (0b01111111 << 8) | 0b11111110,  (0b00111111 << 8) | 0b11111100,  (0b00011111 << 8) | 0b11111000,  (0b00001111 << 8) | 0b11110000,  (0b00000111 << 8) | 0b11100000,  (0b00000011 << 8) | 0b11000000,  (0b00000001 << 8) | 0b10000000}};

//define options positions
#define REUPDATE_TIME 0
#define CHOOSE_WIFI_NETWORK 1
#define BATTERY 2
#define ACCELTEST 3
#define ABOUT 4


button settingButtons[SETTING_OPTIONS] = {
  { 0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "Re-update Time"},
  { 0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "Choose WiFi Network"},
  { 0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "Charge Battery"},
  { 0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "AccelTest"},
  { 0, 0, SETTING_BUTTON_WIDTH, SETTING_BUTTON_HEIGHT, INTERFACE_COLOR, BACKGROUND_COLOR, "About"}
};

String settingOptions[] = {
  "Re-update Time",
  "Choose WiFi Network",
  "About"
};


void switchToSettings() {
  SweepClear();
  settingScrollPosition = 0;
  drawSettings();
  paintButtonFull(homeButton);
  page = SETTINGS;
}

void drawSettings() {


  //configuring buttons
  if (SETTING_OPTIONS > MAX_ON_SCREEN_SETTINGS_BUTTONS) {
    for (int a = 0; a < MAX_ON_SCREEN_SETTINGS_BUTTONS; a++) {
      settingButtons[a + settingScrollPosition]._x = 1;
      settingButtons[a + settingScrollPosition]._y = 1 + SETTING_BUTTON_HEIGHT * a ;
      paintButton(settingButtons[a + settingScrollPosition]);
    }
  } else {
    for (int a = 0; a < SETTING_OPTIONS; a++) {
      settingButtons[a]._x = 1;
      settingButtons[a]._y = 1 + SETTING_BUTTON_HEIGHT * a;
      paintButton( settingButtons[a]);
    }
  }
  paintButton(upArrowButton);
  paintButton(downArrowButton);

  paintButton(homeButton);
}

void SettingsTouchHandler(int x, int y) {
  if (checkButtonPress(homeButton, x, y) ) {
    pressButton(homeButton);
    switchToHome();
  }
  if (checkButtonPress(upArrowButton, x, y) ) {
    if (settingScrollPosition > 0) {
      settingScrollPosition--;
      SweepClear();
      drawSettings();
    }
  }
  if (checkButtonPress(downArrowButton, x, y) ) {
    if (settingScrollPosition < SETTING_OPTIONS - MAX_ON_SCREEN_SETTINGS_BUTTONS) {
      settingScrollPosition++;
      SweepClear();
      drawSettings();
    }
  }
  for (int a = 0; a < SETTING_OPTIONS; a++) {
    if ( checkButtonPress(settingButtons[a], x, y)) {
      switch (a) {
        case REUPDATE_TIME: reAdjustTime(); break;
        case CHOOSE_WIFI_NETWORK: changeNetwork(); break;
        case ABOUT: about(); break;
        case BATTERY: batterySettings(); break;
        case ACCELTEST: accelTest(); break;
      }
    }
  }
}

void accelTest() {

  pinMode(ACCEL_X, INPUT);
  pinMode(ACCEL_Y, INPUT);
  pinMode(ACCEL_Z, INPUT);

  Window w = Window(0, 14, 160, 100, false);

  long lastTouchTime = millis();

  while (w.isFocused()) {
    boolean active = lastTouchTime + 5000 > millis(); //if the screen is off there's no sense in changing anything
    if (active) {
      digitalWrite(LCD_LED_CTRL, HIGH);
    } else {
      digitalWrite(LCD_LED_CTRL, LOW);
    }

    Window w = Window(0, 14, 160, 100, false);
    w.touch();
    w.println("X = " + String(analogRead(ACCEL_X)));
    w.println("Y = " + String(analogRead(ACCEL_Y)));
    w.println("Z = " + String(analogRead(ACCEL_Z)));
    for (int a = 0; a < 1000; a++) {
      if (ts.touched()) {
        lastTouchTime = millis();
      }
      w.touch();
      delay(1);
    }
    if (!w.isFocused()) {
      break;
    }
  }
  digitalWrite(CHARGING_PIN, LOW);
  SweepClear();
  drawSettings();
}

void  reAdjustTime() {
  Window w = Window(14, 14, 132, 100, false);
  w.println("Getting Updated Time from Server");
  w.println(getInternetTime());
  delay(500);
  w.println("Done");
  delay(500);
  SweepClear();
  drawSettings();
}

void batterySettings() {
  digitalWrite(CHARGING_PIN, HIGH);
  Window w = Window(0, 14, 160, 100, false);

  long lastTouchTime = millis();

  while (w.isFocused()) {
    boolean active = lastTouchTime + 5000 > millis(); //if the screen is off there's no sense in changing anything
    if (active) {
      digitalWrite(LCD_LED_CTRL, HIGH);
    } else {
      digitalWrite(LCD_LED_CTRL, LOW);
    }

    Window w = Window(0, 14, 160, 100, false);
    w.touch();
    w.println("Charging Battery");
    w.println("Current Voltage " + String(getBatteryVoltage()));
    for (int a = 0; a < 1000; a++) {
      if (ts.touched()) {
        lastTouchTime = millis();
      }
      w.touch();
      delay(1);
    }
    if (!w.isFocused()) {
      break;
    }
  }
  digitalWrite(CHARGING_PIN, LOW);
  SweepClear();
  drawSettings();
}

void changeNetwork() {
  SelectionWindow w = SelectionWindow(0, 14, 160, 100);
  int homeWifi = w.addOption("Bellafaire Family Wifi");
  int phoneWifi = w.addOption("Phone Wifi");

  int selectedOption = w.focus();
  if (selectedOption == homeWifi) {
    ssid = "Bellafaire Family Wifi";
    password = "cashewbunny";
    testWifi();
  } else if (selectedOption == phoneWifi) {
    ssid = "A truly fruitless network";
    password = "matthewbellafaire";
    testWifi();
  }
}

void testWifi() {
  Window w = Window(0, 14, 160, 100, false);
  w.print(String("Connecting to ") + ssid);

  char ssid0[60];
  char password0[60];

  ssid.toCharArray(ssid0, ssid.length() + 1);
  password.toCharArray(password0, password.length() + 1);

  WiFi.begin(ssid0, password0);

  int count = 0;
  while (WiFi.status() != WL_CONNECTED ) {
    delay(400);
    count++;
    w.print(".");
    if (count > 20) {
      w.println("CONNECTION FAILED");
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    w.println("Connected");
    w.focus();
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  w.println("Disconnected from wifi");

  SweepClear();
  drawSettings();
}


void about() {
  Window w = Window(0, 14, 160, 100, true);
  w.println("SmartWatch OS build 0.0.1");
  w.println("Created by Matthew James Bellafaire");
  w.println("hackaday.io/bellafaire");

  w.focus();

  SweepClear();
  drawSettings();
}
