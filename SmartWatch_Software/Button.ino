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

//determines width of text displayed on LCD screen, default text has width of 5 pixels + 1 space pixel
int TextWidth(button b) {
  return b._text.length() * 6;
}


void paintButton(button b) {
  frameBuffer->drawRect(b._x, b._y, b._width, b._height, b._color);
  frameBuffer->setTextColor(b._color);
  frameBuffer->setCursor(b._x + (int)(b._width - TextWidth(b)) / 2, b._y + (int)b._height / 2 - 5);
  frameBuffer->println(b._text);
}

void paintButton(iconButton b) {
  frameBuffer->drawRect(b._x, b._y, b._width, b._height, b._color);
  for (int row = 0; row < 16; row++) {
    for (int column = 0; column < 16; column++) {
      frameBuffer->drawPixel(
        b._x + b._width / 2 - 8 + column,
        b._y + b._height / 2 - 8 + row,
        (b.icon[row] & (1 << (15 - column))) ? b._color : b._backgroundColor
      );
    }
  }
}



void paintButtonNoBuffer(button b) {
  tft.fillRect(b._x, b._y, b._width, b._height, b._backgroundColor);
  tft.drawRect(b._x, b._y, b._width, b._height, b._color);
  tft.setTextColor(b._color);
  tft.setCursor(b._x + (int)(b._width - TextWidth(b)) / 2, b._y + (int)b._height / 2 - 5);
  tft.println(b._text);
}


void paintButtonNoBuffer(iconButton b) {
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



void paintButtonFull(button b) {
  frameBuffer->fillRect(b._x, b._y, b._width, b._height, b._backgroundColor);
  frameBuffer->drawRect(b._x, b._y, b._width, b._height, b._color);
  frameBuffer->setTextColor(b._color);
  frameBuffer->setCursor(b._x + (int)(b._width - TextWidth(b)) / 2, b._y + (int)b._height / 2 - 5);
  frameBuffer->println(b._text);
}


void paintButtonFull(iconButton b) {
  frameBuffer->fillRect(b._x, b._y, b._width, b._height, b._backgroundColor);
  frameBuffer->drawRect(b._x, b._y, b._width, b._height, b._color);
  for (int row = 0; row < 16; row++) {
    for (int column = 0; column < 16; column++) {
      frameBuffer->drawPixel(
        b._x + b._width / 2 - 8 + column,
        b._y + b._height / 2 - 8 + row,
        (b.icon[row] & (1 << (15 - column))) ? b._color : b._backgroundColor
      );
    }
  }
}

void pressButton(iconButton b) {
  frameBuffer->fillRect(b._x, b._y, b._width, b._height, b._color);
}

void pressButton(button b) {
  frameBuffer->fillRect(b._x, b._y, b._width, b._height, b._color);
}

bool checkButtonPress(button b) {
  struct point p = readTouch();
  return checkButtonPress(b, p.xPos, p.yPos);
}

bool checkButtonPress(iconButton b) {
  struct point p = readTouch();
  return checkButtonPress(b, p.xPos, p.yPos);
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
