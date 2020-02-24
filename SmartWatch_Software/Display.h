void initLCD() {
  tft.initR(INITR_GREENTAB);   // initialize a ST7735S chip, green tab
  tft.setRotation(1);
  tft.fillScreen(BACKGROUND_COLOR);

  //init touch functions
  ts.begin();
  ts.setRotation(0);
}

//fairly crude mapping function that translates touch to pixel values on the screen
struct point getTouchedPosition() {
  struct point p;
  if (ts.touched()) {
    TS_Point pread = ts.getPoint();
    p.y = constrain(map( pread.x, XMAX, XMIN, 128, 0), 0, 128);
    p.x =  constrain(map( pread.y, YMAX, YMIN, 160, 0), 0, 160);

  } else {
    p.x = -1;
    p.y = -1;
  }
  return p;
}

void getTouchedPosition(int *x, int *y) {
  struct point p;
  if (ts.touched()) {
    TS_Point pread = ts.getPoint();
    p.y = constrain(map( pread.x, XMAX, XMIN, 128, 0), 0, 128);
    p.x =  constrain(map( pread.y, YMAX, YMIN, 160, 0), 0, 160);

  } else {
    p.x = -1;
    p.y = -1;
  }
  *x = p.x;
  *y = p.y;
}
