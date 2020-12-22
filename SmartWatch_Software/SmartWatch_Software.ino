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

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 60         /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR boolean YWakeupCondition = false;
RTC_DATA_ATTR int YWakeupCount = 0;

static void RTC_IRAM_ATTR wake_stub();

static void RTC_IRAM_ATTR wake_stub() {

}

//puts device to sleep and configures wakeup sources
void deviceSleep() {
  //re-enable touch wakeup
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); //1 = High, 0 = Low
  esp_set_deep_sleep_wake_stub(&wake_stub);
  esp_deep_sleep_start();
}

void setup()
{
  unsigned long wakeupTime = micros();
  unsigned long chrono = micros();
#ifdef DEBUG
  Serial.begin(115200);
#endif
  printDebug("bootCount = " + String(bootCount));

  //init I2C communication
  Wire.begin(I2C_SDA, I2C_SCL, 100000);

  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  pinMode(CHG_STAT, INPUT);
  pinMode(TOUCH_IRQ, INPUT);
  initTouch();

  EEPROM.begin(EEPROM_SIZE);

  if (getBatteryVoltage() < 3.2) {

    printDebug("Battery Voltage is too low to start device: " + String(getBatteryVoltage(), 3));
    pinMode(LCD_LED, OUTPUT);
    pinMode(LCD_EN, OUTPUT);
    digitalWrite(LCD_EN, HIGH);

    for (int a = 0; a < 3; a++) {
      digitalWrite(LCD_LED, HIGH);
      delay(100);
      digitalWrite(LCD_LED, LOW);
      delay(100);
    }



  } else {

    //the battery monitor only needs to be configured once when powered on.
    if (bootCount == 0)
    {
      notificationData[0] = 0;
      initTouch();
      initLCD();
      initBatMonitor();
      testScreen();
      tft.println("phone notifications obtained");
      //      getInternetTime();
      updateTimeFromNotificationData();
      tft.println("obtained time");

      printDebug("Battery Monitor initialized");

    }

    //Check if this is the first reboot and get ready to setup another sleep
    ++bootCount;

    switch (wakeup_reason)
    {
      case ESP_SLEEP_WAKEUP_EXT0:
        deviceActive = true;
        //if woken up by user touching screen
        initTouch();
        initLCD();
        MainLoop();
        break;
      case ESP_SLEEP_WAKEUP_EXT1:
        break;
      case ESP_SLEEP_WAKEUP_TIMER:
        break;
      default:
        printDebug("Wakeup was not caused by deep sleep: " + wakeup_reason);
        break;
    }
  }

  printDebug("Going to sleep now");

#ifdef DEBUG
  printDebug("Awake for " + String(micros() - wakeupTime) + "uS");
  Serial.flush();
#endif

  deviceSleep();
}

void loop()
{
}
