//found here https://www.esp32.com/viewtopic.php?t=5398 (with some changes for timezone)
void mjd_set_timezone_est()
{
  ESP_LOGD(TAG, "%s()", __FUNCTION__);

  // @doc https://remotemonitoringsystems.ca/time-zone-abbreviations.php
  // @doc timezone UTC = UTC
  setenv("TZ", "EST5EDT", 1);
  tzset();
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

  tim.tm_hour = (String(timeString[0]) + String(timeString[1])).toInt() + (SETTING_DAYLIGHT_SAVINGS);
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

boolean getTimeFromBLE() {
  if (connected) {
    if (!timeUpdated) {
      //gets current android notifications as a string
      String timeStr = "";
      boolean success = sendBLE("/time", &timeStr, true);
      if (success) {
        updateTimeFromTimeString(timeStr);
        timeUpdated = true;
      } else {
        return false;
      }
    } else {
      printDebug("Time already updated");
    }
  } else {
    printDebug("BLE Not connected");
    return false;
  }
  return true;
}