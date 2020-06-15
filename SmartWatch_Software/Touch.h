struct point
{
  int xPos;
  int yPos;
};

struct point readTouch()
{
  struct point p;
  if (!digitalRead(TOUCH_IRQ))
  {
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