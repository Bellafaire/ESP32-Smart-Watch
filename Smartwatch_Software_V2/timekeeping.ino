//found here https://www.esp32.com/viewtopic.php?t=5398 (with some changes for timezone)
void mjd_set_timezone_est()
{
  ESP_LOGD(TAG, "%s()", __FUNCTION__);

  // @doc https://remotemonitoringsystems.ca/time-zone-abbreviations.php
  // @doc timezone UTC = UTC
  setenv("TZ", "EST5EDT", 1);
  tzset();
}


////referenced time update from https://www.esp32.com/viewtopic.php?t=6043
//void updateTimeFromNotificationData() {
//  //check that the notification data we have is complete
//  String lastNotificationLine = parseFromNotifications(getNotificationLines(), 0);
//  if (lastNotificationLine[0] == '*' &&
//      lastNotificationLine[1] == '*' &&
//      lastNotificationLine[2] == '*') {
//
//    //get the last line of the notification string, that will contain the current time,
//    //note that this function should only be called directly after receiving a notification update
//    //otherwise the time will be inaccurate
//    String timeString = parseFromNotifications(getNotificationLines() - 1, 0);
//    printDebug("Parsed Time String - " + timeString);
//
//    //format "HH:mm:ss dd-MM-yyyy"
//    //simply parse out the data from the string, some adjustments need to be made for year and month.
//    struct tm tim;
//
//    //time of year
//    tim.tm_year = (String(timeString[15]) + String(timeString[16]) + String(timeString[17]) + String(timeString[18])).toInt() - 1900;
//    tim.tm_mon = (String(timeString[12]) + String(timeString[13])).toInt() - 1; //month is zero indexed...
//    tim.tm_mday = (String(timeString[9]) + String(timeString[10])).toInt();
//
//    //time of day
//    tim.tm_hour = (String(timeString[0]) + String(timeString[1])).toInt() + readDataField(DAYLIGHT_SAVINGS);
//    tim.tm_min = (String(timeString[3]) + String(timeString[4])).toInt();
//    tim.tm_sec = (String(timeString[6]) + String(timeString[7])).toInt();
//
//    //make sure timezone is set to EST
//    mjd_set_timezone_est();
//
//    //convert the time struct into time_t variable (essentially an integer representation of time)
//    time_t t = mktime(&tim);
//
//    printDebug("Setting time: " + String(asctime(&tim)));
//
//
//    //place the time_t variable into a timeval and update the RTC clock so that the time can be continued in deep sleep
//    struct timeval tv = { .tv_sec = t };
//    settimeofday(&tv, NULL);
//
//  } else {
//    printDebug("Notification data not complete, cannot parse time");
//
//  }
//}
