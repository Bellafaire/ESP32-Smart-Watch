#include <WiFi.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <XPT2046_Touchscreen.h>
#include "time.h"
#include "esp_sleep.h"

#define TEXT_COLOR ST77XX_WHITE
#define INTERFACE_COLOR 0xFFFF

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128

#define CHARGING_PIN 4
#define BATTERY_SENSE 32

int BACKGROUND_COLOR =  ST77XX_BLACK;
int ERROR_COLOR =  ST77XX_BLUE;

#define DEBUG

#define LCD_LED_CTRL 25

#define TOUCH_CS  14
#define TOUCH_INT 10

XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_INT);

// TOUCH CALIBRATION
#define XMAX 3711
#define XMIN 259
#define YMAX 3723
#define YMIN 465

struct point {
  int x, y;
};

#define TFT_CS         5
#define TFT_RST        22
#define TFT_DC         21

String ssid       = "Bellafaire Family Wifi";
String password   = "cashewbunny";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -5 * 3600;
const int   daylightOffset_sec = 3600;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//structures 
typedef struct onscreenButton button;
typedef struct iconButton iconButton;

struct onscreenButton {
  int _x, _y, _width, _height, _color, _backgroundColor;
  String _text;
};

struct iconButton {
  int _x, _y, _width, _height, _color, _backgroundColor;
  uint16_t icon[16]; //16x16 icon (single color)
};

//battery.h
float getBatteryVoltage();

//button.h
int TextWidth(button b);
void paintButton(iconButton b);
void paintButton(button b);
void paintButtonFull(button b);
void paintButtonFull(iconButton b);
bool checkButtonPress(iconButton b, int x, int y) ;
bool checkButtonPress(button b, int x, int y);
void pressButton(iconButton b);
void pressButton(button b);

//animations.h
void SweepClear();

//display.h
void initLCD();
struct point getTouchedPosition();
void getTouchedPosition(int *x, int *y);

//notifications.h
void switchToNotifications();
void drawNotifications();
void NotificationsTouchHandler(int x, int y);

//TimeTracker.h
String getInternetTime();
void drawDate(int x, int y, int textSize);
void drawDateCentered(int y, int textSize);
void drawTime(int x, int y, int textSize);

//home.h
void switchToHome();
void drawHome();
void HomeTouchHandler(int x, int y);

//settings.h
void switchToSettings();
void drawSettings();
void SettingsTouchHandler(int x, int y);
void  reAdjustTime();
void changeNetwork();
void about();
void testWifi();
void batterySettings();
