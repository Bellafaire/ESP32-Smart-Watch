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

#include "Declarations.h"

int batteryPercentage = 100;
esp_sleep_wakeup_cause_t wakeup_reason;
boolean deepSleepWake = true;
boolean notificationsUpdated = false;
boolean timeUpdated = false;
boolean wasActive = false;
int sleepCount = 0;
boolean displayTimeOnly = false;
int backlightBrightness = 0;
unsigned long wakeStart = 0;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  EEPROM.begin(EEPROM_SIZE);
  Wire.begin(I2C_SDA, I2C_SCL, 100000);
  pinMode(CHG_STAT, INPUT);
  initLCD();
  initBatMonitor();
  initTouch();

  initBLE();

  loadEEPROMSettings();

  //create "watchdog task" to put the device in deepsleep if something goes wrong
  xTaskCreatePinnedToCore(    watchDog
                              ,  "watchdog"
                              ,  1024  // Stack size
                              ,  NULL
                              ,  3  // Priority
                              ,  NULL
                              ,  1);

  //the currentPage variable controls which page is currently being displayed.
  currentPage = (void*)initHome;
}


//watchdog tasks, it's possible some of the code can bind up. This at least prevents
//the user from having to manually press the reset button
void watchDog(void *pvParameters)
{
  (void) pvParameters;

  for (;;)
  {
    //    printDebug("***watchdog serviced***");
    if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
      if (millis() > lastTouchTime + 20000
          &&  readZAccel() < ACCELEROMETER_STAY_AWAKE_THRESHOLD) {
        esp_sleep_enable_timer_wakeup(1);
        esp_deep_sleep_start();
      }
    }
    vTaskDelay(500);
  }
}



void deviceSleep() {
  batteryPercentage = getBatteryPercentage();
  //re-enable touch wakeup
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); //1 = High, 0 = Low

  //touch only doesn't require occasional wakeup, lets not waste power on it
  if (SETTING_WAKEUP_TYPE != WAKEUP_TOUCH_ONLY) {
    esp_sleep_enable_timer_wakeup(TIMER_SLEEP_TIME);
  }

  setBacklight(0);

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
    printDebug("Going to sleep");
  }

  //kill running tasks
  if (xTouch != NULL) {
    vTaskDelete(xTouch);
  }
  Serial.flush();

  //put display to sleep
  tft.enableSleep(true);

  if (wasActive && !connected) {
    esp_deep_sleep_start();
  }
  connected = false;
  sleepCount++;
  esp_light_sleep_start();
}

void loop() {
  wasActive = false;

  //check the wakeup reason, if it's touch we go right into the main loop.
  //if it's timer then we're checking whether the accelerometer is in the proper threshold region.
  if (wakeupCheck()) {
    backlightBrightness = 0;
    lastTouchTime = millis() - (TAP_WAKE_TIME - 1000);
    wakeStart = millis();

    wasActive = true;

    //do all the normal things we have to do when the device wakes up
    onWakeup();

    //the current page is set by a void pointer, this pointer can be reassigned to new pages.
    //using this approach creating new pages should be much easier since they're more-or-less self contained
    //all pages draw to the framebuffer then the buffer is drawn at the end
    //stays awake for 15 if touched or until the z axis no longer meets the threshold - 100
    while (millis() < lastTouchTime + TAP_WAKE_TIME || (readZAccel() > ACCELEROMETER_STAY_AWAKE_THRESHOLD)) {
      //fade the backlight on
      setBacklight(constrain(map((millis() - wakeStart), 0, 3000, 0, 255), 0, 255));

      //specific elements need to bypass the loop thread drawing so that they can
      //have more direct control of the display for short periods of time.
      if (drawInLoop) {
        ((void(*)())currentPage)();
        tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
      }

      if ((currentPage != (void*)initTimeOnly) && (currentPage != (void*)timeOnly)) {
        //if we're connected and haven't updated our notification data then lets do so
        if (connected && !notificationsUpdated) {
          boolean success = sendBLE("/notifications", &notificationData, false);
          if (success) {
            notificationsUpdated = true;
            printDebug("notifications updating");
          }
        }
        if (connected && !timeUpdated) {
          //gets current android notifications as a string
          String timeStr = "";
          boolean success = sendBLE("/time", &timeStr, true);
          if (success) {
            updateTimeFromTimeString(timeStr);
            timeUpdated = true;
          }
        }
      }
    }
  }
  deviceSleep();
}


//byte SETTING_DAYLIGHT_SAVINGS = 0;
//byte SETTING_WAKEUP_TYPE = 0;
//
//
////wakeup type defines
//#define WAKEUP_TOUCH_ONLY 0
//#define WAKEUP_ACCELEROMETER 1
//#define WAKEUP_ACCELEROMTER_DISPLAY_TIME 2


//checks wakeup conditions and determines whether or not to activate the watch.
boolean wakeupCheck() {
  //get the wakeup reason, if it's a touch interrupt we'll know from this
  wakeup_reason = esp_sleep_get_wakeup_cause();

  displayTimeOnly = false;

  //read accelerometer once, less ADC reads will speed things up slightly. (not polling the ADC for every if statement)
  int yAccel = readYAccel();
  int zAccel = readZAccel();

  //kind of a convoluted algorithim to attempt to filter any movements that aren't what we want.
  //this function checks for touch wakeup and for accelerometer wakeup, the accelerometer wakeup being the more complicated
  //what we want is the user to "dip" their wrist down (turn the watch so the screen is facing opposite their body perpendicular to the ground)
  //then bring the watch up to a level position. This entire action must be completed within the timespan of DIP_ACTION_LENGTH the
  //watch must then remain level for at least VERTICAL_ACTION_LENGTH in order for an accelerometer wakeup to be activated.
  //the threshold, accelerometer, timing, and other control variables are present in the Declarations.h file under the "SLEEP and Wake" section

  //wakeup value to be returned
  boolean accelerometer_wakeup = false;

  //first we check for the dip, if it's values are recognized within the given threshold then the
  //user may be starting the wakeup process.
  if (ACCEL_Y_DIP < yAccel + DIP_THRESHOLD_VALUE
      &&  ACCEL_Y_DIP > yAccel - DIP_THRESHOLD_VALUE
      && ACCEL_Z_DIP < zAccel + DIP_THRESHOLD_VALUE
      && ACCEL_Z_DIP > zAccel - DIP_THRESHOLD_VALUE) {

    //store the time we last saw the dip, we rely on timing heavily in determining the dip action
    lastDipRecognized = millis();
  }

  //now check if the user has the watch horizontal within the given threshold.
  //also check whether or not the timer is already running (dip has been active for a certain time)
  //if so we want to let the timer run longer since it may be that the action is in progress.
  if (ACCEL_Y_VERT < yAccel + DIP_THRESHOLD_VALUE
      &&  ACCEL_Y_VERT > yAccel - DIP_THRESHOLD_VALUE
      && ACCEL_Z_VERT < zAccel + DIP_THRESHOLD_VALUE
      && ACCEL_Z_VERT > zAccel - DIP_THRESHOLD_VALUE
      && millis() - verticalStarted > DIP_ACTION_LENGTH) {

    //if detected and not running save the time the watch started being level.
    verticalStarted = millis();
  }

  //now check the timers again, if the last dip was under the required timing value and the watch has been vertical
  //for the required timing value but still less than the required time for the dip action then wake the device up.
  //also check that the vertical action happened AFTER the dip, otherwise we'd be turning the screen on when the user looked away from it
  if ( ((millis() - lastDipRecognized) < DIP_ACTION_LENGTH)
       && ((millis() - verticalStarted) > VERTICAL_ACTION_LENGTH)
       && ((millis() - verticalStarted) < DIP_ACTION_LENGTH)
       && lastDipRecognized < verticalStarted) {

    //set wakeup condition
    accelerometer_wakeup = true;
  }

  //debug spam
  //    printDebug("X: " + String(readXAccel()) + " Y: " + String(readYAccel()) + " Z: " + String(readZAccel()) + " millis(): " + String(millis()) + " lastDip:" + String(millis() - lastDipRecognized) + " verticalStarted:" + String(millis() - verticalStarted));

  //if the accelerometer condition was recognized or user touched the screen then wakeup the device.

  switch (SETTING_WAKEUP_TYPE) {
    case WAKEUP_TOUCH_ONLY:
      return (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0);
      break;
    case WAKEUP_ACCELEROMETER:
      return ((wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) || accelerometer_wakeup) && (sleepCount != 0);
      break;
    case WAKEUP_ACCELEROMTER_DISPLAY_TIME:
      if (wakeup_reason != ESP_SLEEP_WAKEUP_EXT0) {
        displayTimeOnly = true;
      }
      return ((wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) || accelerometer_wakeup) && (sleepCount != 0);
      break;
    default:
      break;
  }
  return false;
}



void onWakeup() {
  //new wakeup so we'll want to update notifications next chance we get
  notificationsUpdated = false;
  timeUpdated = false;

  //incase the device was shut down in the middle of a bluetooth operation since
  //the android device will not complete the operation and update the ESP32
  operationInProgress = false;

  getRTCTime();
  printRTCTime();
  //wake up display
  tft.enableSleep(false);

  if (displayTimeOnly) {
    currentPage = (void*)initTimeOnly;
  } else {
    //always start on the home page when waking up
    currentPage = (void*)initHome;
  }

}
