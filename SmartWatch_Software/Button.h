//determines width of text displayed on LCD screen, default text has width of 5 pixels + 1 space pixel
int TextWidth(button b) {
  return b._text.length() * 6;
}

void paintButton(button b) {
  tft.drawRect(b._x, b._y, b._width, b._height, b._color);
  tft.setTextColor(b._color);
  tft.setCursor(b._x + (int)(b._width - TextWidth(b)) / 2, b._y + (int)b._height / 2 - 5);
  tft.println(b._text);
}

void paintButton(iconButton b) {
  tft.drawRect(b._x, b._y, b._width, b._height, b._color);
  for (int row = 0; row < 16; row++) {
    for (int column = 0; column < 16; column++) {
      tft.drawPixel(
        b._x + b._width / 2 - 8 + column,
        b._y + b._height / 2 - 8 + row,
        (b.icon[row] & (1 << (15 - column))) ? b._color : b._backgroundColor
      );
    }
  }
}


void paintButtonFull(button b) {
  tft.fillRect(b._x, b._y, b._width, b._height, b._backgroundColor);
  tft.drawRect(b._x, b._y, b._width, b._height, b._color);
  tft.setTextColor(b._color);
  tft.setCursor(b._x + (int)(b._width - TextWidth(b)) / 2, b._y + (int)b._height / 2 - 5);
  tft.println(b._text);
}


void paintButtonFull(iconButton b) {
  tft.fillRect(b._x, b._y, b._width, b._height, b._backgroundColor);
  tft.drawRect(b._x, b._y, b._width, b._height, b._color);
  for (int row = 0; row < 16; row++) {
    for (int column = 0; column < 16; column++) {
      tft.drawPixel(
        b._x + b._width / 2 - 8 + column,
        b._y + b._height / 2 - 8 + row,
        (b.icon[row] & (1 << (15 - column))) ? b._color : b._backgroundColor
      );
    }
  }
}

void pressButton(iconButton b){
  tft.fillRect(b._x, b._y, b._width, b._height, b._color);
}

void pressButton(button b){
  tft.fillRect(b._x, b._y, b._width, b._height, b._color);
}

bool checkButtonPress(iconButton b, int x, int y) {
  if (x > b._x && x < b._x + b._width && y > b._y && y < b._y + b._height) {
    return true;
  } else {
    return false;
  }
}

bool checkButtonPress(button b, int x, int y) {
  if (x > b._x && x < b._x + b._width && y > b._y && y < b._y + b._height) {
    return true;
  } else {
    return false;
  }
}
