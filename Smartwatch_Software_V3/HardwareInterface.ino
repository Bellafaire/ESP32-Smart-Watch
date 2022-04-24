/*****************************************************************************
  The MIT License (MIT)
  Copyright (c) 2020 Matthew James Bellafaire
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
******************************************************************************/
/* Hardware interfacing code, this should ideally be the only file that requires modification if
    the hardware should change, everything else is more-or-less talking to a function in this file
    if any hardware external to the ESP32 needs to talk
*/

/********************************************************************
                                LCD
 ********************************************************************/
void initLCD(boolean backlight_on)
{
  pinMode(LCD_EN, OUTPUT);
  digitalWrite(LCD_EN, HIGH);
  tft.initR(INITR_GREENTAB); // initialize a ST7735S chip, green tab
  tft.setRotation(3);
  tft.fillScreen(BACKGROUND_COLOR);
  pinMode(LCD_LED, OUTPUT);
  digitalWrite(LCD_LED, backlight_on);

  // ledcSetup(LCD_BACKLIGHT_PWM_CHAN, 300, 8);
  // ledcAttachPin(LCD_LED, LCD_BACKLIGHT_PWM_CHAN);
}

// this really only exists for readability elsewhere in the code.
void setBacklight(byte brightness)
{
  ledcWrite(LCD_BACKLIGHT_PWM_CHAN, brightness);
}

/********************************************************************
                               Touch
 ********************************************************************/
int rapidTouchCount = 0;

// watchdog tasks, it's possible some of the code can bind up. This at least prevents
// the user from having to manually press the reset button
void watchDog(void *pvParameters)
{
  (void)pvParameters;

  for (;;)
  {
    //    printDebug("***watchdog serviced***");
    if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0)
    {
      if (millis() > lastTouchTime + 20000 && readZAccel() < ACCELEROMETER_STAY_AWAKE_THRESHOLD)
      {
        esp_sleep_enable_timer_wakeup(1);
        esp_deep_sleep_start();
      }
    }
    vTaskDelay(500);
  }
}

/* Touch interrupt
   This interrupt triggers the touch handling task defined in TouchInterface.ino which allows
   for touch areas to be identified. It also contains a rapid-touch shutdown which can be triggerred
   by tapping the screen rapidly.
*/
void IRAM_ATTR TOUCH_ISR()
{
  //  printDebug("TouchInterrupt");
  if ((millis() - lastTouchTime < 200))
  {
    if ((millis() - lastTouchTime > 50))
      rapidTouchCount++;
    // printDebug(String(rapidTouchCount)); //this line causes crashes occasionally leave it out unless specifically working on this function (and of course expect some crashes)
    if (rapidTouchCount > 50)
    {
#ifdef DEBUG
      Serial.println("**** Rapid Touch shutdown registered ****");
      Serial.flush();
#endif

      // clear calibration data after rapid touch
      CLEAR_TOUCH_CALIBRATION = true;
      rapidTouchCount = 0;
      // rapid touch is meant to restart, using deep sleep we can essentially go back to
      // the same state we would get from a cold restart
      esp_sleep_enable_timer_wakeup(1);
      esp_deep_sleep_start();
    }
  }
  else
  {
    rapidTouchCount = 0;
  }
#ifdef USE_TOUCH_HANDLING_TASK
  xTaskCreatePinnedToCore(TouchTask, "TOUCH_TASK", 4 * 1024, (void *)1, 2, NULL, 1);
#endif
  lastTouchTime = millis();
}

// init touch IC
void initTouch()
{

  // dummy read the touch controller, bits 2 and 3 determine the power down state, 00 enables touch IRQ and puts the device into power down
  struct point p;
  p.x = readRegister(TOUCH_ADDR, 0b11010010);
  p.y = readRegister(TOUCH_ADDR, 0b11000010);

  pinMode(TOUCH_IRQ, INPUT);

  rapidTouchCount = 0;
  attachInterrupt(TOUCH_IRQ, TOUCH_ISR, FALLING);
}

// activates touch interrupt
void activateTouch()
{
  attachInterrupt(TOUCH_IRQ, TOUCH_ISR, FALLING);
}

// deactivates touch interrupt
void deactivateTouch()
{
  detachInterrupt(TOUCH_IRQ);
  rapidTouchCount = 0;
}

// read touch
struct point readTouch()
{
  struct point p;
  if (!digitalRead(TOUCH_IRQ))
  {
    // since we already have a readRegister function for the 16 bit battery monitor we just reuse it here
    // and shift the result to the right. the values read from the touch controller are then
    // mapped according to the values defined in declarations.h

    int xval = readRegister(TOUCH_ADDR, 0b11010010) >> 8;
    int yval = readRegister(TOUCH_ADDR, 0b11000010) >> 8;

    // map touch screen readings to loaded EEPROM calibration data
    // p.x = map(xval, SETTING_X_MIN, SETTING_X_MAX, 0, SCREEN_WIDTH);
    // p.y = map(yval, SETTING_Y_MIN, SETTING_Y_MAX, 0, SCREEN_HEIGHT);

    // determine quadrant boundries
    int xtot = 0;
    int ytot = 0;
    for (int a = 0; a < TOTAL_CALIBRATION_POINTS; a++)
    {
      xtot += calibrationX[a];
      ytot += calibrationY[a];
    }

    // these are the markers for the quadrant.
    int xQuad = xtot / TOTAL_CALIBRATION_POINTS;
    int yQuad = ytot / TOTAL_CALIBRATION_POINTS;

    p.x = (xval < yQuad) ? map(xval, calibrationX[0], calibrationX[1], CALIBRATION_POINTS_X[0], CALIBRATION_POINTS_X[1]) : map(xval, calibrationX[2], calibrationX[3], CALIBRATION_POINTS_X[2], CALIBRATION_POINTS_X[3]);
    p.y = (yval < xQuad) ? map(yval, calibrationY[0], calibrationY[2], CALIBRATION_POINTS_Y[0], CALIBRATION_POINTS_Y[2]) : map(yval, calibrationY[1], calibrationY[3], CALIBRATION_POINTS_Y[1], CALIBRATION_POINTS_Y[3]);

    // for calibrating touch screen
    printDebug("Raw - x:" + String(xval) + " y:" + String(yval) + " Mapped - x:" + String(p.x) + " y:" + String(p.y));
  }
  else
  {
    p.x = -1;
    p.y = -1;
  }
  return p;
}

// read touch returns raw values
struct point readTouchRaw()
{
  struct point p;
  // since we already have a readRegister function for the 16 bit battery monitor we just reuse it here
  // and shift the result to the right. the values read from the touch controller are then
  // mapped according to the values defined in declarations.h

  int xval = readRegister(TOUCH_ADDR, 0b11010010) >> 8;
  int yval = readRegister(TOUCH_ADDR, 0b11000010) >> 8;

  // for calibrating touch screen
  // printDebug("Raw Touch Screen Readings - x:" + String(xval) + " y:" + String(yval));

  p.x = xval;
  p.y = yval;

  return p;
}

/********************************************************************
                               I2C
    I2C interfacing, abstracts away I2C slightly to make surrounding
    Code easier to work with.
 ********************************************************************/

// since apparently the I2C peripheral on the ESP32 is very-not-thread-safe this
// boolean is toggled during every operation, this prevents a different thread from
// attempting to use the I2C peripheral when an operation is incomplete. It's mildly inconvinent
// but better than the device randomly crashing.
static boolean I2C_OPERATION_IN_PROGRESS = false;

void WriteAndVerifyRegister(char RegisterAddress, int RegisterValueToWrite)
{
  int Attempt = 0;
  int RegisterValueRead;
  do
  {
    sendWrite(BAT_MONITOR_ADDR, RegisterAddress, RegisterValueToWrite);
    delay(1); // 1ms

    RegisterValueRead = readRegister(BAT_MONITOR_ADDR, RegisterAddress);
  } while (RegisterValueToWrite != RegisterValueRead && Attempt++ < 3);
}

void sendWrite(byte deviceAddr, byte location, int d)
{
  if (!I2C_OPERATION_IN_PROGRESS)
  {
    I2C_OPERATION_IN_PROGRESS = true;
    Wire.beginTransmission(deviceAddr);
    Wire.write(location);
    Wire.write(d & 0x00FF);
    Wire.write(d >> 8);
    Wire.endTransmission();
    I2C_OPERATION_IN_PROGRESS = false;
  }
  else
  {
    printDebug("I2C Process FAILED, Peripheral in use. Could not write data");
  }
}

int readRegister(byte deviceAddr, byte location)
{
  if (!I2C_OPERATION_IN_PROGRESS)
  {
    I2C_OPERATION_IN_PROGRESS = true;
    Wire.beginTransmission(deviceAddr); // select device with "beginTransmission()"
    Wire.write(location);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)deviceAddr, (uint8_t)2, (uint8_t)1);

    int a = Wire.read();
    a |= Wire.read() << 8;

    //  int a = Wire.read() << 8 | Wire.read();

    I2C_OPERATION_IN_PROGRESS = false;
    return a;
  }
  else
  {
    printDebug("I2C Process FAILED, Peripheral in use. Could not read data");
    return -1;
  }
}

/********************************************************************
                          Battery Monitor
 ********************************************************************/
// starts battery monitor and configures with parameters stated in Declarations.h
// most of this code came directly from t manufactuer's example
void initBatMonitor()
{
  int StatusPOR = readRegister(BAT_MONITOR_ADDR, 0x00) & 0x0002;
  if (StatusPOR == 0)
  {
  }
  else
  {

    while (readRegister(BAT_MONITOR_ADDR, 0x3D) & 1)
    {
      delay(1);
    }
    int HibCFG = readRegister(BAT_MONITOR_ADDR, 0xBA); // Store original HibCFG value
    sendWrite(BAT_MONITOR_ADDR, 0x60, 0x90);           // Exit Hibernate Mode step 1
    sendWrite(BAT_MONITOR_ADDR, 0xBA, 0x0);            // Exit Hibernate Mode step 2
    sendWrite(BAT_MONITOR_ADDR, 0x60, 0x0);            // Exit Hibernate Mode step 3

    sendWrite(BAT_MONITOR_ADDR, 0x18, designcap); // Write DesignCap
    sendWrite(BAT_MONITOR_ADDR, 0x1E, ichgterm);  // Write IchgTerm
    sendWrite(BAT_MONITOR_ADDR, 0x3A, vempty);    // Write VEmpty

    sendWrite(BAT_MONITOR_ADDR, 0xDB, 0x8000); // Write ModelCFG

    sendWrite(BAT_MONITOR_ADDR, 0x1D, 0x2210); // Write CONFIG1
    sendWrite(BAT_MONITOR_ADDR, 0xBB, 0x365A); // Write CONFIG2

    // Poll ModelCFG.Refresh(highest bit),
    // proceed to Step 3 when ModelCFG.Refresh=0.
    while (readRegister(BAT_MONITOR_ADDR, 0xDB) & 0x8000)
      delay(1);                                // do not continue until ModelCFG.Refresh==0
    sendWrite(BAT_MONITOR_ADDR, 0xBA, HibCFG); // Restore Original HibCFG value
  }

  int Status = readRegister(BAT_MONITOR_ADDR, 0x00); // Read Status
  WriteAndVerifyRegister(0x00, Status & 0xFFFD);     // Write and Verify

  StatusPOR = readRegister(BAT_MONITOR_ADDR, 0x00) & 0x0002; // Read POR bit in Status Register

#ifdef DEBUG
  int RepCap = readRegister(BAT_MONITOR_ADDR, 0x05); // Read RepCap
  Serial.print("RepCap 0x");
  Serial.println(RepCap, HEX);

  int RepSOC = readRegister(BAT_MONITOR_ADDR, 0x06); // Read RepSOC
  Serial.print("RepSOC 0x");
  Serial.println(RepSOC, HEX);

  int TTE = readRegister(BAT_MONITOR_ADDR, 0x11); // Read TTE
  Serial.print("TTE 0x");
  Serial.println(TTE, HEX);

  int VCELL = readRegister(BAT_MONITOR_ADDR, 0x09); // Read VCELL
  Serial.print("VCELL 0x");
  Serial.println(VCELL, HEX);
#endif
}

float getBatteryCurrent()
{
  return ((short)readRegister(BAT_MONITOR_ADDR, 0x0A)) * 0.00015625; // for 0.010 ohm shunt
}

float getTimeUntilEmpty()
{
  int a = readRegister(BAT_MONITOR_ADDR, 0x11);
  float t = (float)a * 5.625; // gives time til empty in seconds
  t = (t) / 3600;             // convert to hours
  return t;
}

float getBatteryVoltage()
{
  int a = readRegister(BAT_MONITOR_ADDR, 0x19);
  float voltage = (float)a * 0.000078125;
  return voltage;
  // return 3.19; //debug
}

// returns in Ah
float getTotalCapacity()
{
  int a = readRegister(BAT_MONITOR_ADDR, 0x10);
  return a * 0.0005;
}

// returns in Ah
float getRemainingCapacity()
{
  int a = readRegister(BAT_MONITOR_ADDR, 0x05);
  return a * 0.0005;
}

int getBatteryPercentage()
{
  int a = readRegister(BAT_MONITOR_ADDR, 0x06);
  a = a >> 8;
  return a;
}

/********************************************************************
                          Accelerometer
 ********************************************************************/

int readXAccel()
{
  int read_raw;
  adc2_config_channel_atten((adc2_channel_t)X_ACCEL_ADC_CH, ADC_ATTEN_11db);
  adc2_get_raw((adc2_channel_t)X_ACCEL_ADC_CH, ADC_WIDTH_BIT_12, &read_raw);
  return read_raw;
}

int readYAccel()
{
  int read_raw;
  adc2_config_channel_atten((adc2_channel_t)Y_ACCEL_ADC_CH, ADC_ATTEN_11db);
  adc2_get_raw((adc2_channel_t)Y_ACCEL_ADC_CH, ADC_WIDTH_BIT_12, &read_raw);
  return read_raw;
}
int readZAccel()
{
  return analogRead(Z_ACCEL);
}

/********************************************************************
                          Battery Charger
 ********************************************************************/

boolean isCharging()
{
  return !digitalRead(CHG_STAT);
}
