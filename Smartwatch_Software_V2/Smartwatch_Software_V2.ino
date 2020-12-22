#include "Declarations.h"

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL, 100000);
  initLCD();
  initTouch();
}

void loop() {
  frameBuffer->fillScreen(0x0000);
  frameBuffer->setCursor(0, 0);
  if (!digitalRead(TOUCH_IRQ)) {
    struct point p = readTouch();
    frameBuffer->fillCircle(p.x, p.y, 4, 0xFFFF);
  }
  frameBuffer->print("Smartwatch " + String(millis()));
  tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
}
