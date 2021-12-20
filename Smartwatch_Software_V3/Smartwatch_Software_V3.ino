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
#include "drawable.h"

int batteryPercentage = 100;
esp_sleep_wakeup_cause_t wakeup_reason;
boolean timeUpdated = false;
boolean notificationsUpdated = false;

void onConnectEvent()
{
  updateTime();
}

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("ESP32 Smartwatch");
#endif
  EEPROM.begin(EEPROM_SIZE);
  Wire.begin(I2C_SDA, I2C_SCL, 400000);
  pinMode(CHG_STAT, INPUT);
  initLCD();
  initBatMonitor();
  initTouch();

  initBLE();

  printDebug("Starting Advertisement");
  startBLEAdvertising();

  loadEEPROMSettings();

  // create "watchdog task" to put the device in deepsleep if something goes wrong
  xTaskCreatePinnedToCore(watchDog, "watchdog", 1024, NULL, 3, NULL, 0);
  setHomePage();
  // updateTime();
}

void getNotifications()
{
  boolean success = sendBLE("/notifications", &notificationData, false);
  if (success)
  {
    notificationsUpdated = true;
    printDebug("notifications updating");
  }
}

boolean wakeupCheck()
{
  return ((millis() - lastTouchTime) < 3000) || (readZAccel() > ACCELEROMETER_STAY_AWAKE_THRESHOLD);
}

void deviceSleep()
{
  batteryPercentage = getBatteryPercentage();
  digitalWrite(LCD_LED, LOW);
  // re-enable touch wakeup

  printDebug("Going to sleep");

  Serial.flush();

  // put display to sleep
  tft.enableSleep(true);

  deactivateTouch();

  connected = false;
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); // 1 = High, 0 = Low
  esp_light_sleep_start();
}

void onWakeup()
{
  getRTCTime();
  setHomePage();

  // wake up display
  tft.enableSleep(false);
  digitalWrite(LCD_LED, HIGH);

  // new wakeup so we'll want to update notifications next chance we get
  notificationsUpdated = false;
  timeUpdated = false;

  activateTouch();

  printDebug("Starting Advertisement");
  startBLEAdvertising();
}

void loop()
{
  if (wakeupCheck())
  {
    drawFrame();

    if (connected)
    {
      if (!notificationsUpdated)
        getNotifications();
    }
  }
  else
  {
    deviceSleep();

    // the program halts until this point so we know that there was a touch
    // if this line is being executed since the ESP32 has been woken up.
    lastTouchTime = millis();
    onWakeup();
  }
}
