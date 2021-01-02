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

  //temporary until settings configuration option is pulled in
  setDataField(0, DAYLIGHT_SAVINGS);
}



void watchDog(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  for (;;)
  {
    //    printDebug("***watchdog serviced***");
    if (millis() > lastTouchTime + 15000) {
      esp_sleep_enable_timer_wakeup(1);
      esp_deep_sleep_start();
    }
    vTaskDelay(500);
  }
}


void deviceSleep() {
  batteryPercentage = getBatteryPercentage();
  //re-enable touch wakeup
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); //1 = High, 0 = Low
  digitalWrite(LCD_LED, LOW);
  if (connected) {
    pClient->disconnect();
    while (connected) {
      delay(1);
    };
    printDebug("disconnected");
  }
  Serial.flush();

  //put display to sleep
  tft.enableSleep(true);

  esp_light_sleep_start();
}

void clearTask(TaskHandle_t handle) {
  if (handle != NULL) {
    vTaskDelete(handle);
  }
}


void loop() {
  //do everything we need to on wakeup.
  onWakeup();

  //the current page is set by a void pointer, this pointer can be reassigned to new pages.
  //using this approach creating new pages should be much easier since they're more-or-less self contained
  //all pages draw to the framebuffer then the buffer is drawn at the end
  while (millis() < lastTouchTime + 10000) {
    ((void(*)())currentPage)();
    tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  printDebug("Going to sleep");
  deviceSleep();
}


void onWakeup() {
  getRTCTime();
  printRTCTime();
  //wake up display
  tft.enableSleep(false);
  digitalWrite(LCD_LED, HIGH);
  //initalizes BLE connection in seperate thread
  //when connected will update the "connected" variable to true
  initBLE();
}

void drawNotifications(String notificationData, int x, int y, int color) {
  //count lines
  int lineCount = 0;
  for (int a = 0; a < notificationData.length(); a++) {
    if (notificationData[a] == '\n') {
      lineCount++;
    }
  }
  //last 2 lines are not notification data, they're time and EOM terminator
  lineCount = lineCount - 1;

  frameBuffer->setTextColor(color);
  frameBuffer->setCursor(x, y);

  for (int a = 0; a < lineCount; a++) {
    String line = getValue(notificationData, '\n', a);
    frameBuffer->println(getValue(line, ';', 0));
  }
}
