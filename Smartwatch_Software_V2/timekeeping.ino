//found here https://www.esp32.com/viewtopic.php?t=5398 (with some changes for timezone)
void mjd_set_timezone_est()
{
  ESP_LOGD(TAG, "%s()", __FUNCTION__);

  // @doc https://remotemonitoringsystems.ca/time-zone-abbreviations.php
  // @doc timezone UTC = UTC
  setenv("TZ", "EST5EDT", 1);
  tzset();
}


void updateTimeFromNotificationData(String notificationString) {
  //check that the notification data we have is complete
  //first count the number of line breaks in the string
  int lineCount = 0;
  for (int a = 0; a < notificationString.length(); a++) {
    if (notificationString[a] == '\n') {
      lineCount++;
    }
  }

  String lastNotificationLine = getValue(notificationString, '\n', lineCount);
  if (lastNotificationLine[0] == '*' &&
      lastNotificationLine[1] == '*' &&
      lastNotificationLine[2] == '*') {

    //get the last line of the notification string, that will contain the current time,
    //note that this function should only be called directly after receiving a notification update
    //otherwise the time will be inaccurate
    String timeString = getValue(notificationString, lineCount - 1, 0);
    printDebug("Parsed Time String - " + timeString);

    //format "HH:mm:ss dd-MM-yyyy"
    //simply parse out the data from the string, some adjustments need to be made for year and month.
    struct tm tim;

    //time of year
    tim.tm_year = (String(timeString[15]) + String(timeString[16]) + String(timeString[17]) + String(timeString[18])).toInt() - 1900;
    tim.tm_mon = (String(timeString[12]) + String(timeString[13])).toInt() - 1; //month is zero indexed...
    tim.tm_mday = (String(timeString[9]) + String(timeString[10])).toInt();

    //time of day
    tim.tm_hour = (String(timeString[0]) + String(timeString[1])).toInt() + readDataField(DAYLIGHT_SAVINGS);
    tim.tm_min = (String(timeString[3]) + String(timeString[4])).toInt();
    tim.tm_sec = (String(timeString[6]) + String(timeString[7])).toInt();

    //make sure timezone is set to EST
    mjd_set_timezone_est();

    //convert the time struct into time_t variable (essentially an integer representation of time)
    time_t t = mktime(&tim);

    printDebug("Setting time: " + String(asctime(&tim)));


    //place the time_t variable into a timeval and update the RTC clock so that the time can be continued in deep sleep
    struct timeval tv = { .tv_sec = t };
    settimeofday(&tv, NULL);
    getRTCTime();
  } else {
    printDebug("Notification data not complete, cannot parse time");

  }
}

time_t getRTCTime() {
  setenv("TZ", "EST5EDT", 1);
  tzset();
  time_t t;
  timeval tval;
  gettimeofday(&tval, NULL);
  t = tval.tv_sec;
  timeinfo = localtime(&t);
  return t;
}


void printRTCTime() {
  time_t t = getRTCTime();
  printDebug("NOW: " + String(t));
  timeinfo = localtime(&t);
  printDebug("Current RTC Time: " + getDateString(timeinfo) + " " + getTimeString(timeinfo));
}

//checks if the given value takes 2 characters to represented as a string
//if it is only 1 character this function appends a leading zero (useful in time formatting)
String twoCharacters(int value) {
  String r = String(value, DEC);
  if (r.length() < 2) {
    r = "0" + r;
  }
  return r;
}

String getTimeString(struct tm* timeinfo) {
  setenv("TZ", "EST5EDT", 1);
  tzset();
  String hour =  twoCharacters(timeinfo->tm_hour);
  String minute = twoCharacters(timeinfo->tm_min);
  String seconds =  twoCharacters(timeinfo->tm_sec);

  String timeString =
    hour  + ":"
    + minute + ":"
    + seconds;
  return  timeString;
}

String getDateString(struct tm* timeinfo) {
  setenv("TZ", "EST5EDT", 1);
  tzset();
  String month = twoCharacters(timeinfo->tm_mon + 1);
  String day = twoCharacters(timeinfo->tm_mday);
  String year = twoCharacters(timeinfo->tm_year + 1900);

  String dateString = month + "-"
                      + day + "-"
                      + year;
  return dateString;
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


void drawTime(int x, int y, int textSize, int color)
{
  //configure current timezone (this information gets lost in deep sleep)
  mjd_set_timezone_est();
  getRTCTime();
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

  frameBuffer->setTextSize(textSize);

  frameBuffer->setTextColor(color);

  for (int a = 0; a < 11; a++)
  {
    frameBuffer->setCursor(x + a * 6 * textSize, y);
    frameBuffer->print(timestr[a]);
  }
  frameBuffer->setTextSize(1);
}
