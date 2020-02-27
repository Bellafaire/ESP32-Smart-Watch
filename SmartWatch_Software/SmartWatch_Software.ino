#include "Declarations.h"
#include "TimeTracker.h"
#include "Animations.h"
#include "Window.h"
#include "SelectionWindow.h"
#include "Display.h"
#include "Button.h"
#include "Pages.h"
#include "home.h"
#include "settings.h"
#include "apps.h"
#include "Notifications.h"
#include "battery.h"

#define SCREEN_TOUCH_ON_TIME 5000
unsigned long lastTouch = 0;

const int SleepTime = 10000; //we wake the micro-controller up after this many microseconds
const int lowPrioritySleepTime = 1000000; //we execute low priority functions this often
int sleepTracker = 0;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);

#endif
  pinMode(LCD_LED_CTRL, OUTPUT);
  pinMode(CHARGING_PIN, OUTPUT);
  pinMode(BATTERY_SENSE, OUTPUT);

  digitalWrite(CHARGING_PIN, LOW);
  //  digitalWrite(LCD_LED_CTRL, HIGH);

  pinMode(LCD_LED_CTRL, OUTPUT);
  digitalWrite(LCD_LED_CTRL, HIGH);

  initLCD();

  tft.setTextColor(INTERFACE_COLOR);
  tft.setCursor(0, 50);
  tft.println("Getting Internet Time");;

  getInternetTime();
  esp_sleep_enable_timer_wakeup(SleepTime);

  SweepClear();
}

void loop() {
  highPriority();
  delay(10);
}

/* things like touch interfacing and other user input go here */
void highPriority() {

  boolean active = lastTouch + SCREEN_TOUCH_ON_TIME > millis(); //if the screen is off there's no sense in changing anything


  if (active) {
    digitalWrite(LCD_LED_CTRL, HIGH);
  } else {
    digitalWrite(LCD_LED_CTRL, LOW);
  }

  if (ts.touched()) {

    lastTouch = millis();

    struct point p = getTouchedPosition();
    tft.drawCircle(p.x, p.y, 2, 0x1D07);


    switch (page) {
      case HOME: HomeTouchHandler(p.x, p.y); break;
      case NOTIFICATIONS: NotificationsTouchHandler(p.x, p.y); break;
      case SETTINGS: SettingsTouchHandler(p.x, p.y); break;
    }
  }

  if (sleepTracker > lowPrioritySleepTime / SleepTime && active) {
    lowPriority();
    sleepTracker = 0;
  }
  sleepTracker++;
}

/* things like drawing the time don't need to happen often at all, so we tuck them in here
*/
void lowPriority() {
  if (page == HOME) {
    drawHome();
  }
}
