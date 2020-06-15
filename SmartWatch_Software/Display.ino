void initLCD() {
  pinMode(LCD_EN, OUTPUT); 
  digitalWrite(LCD_EN, HIGH); 
  tft.initR(INITR_GREENTAB);   // initialize a ST7735S chip, green tab
  tft.setRotation(3);
  tft.fillScreen(BACKGROUND_COLOR);
  pinMode(LCD_LED, OUTPUT); 
  digitalWrite(LCD_LED, HIGH); 
}
