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
  Wire.begin(I2C_SDA, I2C_SCL, 400000);
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
                              ,  0);

  currentPage = (void*)initHome;

  if (CLEAR_TOUCH_CALIBRATION) {
    //rapid touch shutdown also clears calibration data. This is to prevent a case where the calibration
    //was messed up and resulted in the device being unuseable.
    for (int a = X_MAX; a < Y_MIN + 2; a++) {
      setDataField(0, a);
      loadEEPROMSettings();
    }
  }


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

void updateDisplay(void *pvParameters)
{
  (void) pvParameters;
  tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  vTaskDelete(NULL);
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
      setBacklight(constrain(map((millis() - wakeStart), 0, 3000, 0, SETTING_SCREEN_BRIGHTNESS), 10, SETTING_SCREEN_BRIGHTNESS));

      //specific elements need to bypass the loop thread drawing so that they can
      //have more direct control of the display for short periods of time.
      if (drawInLoop) {
        ((void(*)())currentPage)();
        xTaskCreatePinnedToCore(    updateDisplay
                                    ,  "display"
                                    ,  48 * 1024 // Stack size
                                    ,  NULL
                                    ,  2  // Priority
                                    ,  NULL
                                    ,  0);
        //give some time to other freertos tasks running outside of the main loop
        vTaskDelay(10);
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

      //printDebug("X: " + String(readXAccel()) + " Y: " + String(readYAccel()) + " Z: " + String(readZAccel()) + " millis(): " + String(millis()) );
    }
  }
  deviceSleep();
}


int accelWakeupState = 0;

//checks wakeup conditions and determines whether or not to activate the watch.
boolean wakeupCheck() {
  //get the wakeup reason, if it's a touch interrupt we'll know from this
  wakeup_reason = esp_sleep_get_wakeup_cause();
  displayTimeOnly = false;

  //read accelerometer once, less ADC reads will speed things up slightly. (not polling the ADC for every if statement)
  int yAccel = readYAccel();
  int zAccel = readZAccel();


  /*accelerometer wakeup algorithim implements a state machine in order to determine when to wakeup
    S0 -> S1 - watch is perpendicular to the ground
    S1 -> S2 - watch is perpendicular to the ground
    S2 -> S3 - watch is perpendicular to the ground (Stay in state 3 as long as watch is in this position)
    S3 -> S4 - either watch screen is facing up or watch screen is neither up or down.
    S4 -> S5 - watch is facing up
    S5 -> S6 - watch is facing up
    S6 -> S7 - watch is facing up
    S7 - activation of watch from accelerometer data.
    State machine goes to S0 if the conditions to move to the next state are not met.
  */
  //check for dip (if user is holding wrist out so screen is perpendicular to the ground)
  if (accelWakeupState < 3 &&
      isDown(yAccel, zAccel)) {
    accelWakeupState++;
  }
  else if (isDown(yAccel, zAccel)) {
    //do nothing, we want to stay in this state for now.
  }
  //basically we're trying to catch if the user flicks their wrist up, sometimes the watch
  //can come out of sleep in a way that it misinterprets the flick motion
  else if (!isDown(yAccel, zAccel) && !isUp(yAccel, zAccel) && accelWakeupState == 3) {
    accelWakeupState++;
  }
  //check if watch screen is facing straight up
  else if (accelWakeupState >= 3 &&
           isUp(yAccel, zAccel)
          ) {
    accelWakeupState++;
  } else {
    accelWakeupState = 0;
  }

  boolean accelerometer_wakeup = accelWakeupState > 6;
  if (accelerometer_wakeup) {
    accelWakeupState = 0;
  }
  //debug spam, use if working on this algorithim
  //    printDebug("X: " + String(readXAccel()) + " Y: " + String(readYAccel()) + " Z: " + String(readZAccel()) + " millis(): " + String(millis()) + " state:" + String(accelWakeupState) );


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

  //clear the calculator active flag if it was set to true
  calculatorActive = false;

  getRTCTime();
  printRTCTime();
  //wake up display
  tft.enableSleep(false);

  if (displayTimeOnly) {
    currentPage = (void*)initTimeOnly;
  } else {
    //check whether we have calibration data for a screen, a fresh ESP32 would not have any data so that needs
    //to be acquired immediately before the device can be used.
    if ((SETTING_X_MAX == SETTING_X_MIN) || (SETTING_Y_MAX == SETTING_Y_MIN)) {
      currentPage = (void*)initCalibration;
    } else {
      //the currentPage variable controls which page is currently being displayed.
      currentPage = (void*)initHome;
    }
  }

}
