#include "Declarations.h"
#include "TimeTracker.h"
#include "LCD.h"
#include <soc/rtc.h>
#include <esp_clk.h>

void interfaceLoop();
void activate();

#define SCREEN_TOUCH_ON_TIME 5000
unsigned long lastTouch = 0;

int count = 0;

void setup()
{
  unsigned long chrono = micros();
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("bootCount = " + String(bootCount));
  Serial.flush();
#endif

  if (bootCount == 0)
  {
//    getInternetTime();
  }

  bootCount++;

  delay(1000);
  initLCD();
  Serial.println("Initialized LCD");

  delay(1000);

  //  gpio_deep_sleep_hold_en();
  //
  //  esp_sleep_enable_timer_wakeup(SleepTime);
  //
  //  printLocalTime(); // This sets the internal clock
  //
  //  esp_deep_sleep_start();
  Serial.println("Entering Display Loop");
}

void loop()
{
  disp();
  Serial.println(count++);
}
