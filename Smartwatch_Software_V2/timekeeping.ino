/*****************************************************************************
  The MIT License (MIT)
  Copyright (c) 2020 Matthew James Bellafaire
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
******************************************************************************/

//found here https://www.esp32.com/viewtopic.php?t=5398 (with some changes for timezone)
void mjd_set_timezone_est()
{
  ESP_LOGD(TAG, "%s()", __FUNCTION__);

  // @doc https://remotemonitoringsystems.ca/time-zone-abbreviations.php
  // @doc timezone UTC = UTC
  setenv("TZ", "EST5EDT", 1);
  tzset();
}

//parses time out of the notification data obtained from the phone
void updateTimeFromTimeString(String timeString) {

  printDebug("Parsing time from " + timeString);

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
}

//pulls the time from the ESP32's onboard RTC
time_t getRTCTime() {
  //set the timezone of the ESP32, this information is generally lost during deepsleep
  setenv("TZ", "EST5EDT", 1);
  tzset();

  //create time holder variables
  time_t t;
  timeval tval;

  //get time from the RTC
  gettimeofday(&tval, NULL);

  //convert to time values we can actually use
  t = tval.tv_sec;           //seconds since epoch
  timeinfo = localtime(&t);  //time struct

  //return seconds since epoch
  return t;
}


//prints RTC to serial terminal when in debug mode
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


//converts time struct to a string for display on the LCD
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

//converts time struct to date for display on the LCD
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


//draws the gime at a given location on the LCD using the frameBuffer
void drawTime(int x, int y, int textSize, int color, int shadowOffset)
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
    frameBuffer->setTextColor(BACKGROUND_COLOR);
    frameBuffer->setCursor(x + a * 6 * textSize + shadowOffset, y + shadowOffset);
    frameBuffer->print(timestr[a]);

    frameBuffer->setTextColor(color);
    frameBuffer->setCursor(x + a * 6 * textSize, y);
    frameBuffer->print(timestr[a]);
  }
  frameBuffer->setTextSize(1);
}

//draws the current date to the LCD at a given location utilizing the frameBuffer
void drawDate(int x, int y, int textSize)
{
  //configure current timezone (this information gets lost in deep sleep)
  getRTCTime();

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
    frameBuffer->setCursor(x + a * 6 * textSize, y);
    frameBuffer->print(Date[a]);
  }
}

//draws the date centered on the screen at a given y position (Uses framebuffer)
void drawDateCentered(int y, int textSize)
{
  //configure current timezone (this information gets lost in deep sleep)
  getRTCTime();

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
    frameBuffer->setCursor(x + a * 6 * textSize, y);
    frameBuffer->print(Date[a]);
  }
}
