#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

//look and feel and colors
#define BACKGROUND_COLOR ST77XX_BLACK
#define TEXT_COLOR ST77XX_WHITE


//prints debug information to the serial terminal when declared
#define DEBUG 

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
#define designcap 1200   //600mAh (0.5mAh resolution / 600mAh) (for 10m sense resistor)
#define ichgterm 0x0640
#define vempty 0x9650
#define modelcfg 0x8400

//Touch Calibration
#define X_MAX 233
#define X_MIN 19
#define Y_MAX 230
#define Y_MIN 14

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128


/***********************************************
 *                                             *
 *        Files and Function Signitures        *
 *    (functions then their associated file)   *
 *                                             *
 ***********************************************/
//Battery_Monitor.h
void initBatMonitor();
float getBatteryCurrent();
float getBatteryVoltage();
float getTotalCapacity();
float getRemainingCapacity();
int getBatteryPercentage();
void WriteAndVerifyRegister(char RegisterAddress, int RegisterValueToWrite);
int readRegister(byte deviceAddr, byte location);
void sendWrite(byte deviceAddr, byte location, int d);

//Display.h
void initLCD();

//Touch.h
struct point readTouch();
void printPoint(struct point p);

//MainLoop.h
void testScreen();
void MainLoop();

#include "Battery_Monitor.h"
#include "Display.h"
#include "Touch.h"
#include "MainLoop.h"