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
  xTaskCreatePinnedToCore(
    watchDog
    ,  "watchdog"
    ,  1024  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL
    ,  1);

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
  onWakeup();
  active();
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

int ta;

void testFunction() {
  printDebug("************ TEST FUNCTION CALLED ***********");
  deactivateTouchArea(ta);
}

void active() {


  String notificationData = "";

  ta = createTouchArea(0, 0, 50, 50, (void*)testFunction);

  AnimationCircle circ1 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 20, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), 3.5, 2);
  AnimationCircle circ2 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 25, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), -3, 3);
  AnimationCircle circ3 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 31, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), 2.5, 4);
  AnimationCircle circ4 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 38, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), -2, 5);
  AnimationCircle circ5 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 45, 3, RGB_TO_BGR565(150, 150, 150), RGB_TO_BGR565(0, 0, 255), 1.5, 6);


  while (millis() < lastTouchTime + 10000) {

    if (connected) {
      circ1.setColor(RGB_TO_BGR565(0, 255, 0));
    } else if (myDevice) {
      circ1.setColor(RGB_TO_BGR565(0, 0, 255));
    } else {
      circ1.setColor(RGB_TO_BGR565(255, 0, 0));
    }

    if (connected && notificationData.length() < 10) {
      notificationData = sendBLE("/notifications", true); //gets current android notifications as a string
      updateTimeFromNotificationData(notificationData);
    }

    frameBuffer->drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

    drawTime(3, 5, 2, 0xFFFF, 1);
    drawDateCentered(20, 1);

    frameBuffer->setTextColor(0x0000);
    frameBuffer->setCursor(SCREEN_WIDTH - 6 * 4, 5 );
    frameBuffer->println(String(batteryPercentage) + "%");

    frameBuffer->setTextColor(0xFFFF);
    frameBuffer->setCursor(SCREEN_WIDTH - 6 * 4 - 1, 5 - 1);
    frameBuffer->println(String(batteryPercentage) + "%");


    drawNotifications(notificationData, 0, 30, 0xFFFF);

    circ5.animateAndDraw(frameBuffer);
    circ4.animateAndDraw(frameBuffer);
    circ3.animateAndDraw(frameBuffer);
    circ2.animateAndDraw(frameBuffer);
    circ1.animateAndDraw(frameBuffer);


    tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  }
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
