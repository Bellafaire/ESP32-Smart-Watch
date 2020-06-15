#include <Wire.h>

//I2C Pins and Addresses
#define I2C_SCL 32
#define I2C_SDA 33
#define BAT_MONITOR_ADDR 0x36
#define TOUCH_ADDR 0x48

#define X_MAX 233
#define X_MIN 19
#define Y_MAX 230
#define Y_MIN 14

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128

struct point {
  int xPos;
  int yPos;
};

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL, 100000);
  Serial.begin(115200);
  Serial.println("Touch IC testing");
}

int readRegister(byte deviceAddr, byte location)
{
  Wire.beginTransmission(deviceAddr); // select device with "beginTransmission()"
  Wire.write(location);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddr, 2, true); //request battery percentage

  int  a = Wire.read();
  a |= Wire.read() << 8;

  //  int a = Wire.read() << 8 | Wire.read();
  return a;
}

void printPoint(struct point p) {
  Serial.print("Point (x=");
  Serial.print(p.xPos);
  Serial.print(", y=");
  Serial.print(p.yPos);
  Serial.println(")");
}

struct point readTouch() {
  struct point p;
  p.xPos =  map(readRegister(TOUCH_ADDR, 0b11010010) >> 8, X_MIN, X_MAX,0, SCREEN_WIDTH);
  p.yPos = map(readRegister(TOUCH_ADDR, 0b11000010) >> 8, Y_MIN, Y_MAX, SCREEN_HEIGHT,0);
  printPoint(p); 
  return p; 
}


void loop() {
  readTouch();
  delay(100);
}
