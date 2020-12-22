#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Wire.h>
#include "BLEDevice.h"
#include <EEPROM.h>
#include <soc/rtc.h>
#include <sys/time.h>

/*******************************************************************
                              DEBUG
 *******************************************************************/
//prints debug information to the serial terminal when declared
#define DEBUG

#ifdef DEBUG
#define printDebug(a) Serial.println(a)
#endif
#ifndef DEBUG
#define printDebug(a)
#endif


/*******************************************************************
                       Hardware Pin Declarations
 *******************************************************************/

//touch screen driver interrupt request
#define TOUCH_IRQ 4

//LCD pins
#define LCD_EN 13
#define LCD_CS 16
#define LCD_LED 14
#define LCD_SCK 18
#define LCD_DC 21 //LCD_A0
#define LCD_RST 22
#define LCD_MOSI 23

//power regulation/monitoring pins
//#define BAT_ALRT 34 //not used in v4.1
#define CHG_STAT 35
#define REG_PG 36
#define ACCEL_EN 34

//I2C Pins and Addresses
#define I2C_SCL 32
#define I2C_SDA 33
#define BAT_MONITOR_ADDR 0x36
#define TOUCH_ADDR 0x48

//Accelerometer connections (optional extra)
#define X_ACCEL 26
#define Y_ACCEL 27
#define Z_ACCEL 25

#define X_ACCEL_ADC_CH 9
#define Y_ACCEL_ADC_CH 7
#define Z_ACCEL_ADC_CH 8


/********************************************************************
                           LCD Objects
 ********************************************************************/
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128
Adafruit_ST7735 tft = Adafruit_ST7735(LCD_CS, LCD_DC, LCD_RST);
GFXcanvas16 *frameBuffer = new GFXcanvas16 (SCREEN_WIDTH, SCREEN_HEIGHT);

#define BACKGROUND_COLOR ST77XX_BLACK
#define TEXT_COLOR ST77XX_WHITE
#define INTERFACE_COLOR ST77XX_WHITE
#define GRAYED 0xBDF7
int ERROR_COLOR = ST77XX_BLUE;


/********************************************************************
                            TOUCH
 ********************************************************************/
struct point
{
  int x = -1;
  int y = -1;
};

unsigned long lastTouchTime = 0;

//Touch Calibration
#define X_MAX 233
#define X_MIN 19
#define Y_MAX 230
#define Y_MIN 14

/********************************************************************
                            timeKeeping
 ********************************************************************/
RTC_DATA_ATTR struct tm* timeinfo;

/********************************************************************
                            DataSeperators
 ********************************************************************/
#define FIELD_SEPARATOR ';'
#define FIELD_SEPARATOR_STRING ";"

/********************************************************************
                        EEPROM Data Fieldfs
 ********************************************************************/
#define EEPROM_SIZE 512

//EEPROM Data locations and information
//eeprom allocations for data placement and their corrosponding string for display.
#define DAYLIGHT_SAVINGS 0
#define DAYLIGHT_SAVINGS_STRING "Daylight Savings"

#define USE_ACCELEROMETER 1
#define USE_ACCELEROMETER_STRING "Enable Acclerometer"
