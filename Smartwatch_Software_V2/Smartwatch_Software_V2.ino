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
boolean wasActive = false;
RTC_DATA_ATTR int sleepCount = 0;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  EEPROM.begin(EEPROM_SIZE);
  Wire.begin(I2C_SDA, I2C_SCL, 100000);
  initLCD();
  initBatMonitor();
  initTouch();



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
  esp_sleep_enable_timer_wakeup(TIMER_SLEEP_TIME);
  digitalWrite(LCD_LED, LOW);
  if (connected) {

    BLEAddress devices = myDevice->getAddress();

    if (esp_ble_gap_disconnect((uint8_t*)devices.getNative()) == ESP_OK) {
      printDebug("%%%%%%%%%%%%%%% CLOSED BLE CLIENT CONNECTION %%%%%%%%%%%%%%%%%%");
    }

    unsigned long disconnectTimeout = millis() + 1000;

    while (connected && millis() < disconnectTimeout) {
      vTaskDelay(10);
    }

    if (millis() >= disconnectTimeout) {
      connected = false;
      printDebug("Device disconnected improperly");
    }

    eTaskState con = eTaskGetState(xConnect);
    if (con == 0) {
      printDebug("Deleting Bluetooth Task");
      vTaskDelete(xConnect);
    }



    printDebug("disconnected");
  }

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
    printDebug("Going to sleep");
  }

  Serial.flush();

  //put display to sleep
  tft.enableSleep(true);

  if (sleepCount < 3) {
    sleepCount = wasActive ? sleepCount + 1 : sleepCount;
    esp_light_sleep_start();
  } else {
    sleepCount = 0;
    printDebug("Deep Sleeping for restart");
    esp_sleep_enable_timer_wakeup(1);
    esp_deep_sleep_start();
  }
}

void loop() {
  //do everything we need to on wakeup.
  wakeup_reason = esp_sleep_get_wakeup_cause();

  wasActive = false;
  //check the wakeup reason, if it's touch we go right into the main loop.
  //if it's timer then we're checking whether the accelerometer is in the proper threshold region.
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0
      || readZAccel() > ACCELEROMETER_WAKEUP_THRESHOLD) {
    if (readZAccel() > ACCELEROMETER_WAKEUP_THRESHOLD) {
      lastTouchTime = millis() - (TAP_WAKE_TIME - 1000);
    }

    wasActive = true;

    //do all the normal things we have to do when the device wakes up
    onWakeup();

    //the current page is set by a void pointer, this pointer can be reassigned to new pages.
    //using this approach creating new pages should be much easier since they're more-or-less self contained
    //all pages draw to the framebuffer then the buffer is drawn at the end
    //stays awake for 15 if touched or until the z axis no longer meets the threshold - 100
    while (millis() < lastTouchTime + TAP_WAKE_TIME || readZAccel() > ACCELEROMETER_STAY_AWAKE_THRESHOLD) {

      //specific elements need to bypass the loop thread drawing so that they can
      //have more direct control of the display for short periods of time.
      if (drawInLoop) {
        ((void(*)())currentPage)();
        tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
      }

      //if we're connected and haven't updated our notification data then lets do so
      if (connected && !notificationsUpdated) {

        //gets current android notifications as a string
        notificationData = sendBLE("/notifications", true);

        //if successful then parse out the time and register a successful notification acquisition
        if (notificationData.length() > 10) {
          updateTimeFromNotificationData(notificationData);
          notificationsUpdated = true;
        }
      }
    }
  }
  deviceSleep();

}


void onWakeup() {
  //new wakeup so we'll want to update notifications next chance we get 
  notificationsUpdated = false;
  getRTCTime();
  printRTCTime();
  //wake up display
  tft.enableSleep(false);
  digitalWrite(LCD_LED, HIGH);

  //always start on the home page when waking up
  currentPage = (void*)initHome;
  //initalizes BLE connection in seperate thread
  //when connected will update the "connected" variable to true
  initBLE();
}
