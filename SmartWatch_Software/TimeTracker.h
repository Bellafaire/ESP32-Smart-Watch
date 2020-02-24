boolean correctTime = false;

String getInternetTime() {


  WiFi.mode(WIFI_STA);
  char ssid0[60];
  char password0[60];

  ssid.toCharArray(ssid0, ssid.length() + 1);
  password.toCharArray(password0, password.length() + 1);

#ifdef DEBUG
  Serial.printf("Connecting to %s ", ssid0);
#endif

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  WiFi.begin(ssid0, password0);
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    count++;
    if (count > 20) {
#ifdef DEBUG
      Serial.println("COULD NOT CONNECT TO WIFI");
#endif
      break;
    }
  }

#ifdef DEBUG
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("CONNECTED");

  }
#endif

  char timestr[12] = "00:00:00 XM";
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
#ifdef DEBUG
    Serial.println("Failed to obtain time");
#endif
    //can't obtain the time for whatever reason, set time to arbitrary value so that we can boot
    struct tm tm;
    tm.tm_year = 2017 - 1900;
    tm.tm_mon = 11;
    tm.tm_mday = 8;
    tm.tm_hour = 19;
    tm.tm_min = 51;
    tm.tm_sec = 10;
    time_t t = mktime(&tm);
#ifdef DEBUG
    printf("Setting time: %s", asctime(&tm));
#endif
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);

    correctTime = false;

    return "00:00:00 XM";
  } else {

    String Hour = String(timeinfo.tm_hour, DEC);
    String Minute = String(timeinfo.tm_min, DEC);
    String Second = String(timeinfo.tm_sec, DEC);

    if (timeinfo.tm_hour > 12) {
      timestr[0] = '0' + ((timeinfo.tm_hour - 12)  / 10);
      timestr[1] = '0' + ((timeinfo.tm_hour - 12) % 10);
      timestr[9] = 'P';
    } else if (timeinfo.tm_hour == 12) {
      timestr[0] = '1';
      timestr[1] = '2';
      timestr[9] = 'P';
    }
    else if (timeinfo.tm_hour == 0) {
      timestr[0] = '1';
      timestr[1] = '2';
      timestr[9] = 'A';
    }
    else {
      timestr[0] = '0' + (timeinfo.tm_hour / 10);
      timestr[1] = '0' + (timeinfo.tm_hour % 10);
      timestr[9] = 'A';
    }

    timestr[3] = '0' + (timeinfo.tm_min / 10);
    timestr[4] = '0' + (timeinfo.tm_min % 10);

    timestr[6] = '0' + (timeinfo.tm_sec / 10);
    timestr[7] = '0' + (timeinfo.tm_sec % 10);

#ifdef DEBUG
    Serial.println("Obtained Time");
#endif
    correctTime = true;
  }

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);


  return timestr;
}

void drawDate(int x, int y, int textSize) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
#ifdef DEBUG
    Serial.println("Failed to obtain time");
#endif
    return;
  }

#ifdef DEBUG
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
#endif

  String weekday;

  switch (timeinfo.tm_wday) {
    case 0: weekday = "Sunday"; break;
    case 1: weekday = "Monday"; break;
    case 2: weekday = "Tuesday"; break;
    case 3: weekday = "Wednesday"; break;
    case 4: weekday = "Thursday"; break;
    case 5: weekday = "Friday"; break;
    case 6: weekday = "Saturday"; break;
    default: weekday = "error"; break;
  }

  String Date = weekday + ", " + String(timeinfo.tm_mon + 1) + "/" + String(timeinfo.tm_mday);

  tft.setTextSize(textSize);
  tft.setTextColor(TEXT_COLOR);
  for (int a = 0; a < Date.length(); a++) {
    tft.fillRect(
      x + a * 6 * textSize,
      y,
      6 * textSize,
      7 * textSize,
      BACKGROUND_COLOR
    );
    tft.setCursor(x + a * 6 * textSize, y );
    tft.print(Date[a]);
  }

}



void drawDateCentered(int y, int textSize) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
#ifdef DEBUG
    Serial.println("Failed to obtain time");
#endif
    return;
  }

#ifdef DEBUG
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
#endif

  String weekday;

  switch (timeinfo.tm_wday) {
    case 0: weekday = "Sunday"; break;
    case 1: weekday = "Monday"; break;
    case 2: weekday = "Tuesday"; break;
    case 3: weekday = "Wednesday"; break;
    case 4: weekday = "Thursday"; break;
    case 5: weekday = "Friday"; break;
    case 6: weekday = "Saturday"; break;
    default: weekday = "error"; break;
  }

  String Date = weekday + ", " + String(timeinfo.tm_mon + 1) + "/" + String(timeinfo.tm_mday);

  int x = (160 - (Date.length() * 6 * textSize)) / 2;

  tft.setTextSize(textSize);
  tft.setTextColor(TEXT_COLOR);
  for (int a = 0; a < Date.length(); a++) {
    tft.fillRect(
      x + a * 6 * textSize,
      y,
      6 * textSize,
      7 * textSize,
      BACKGROUND_COLOR
    );
    tft.setCursor(x + a * 6 * textSize, y );
    tft.print(Date[a]);
  }

}

void drawTime(int x, int y, int textSize)
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
#ifdef DEBUG
    Serial.println("Failed to obtain time");
#endif
    return;
  }

#ifdef DEBUG
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
#endif

  //  display.setFixedFont(ssd1306xled_font6x8);
  //  display.clear();

  // String Time = String(timeinfo.tm_hour + ":" + timeinfo.tm_min + ":" +  timeinfo.tm_sec);


  String Hour = String(timeinfo.tm_hour, DEC);
  String Minute = String(timeinfo.tm_min, DEC);
  String Second = String(timeinfo.tm_sec, DEC);

  char timestr[12] = "00:00:00 XM";
  if (timeinfo.tm_hour > 12) {
    timestr[0] = '0' + ((timeinfo.tm_hour - 12)  / 10);
    timestr[1] = '0' + ((timeinfo.tm_hour - 12) % 10);
    timestr[9] = 'P';
  } else if (timeinfo.tm_hour == 12) {
    timestr[0] = '1';
    timestr[1] = '2';
    timestr[9] = 'P';
  }
  else if (timeinfo.tm_hour == 0) {
    timestr[0] = '1';
    timestr[1] = '2';
    timestr[9] = 'A';
  }
  else {
    timestr[0] = '0' + (timeinfo.tm_hour / 10);
    timestr[1] = '0' + (timeinfo.tm_hour % 10);
    timestr[9] = 'A';
  }

  timestr[3] = '0' + (timeinfo.tm_min / 10);
  timestr[4] = '0' + (timeinfo.tm_min % 10);

  timestr[6] = '0' + (timeinfo.tm_sec / 10);
  timestr[7] = '0' + (timeinfo.tm_sec % 10);

  /*  when writing the time we assume that we're writing over something, so for each character
       we fill in a black box behind it exactly the required size. we do this to try and prevent character "flashing"
       as much as possible.  */
  tft.setTextSize(textSize);
  if (correctTime) {
    tft.setTextColor(TEXT_COLOR);
  } else {
    tft.setTextColor(ERROR_COLOR);
  }
  for (int a = 0; a < 11; a++) {
    tft.fillRect(
      x + a * 6 * textSize,
      y,
      6 * textSize,
      7 * textSize,
      BACKGROUND_COLOR
    );
    tft.setCursor(x + a * 6 * textSize, y );
    tft.print(timestr[a]);
  }

}
