#include "Declarations.h"

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 600         /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

void setup()
{
  unsigned long chrono = micros();
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("bootCount = " + String(bootCount));
  Serial.flush();
#endif

  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  pinMode(TOUCH_IRQ, INPUT);

  //init I2C communication
  Wire.begin(I2C_SDA, I2C_SCL, 100000);
  //the battery monitor only needs to be configured once when powered on.
  if (bootCount == 0)
  {
    getInternetTime();
    initBatMonitor();
#ifdef DEBUG
    Serial.println("Battery Monitor initialized");
#endif
  }

  //Check if this is the first reboot and get ready to setup another sleep
  ++bootCount;

  //wakeup every 10 minutes, we'll use this for getting notification updates and things like that
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  //wakeup when someone touches the screen
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); //1 = High, 0 = Low

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    //if woken up by user touching screen
    initLCD();
    initTouch();
    MainLoop();
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
//if woken up by 10 minute timer
#ifdef DEBUG
    Serial.println("Woken up by timer");
#endif
    break;
  default:
#ifdef DEBUG
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
#endif
    break;
  }

#ifdef DEBUG
  Serial.println("Going to sleep now");
  Serial.flush();
#endif;
  esp_deep_sleep_start();
}

void loop()
{
}
