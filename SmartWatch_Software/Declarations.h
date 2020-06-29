#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include "BluetoothSerial.h"

//just to avoid putting my wifi credentials on the public repo
//Later the wifi credentials should be stored in eeprom or on the android device
#include "J:\Dropbox\Dropbox\Lab Projects\Smart Watch\WifiCredentials.h" //desktop computer location
// #include "C:\Users\James\Dropbox\Lab Projects\Smart Watch\WifiCredentials.h" //laptop computer location

#include "Pages.h"
#include "Icons.h"
//IconButtons.h is included below since it relies on some declarations

//color picker here http://www.barth-dev.de/online/rgb565-color-picker/
//look and feel and colors
#define BACKGROUND_COLOR ST77XX_BLACK
#define TEXT_COLOR ST77XX_WHITE
#define INTERFACE_COLOR ST77XX_WHITE
#define GRAYED 0xBDF7
int ERROR_COLOR = ST77XX_BLUE;

#define screenOnTime 10000 //time before watch screen times out without user input

//variables used globally
boolean touchDetected = false;
int currentPage = HOME;
unsigned long lastTouchTime = 0;

//prints debug information to the serial terminal when declared
// #define DEBUG
#define SHOW_LAST_NOTIFICATION_TIME //when declared shows the time of the last recieved notification update at the bottom of the screen

//touch screen driver interrupt request
#define TOUCH_IRQ 4

//LCD pins
#define LCD_EN 13
#define LCD_CS 14
#define LCD_LED 16
#define LCD_SCK 18
#define LCD_DC 21 //LCD_A0
#define LCD_RST 22
#define LCD_MOSI 23
Adafruit_ST7735 tft = Adafruit_ST7735(LCD_CS, LCD_DC, LCD_RST);

//power regulation/monitoring pins
//#define BAT_ALRT 34 //not used in v4.1
#define CHG_STAT 35
#define REG_PG 36
#define LCD_PG 39

//I2C Pins and Addresses
#define I2C_SCL 32
#define I2C_SDA 33
#define BAT_MONITOR_ADDR 0x36
#define TOUCH_ADDR 0x48

//Battery Monitor Configuration Values
#define designcap 1200 //600mAh (0.5mAh resolution / 600mAh) (for 10m sense resistor)
#define ichgterm 20
#define vempty 0x9650
#define modelcfg 0x8400

//Touch Calibration
#define X_MAX 233
#define X_MIN 19
#define Y_MAX 230
#define Y_MIN 14

//screen resolution (don't need any magic numbers)
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128

//Accelerometer connections (optional extra)
#define Z_ACCEL 25
#define X_ACCEL 26
#define Y_ACCEL 27

//Time tracker variables (Stored in RTC)
RTC_DATA_ATTR char notificationData[2048];
RTC_DATA_ATTR time_t now;
RTC_DATA_ATTR uint64_t Mics = 0;
RTC_DATA_ATTR struct tm *timeinfo;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -5 * 3600;
const int daylightOffset_sec = 0;

//bluetooth serial object used in BluetoothRecieve
BluetoothSerial SerialBT;

//structures
typedef struct onscreenButton button;
typedef struct iconButton iconButton;

struct onscreenButton
{
  int _x, _y, _width, _height, _color, _backgroundColor;
  String _text;
};

struct iconButton
{
  int _x, _y, _width, _height, _color, _backgroundColor;
  uint16_t icon[16]; //16x16 icon (single color)
};

struct point
{
  int xPos;
  int yPos;
};

#include "IconButtons.h"

//window object
class Window
{
  iconButton WindowUpArrowButton = {128, 0, 32, 48, INTERFACE_COLOR, BACKGROUND_COLOR, {(0b00000001 << 8) | 0b10000000, (0b00000011 << 8) | 0b11000000, (0b00000111 << 8) | 0b11100000, (0b00001111 << 8) | 0b11110000, (0b00011111 << 8) | 0b11111000, (0b00111111 << 8) | 0b11111100, (0b01111111 << 8) | 0b11111110, (0b11111111 << 8) | 0b11111111, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000}};
  iconButton WindowDownArrowButton = {128, 48, 32, 48, INTERFACE_COLOR, BACKGROUND_COLOR, {(0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b11111111 << 8) | 0b11111111, (0b01111111 << 8) | 0b11111110, (0b00111111 << 8) | 0b11111100, (0b00011111 << 8) | 0b11111000, (0b00001111 << 8) | 0b11110000, (0b00000111 << 8) | 0b11100000, (0b00000011 << 8) | 0b11000000, (0b00000001 << 8) | 0b10000000}};
  boolean _scroll;
  String textBuffer = "";
  boolean focused = true;
  int scrollPosition = 0;
  int _x, _y, _width, _height, x_cursor, y_cursor;

public:
  Window(int x, int y, int width, int height, boolean scroll);
  void print(String Text);
  void println(String Text);
  void touch();
  void focus();
  boolean isFocused();

private:
  String getValue(String data, char separator, int index);
  void drawTextToWindow(boolean clr);
};

/* a simple menu to allow a user to select from a list of choices, returns an integer for the selected value */
class SelectionWindow {
    int selection;
    int sectionWidth;
    boolean focused = false;
    int _x, _y, _width, _height;
    int totalOptions = 1;
    String options = "Cancel";
    iconButton RightArrowButton =   { 128, 0, 32, 48, INTERFACE_COLOR, BACKGROUND_COLOR,   {(0b00000000 << 8) | 0b10000000,  (0b00000000 << 8) | 0b11000000,  (0b00000000 << 8) | 0b11100000,  (0b00000000 << 8) | 0b11110000,  (0b11111111 << 8) | 0b11111000,  (0b11111111 << 8) | 0b11111100,  (0b11111111 << 8) | 0b11111110,  (0b11111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111110,  (0b11111111 << 8) | 0b11111100,  (0b11111111 << 8) | 0b11111000,  (0b00000000 << 8) | 0b11110000,  (0b00000000 << 8) | 0b11100000,  (0b00000000 << 8) | 0b11000000,  (0b00000000 << 8) | 0b10000000}};
    iconButton LeftArrowButton =    {    128, 0, 32, 48, INTERFACE_COLOR, BACKGROUND_COLOR,  {      (0b00000001 << 8) | 0b00000000,      (0b00000011 << 8) | 0b00000000,  (0b00000111 << 8) | 0b00000000,  (0b00001111 << 8) | 0b00000000,  (0b00011111 << 8) | 0b11111111,  (0b00111111 << 8) | 0b11111111,  (0b01111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111111,  (0b01111111 << 8) | 0b11111111,  (0b00111111 << 8) | 0b11111111,  (0b00011111 << 8) | 0b11111111,  (0b00001111 << 8) | 0b00000000,  (0b00000111 << 8) | 0b00000000,  (0b00000011 << 8) | 0b00000000,  (0b00000001 << 8) | 0b00000000    }  };
    button okButton =  {128, 0, 32, 48, INTERFACE_COLOR, BACKGROUND_COLOR, "Ok"};

  public:
    SelectionWindow(int x, int y, int width, int height);  //constructor determines form/size
    int addOption(String s); //adds option and returns option number
    int focus(); //focuses the window, will return integer representitive of the selected option
  private:
    void drawCurrentOption(); //draws the option's string to the screen
    String getValue(String data, char separator, int index); //splits the options string
    void touch(); //touch handler
};


/***********************************************
 *                                             *
 *            Function Signitures              *
 *                                             *
 ***********************************************/
//Battery_Monitor.ino
void initBatMonitor();
float getBatteryCurrent();
float getBatteryVoltage();
float getTotalCapacity();
float getRemainingCapacity();
int getBatteryPercentage();
void WriteAndVerifyRegister(char RegisterAddress, int RegisterValueToWrite);
int readRegister(byte deviceAddr, byte location);
void sendWrite(byte deviceAddr, byte location, int d);
float getTimeUntilEmpty();

//Display.ino
void initLCD();

//Touch.h
struct point readTouch();
void printPoint(struct point p);
void IRAM_ATTR TOUCH_ISR();
void initTouch();
void handleTouch();

//MainLoop.ino
void testScreen();
void MainLoop();

//TimeTracker.ino
void printLocalTime();
void updateTime(uint64_t elapsedTime);
void beginTimedSleep(unsigned long tm0);

//Home.ino
void switchToHome();
void drawHome();
void HomeTouchHandler(int x, int y);
void homeLoop();
void writeNotifications();
String parseFromNotifications(int line, int field);
int getNotificationLines();
String getValue(String data, char separator, int index);

//animations.ino
void SweepClear();

//Button.ino
int TextWidth(button b);
void paintButton(button b);
void paintButton(iconButton b);
void paintButtonFull(button b);
void paintButtonFull(iconButton b);
void pressButton(iconButton b);
void pressButton(button b);
bool checkButtonPress(iconButton b, int x, int y);
bool checkButtonPress(button b, int x, int y);

//Settings.ino
void about();
void testWifi();
void changeNetwork();
void batterySettings();
void reAdjustTime();
void accelTest();
void SettingsTouchHandler(struct point p);
void drawSettings();
void switchToSettings();
void settingsLoop();

//BluetoothRecieve.ino
void getPhoneNotifications(int timeout);
