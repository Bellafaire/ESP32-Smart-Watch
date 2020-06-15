
//attach IRQ interrupt
void initTouch()
{
  readTouch(); //dummy read (IRQ may not be enabled on startup)
  attachInterrupt(TOUCH_IRQ, TOUCH_ISR, FALLING);
}

void IRAM_ATTR TOUCH_ISR()
{
#ifdef DEBUG
  Serial.println("Touch detected from interrupt");
#endif
  // readTouch();
  lastTouchTime = millis();
  touchDetected = true;
  // handleTouch();
}

//reads touch from the touch controller and enables the Touch interrupt pin
struct point readTouch()
{
  struct point p;
  if (!digitalRead(TOUCH_IRQ))
  {
    //since we already have a readRegister function for the 16 bit battery monitor we just reuse it here
    //and shift the result to the right. the values read from the touch controller are then
    //mapped according to the values defined in declarations.h
    p.xPos = map(readRegister(TOUCH_ADDR, 0b11010010) >> 8, X_MIN, X_MAX, 0, SCREEN_WIDTH);
    p.yPos = map(readRegister(TOUCH_ADDR, 0b11000010) >> 8, Y_MIN, Y_MAX, SCREEN_HEIGHT, 0);
#ifdef DEBUG
    printPoint(p);
    tft.drawCircle(p.xPos, p.yPos, 2, TEXT_COLOR);
#endif
  }
  else
  {
    p.xPos = -1;
    p.yPos = -1;
  }
  return p;
}

void printPoint(struct point p)
{
  Serial.print("Point (x=");
  Serial.print(p.xPos);
  Serial.print(", y=");
  Serial.print(p.yPos);
  Serial.println(")");
}

//All touch handling for individual pages are switched here
void handleTouch()
{
  if (touchDetected)
  {
    switch (currentPage)
    {
    case HOME:
      HomeTouchHandler(readTouch());
      break;
    default:
      break;
    }
    touchDetected = false;
  }
}