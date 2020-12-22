/********************************************************************
                                LCD
 ********************************************************************/
void initLCD() {
  pinMode(LCD_EN, OUTPUT);
  digitalWrite(LCD_EN, HIGH);
  tft.initR(INITR_GREENTAB);   // initialize a ST7735S chip, green tab
  tft.setRotation(3);
  tft.fillScreen(BACKGROUND_COLOR);
  pinMode(LCD_LED, OUTPUT);
  digitalWrite(LCD_LED, HIGH);
}


/********************************************************************
                               Touch
 ********************************************************************/
int rapidTouchCount = 0;

void IRAM_ATTR TOUCH_ISR()
{
  lastTouchTime = millis();
  if (millis() - lastTouchTime < 200) {
    rapidTouchCount++;
    printDebug("rapidTouchCount: " + String(rapidTouchCount));

    if (rapidTouchCount > 50) {
#ifdef DEBUG
      Serial.println("**** Rapid Touch shutdown registered ****");
      Serial.flush();
#endif
      esp_sleep_enable_timer_wakeup(1);
      esp_deep_sleep_start();

    }
  } else {
    rapidTouchCount = 0;
  }
}

void initTouch()
{

  //dummy read the touch controller, bits 2 and 3 determine the power down state, 00 enables touch IRQ and puts the device into power down
  struct point p;
  p.x = map(readRegister(TOUCH_ADDR, 0b11010010) >> 8, X_MIN, X_MAX, 0, SCREEN_WIDTH);
  p.y = map(readRegister(TOUCH_ADDR, 0b11000010) >> 8, Y_MIN, Y_MAX, SCREEN_HEIGHT, 0);

  pinMode(TOUCH_IRQ, INPUT);

  attachInterrupt(TOUCH_IRQ, TOUCH_ISR, FALLING);
}


struct point readTouch() {
  struct point p;
  if (!digitalRead(TOUCH_IRQ))
  {
    //since we already have a readRegister function for the 16 bit battery monitor we just reuse it here
    //and shift the result to the right. the values read from the touch controller are then
    //mapped according to the values defined in declarations.h
    p.x = map(readRegister(TOUCH_ADDR, 0b11010010) >> 8, X_MIN, X_MAX, 0, SCREEN_WIDTH);
    p.y = map(readRegister(TOUCH_ADDR, 0b11000010) >> 8, Y_MIN, Y_MAX, SCREEN_HEIGHT, 0);
  }
  else
  {
    p.x = -1;
    p.y = -1;
  }
  return p;
}

/********************************************************************
                               I2C
 ********************************************************************/
void WriteAndVerifyRegister(char RegisterAddress, int RegisterValueToWrite)
{
  int Attempt = 0;
  int RegisterValueRead;
  do
  {
    sendWrite(BAT_MONITOR_ADDR, RegisterAddress, RegisterValueToWrite);
    delay(1); //1ms

    RegisterValueRead = readRegister(BAT_MONITOR_ADDR, RegisterAddress);
  } while (RegisterValueToWrite != RegisterValueRead && Attempt++ < 3);
}

void sendWrite(byte deviceAddr, byte location, int d)
{
  Wire.beginTransmission(BAT_MONITOR_ADDR);
  Wire.write(location);
  Wire.write(d & 0x00FF);
  Wire.write(d >> 8);
  Wire.endTransmission();
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
