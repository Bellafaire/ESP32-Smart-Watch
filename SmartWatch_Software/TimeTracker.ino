boolean correctTime = true;


//header file for the time.h library https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-timer.h (closest thing to documentation I've found so far)


//found here https://www.esp32.com/viewtopic.php?t=5398 (with some changes for timezone)
void mjd_set_timezone_est()
{
  ESP_LOGD(TAG, "%s()", __FUNCTION__);

  // @doc https://remotemonitoringsystems.ca/time-zone-abbreviations.php
  // @doc timezone UTC = UTC
  setenv("TZ", "EST5EDT", 1);
  tzset();
}

String getInternetTime()
{
  //connect to WiFi
  int wifiCounter = 0;

  mjd_set_timezone_est();

  //i need these values to be strings so some extra code is required
  WiFi.mode(WIFI_STA);
  char ssid0[60];
  char password0[60];
  ssid.toCharArray(ssid0, ssid.length() + 1);
  password.toCharArray(password0, password.length() + 1);

#ifdef DEBUG
  Serial.println("Connecting to " + ssid);
#endif

  WiFi.begin(ssid0, password0);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
#ifdef DEBUG
    Serial.print(".");
#endif
    if (++wifiCounter > 30)
    {
#ifdef DEBUG
      Serial.println("COULD NOT CONNECT TO WIFI ");
#endif
      break;
    }
  }
#ifdef DEBUG
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println(" CONNECTED");
  }
#endif
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  delay(1000);
  printLocalTime();
  unsigned long tm0 = micros();
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  //  bootCount++;
  //  beginTimedSleep (tm0);
  return "00:00:00AM";
}

void printLocalTime()
{
  time(&now);
  timeinfo = localtime(&now);
#ifdef DEBUG
  Serial.printf("%s\n", asctime(timeinfo));
  delay(2); // 26 bytes @ 115200 baud is less than 2 ms
#endif
}

void drawDate(int x, int y, int textSize)
{
  //configure current timezone (this information gets lost in deep sleep)
  mjd_set_timezone_est();
  time(&now);
  timeinfo = localtime(&now);

  String weekday;

  switch (timeinfo->tm_wday)
  {
  case 0:
    weekday = "Sunday";
    break;
  case 1:
    weekday = "Monday";
    break;
  case 2:
    weekday = "Tuesday";
    break;
  case 3:
    weekday = "Wednesday";
    break;
  case 4:
    weekday = "Thursday";
    break;
  case 5:
    weekday = "Friday";
    break;
  case 6:
    weekday = "Saturday";
    break;
  default:
    weekday = "error";
    break;
  }

  String Date = weekday + ", " + String(timeinfo->tm_mon + 1) + "/" + String(timeinfo->tm_mday);

  frameBuffer->setTextSize(textSize);
  frameBuffer->setTextColor(TEXT_COLOR);
  for (int a = 0; a < Date.length(); a++)
  {
    frameBuffer->fillRect(
        x + a * 6 * textSize,
        y,
        6 * textSize,
        7 * textSize,
        BACKGROUND_COLOR);
    frameBuffer->setCursor(x + a * 6 * textSize, y);
    frameBuffer->print(Date[a]);
  }
}

void drawDateCentered(int y, int textSize)
{
  //configure current timezone (this information gets lost in deep sleep)
  mjd_set_timezone_est();
  time(&now);
  timeinfo = localtime(&now);

  String weekday;

  switch (timeinfo->tm_wday)
  {
  case 0:
    weekday = "Sunday";
    break;
  case 1:
    weekday = "Monday";
    break;
  case 2:
    weekday = "Tuesday";
    break;
  case 3:
    weekday = "Wednesday";
    break;
  case 4:
    weekday = "Thursday";
    break;
  case 5:
    weekday = "Friday";
    break;
  case 6:
    weekday = "Saturday";
    break;
  default:
    weekday = "error";
    break;
  }

  String Date = weekday + ", " + String(timeinfo->tm_mon + 1) + "/" + String(timeinfo->tm_mday);

  int x = (160 - (Date.length() * 6 * textSize)) / 2;

  frameBuffer->setTextSize(textSize);
  frameBuffer->setTextColor(TEXT_COLOR);
  for (int a = 0; a < Date.length(); a++)
  {
    frameBuffer->fillRect(
        x + a * 6 * textSize,
        y,
        6 * textSize,
        7 * textSize,
        BACKGROUND_COLOR);
    frameBuffer->setCursor(x + a * 6 * textSize, y);
    frameBuffer->print(Date[a]);
  }
}

void drawTime(int x, int y, int textSize)
{
  //configure current timezone (this information gets lost in deep sleep)
  mjd_set_timezone_est();
  time(&now);
  timeinfo = localtime(&now);

  String Hour = String(timeinfo->tm_hour, DEC);
  String Minute = String(timeinfo->tm_min, DEC);
  String Second = String(timeinfo->tm_sec, DEC);

  byte hour, minute, second = 0;
  hour = timeinfo->tm_hour;
  minute = (timeinfo->tm_min);
  second = timeinfo->tm_sec;

  char timestr[12] = "00:00:00 XM";
  if (timeinfo->tm_hour > 12)
  {
    timestr[0] = '0' + ((hour - 12) / 10);
    timestr[1] = '0' + ((hour - 12) % 10);
    timestr[9] = 'P';
  }
  else if (timeinfo->tm_hour == 12)
  {
    timestr[0] = '1';
    timestr[1] = '2';
    timestr[9] = 'P';
  }
  else if (timeinfo->tm_hour == 0)
  {
    timestr[0] = '1';
    timestr[1] = '2';
    timestr[9] = 'A';
  }
  else
  {
    timestr[0] = '0' + (timeinfo->tm_hour / 10);
    timestr[1] = '0' + (timeinfo->tm_hour % 10);
    timestr[9] = 'A';
  }

  timestr[3] = '0' + (timeinfo->tm_min / 10);
  timestr[4] = '0' + (timeinfo->tm_min % 10);

  timestr[6] = '0' + (timeinfo->tm_sec / 10);
  timestr[7] = '0' + (timeinfo->tm_sec % 10);

  /*  when writing the time we assume that we're writing over something, so for each character
       we fill in a black box behind it exactly the required size. we do this to try and prevent character "flashing"
       as much as possible.  */
  frameBuffer->setTextSize(textSize);
  if (correctTime)
  {
    frameBuffer->setTextColor(TEXT_COLOR);
  }
  else
  {
    frameBuffer->setTextColor(ERROR_COLOR);
  }
  for (int a = 0; a < 11; a++)
  {
    frameBuffer->fillRect(
        x + a * 6 * textSize,
        y,
        6 * textSize,
        7 * textSize,
        BACKGROUND_COLOR);
    frameBuffer->setCursor(x + a * 6 * textSize, y);
    frameBuffer->print(timestr[a]);
  }
}
