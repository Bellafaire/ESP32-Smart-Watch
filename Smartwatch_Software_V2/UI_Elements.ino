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

/* Elements used in the UI design of this project, things that can be abstracted to classes
   should be to ease development of user interaction.

*/

/********************************************************************
                         Animation Circles
                meant to look like a kind of alchemy circle
                they spin on the screen to add some life to
                the overall interface, but they can be used
                to indicate various conditions by changing
                color or speed of rotation
 ********************************************************************/

AnimationCircle:: AnimationCircle(int _x, int _y, int _rotationRadius, int _circleRadius, int _ringColor, int _color, float _speed, int _circleNumber) {
  rotationRadius = _rotationRadius;
  circleRadius = _circleRadius;
  color = _color;
  ringColor = _ringColor;
  circleNumber = _circleNumber;
  speed = _speed;
  x = _x;
  y = _y;

}

void AnimationCircle::animateAndDraw(GFXcanvas16 *buffer) {
  buffer->drawCircle(x, y, rotationRadius, ringColor);

  for (int a = 0; a < circleNumber; a++) {
    int xPos = x + (float) rotationRadius * sin((float)theta * PI / 180 + (2 * PI * (float)a / (float)circleNumber));
    int yPos = y + (float) rotationRadius * cos((float)theta * PI / 180 + (2 * PI * (float)a / (float)circleNumber));

    int xPos2 = x + (float) rotationRadius * sin((float)theta * PI / 180 + (2 * PI * (float)(a + 1 + (circleNumber % 2)) / (float)circleNumber));
    int yPos2 = y + (float) rotationRadius * cos((float)theta * PI / 180 + (2 * PI * (float)(a + 1 + (circleNumber % 2)) / (float)circleNumber));

    buffer->drawLine(xPos, yPos, xPos2, yPos2, ringColor);
    buffer->fillCircle(xPos, yPos, circleRadius, color);
    buffer->drawCircle(xPos, yPos, circleRadius, ringColor);

    buffer->fillCircle(xPos2, yPos2, circleRadius, color);
    buffer->drawCircle(xPos2, yPos2, circleRadius, ringColor);
  }
  theta += speed;
}

void AnimationCircle::setColor(uint16_t _color) {
  color = _color;
}

void  AnimationCircle::setRadius(int newRadius) {
  rotationRadius = newRadius;
}

void  AnimationCircle::setSpeed(float newSpeed) {
  speed = newSpeed;
}


/********************************************************************
                              Buttons

                 Round buttons will use an Icon and
                 when placed on screen automatically
                   assign themselves a touch area
 ********************************************************************/
RoundButton::RoundButton(int _x, int _y, int _radius, uint16_t _icon[16], void* _action) {
  x = _x;
  y = _y;
  radius = _radius;
  action = _action;
  touchAreaID = createTouchArea(_x - _radius, y - _radius , _radius * 2, _radius * 2, _action);

  active = true;

  //better ways to do this
  for (int a = 0; a < 16; a++) {
    icon[a] = _icon[a];
  }
}


boolean RoundButton::isActive() {
  return active;
}

void   RoundButton::draw(GFXcanvas16 * buffer) {
  if (active) {
    buffer->fillCircle(x, y, radius, BACKGROUND_COLOR);
    buffer->drawCircle(x, y, radius, INTERFACE_COLOR);

    for (int row = 0; row < 16; row++) {
      for (int column = 0; column < 16; column++) {
        buffer->drawPixel(
          x - 8 + column,
          y - 8 + row,
          (icon[row] & (1 << (15 - column))) ? INTERFACE_COLOR : BACKGROUND_COLOR
        );
      }
    }
  }
}

void  RoundButton::activate() {
  touchAreaID = createTouchArea(x - radius, y - radius , radius * 2, radius * 2, action);
  active = true;
}
void RoundButton::deactivate() {
  deactivateTouchArea(touchAreaID);
  active = false;
}

RoundButton::RoundButton() {
}


/********************************************************************
                           LEGACY BUTTONS
 ********************************************************************/
//determines width of text displayed on LCD screen, default text has width of 5 pixels + 1 space pixel
int TextWidth(struct onscreenButton b) {
  return b._text.length() * 6;
}


void paintButton(struct onscreenButton b) {
  frameBuffer->drawRect(b._x, b._y, b._width, b._height, b._color);
  frameBuffer->setTextColor(b._color);
  frameBuffer->setCursor(b._x + (int)(b._width - TextWidth(b)) / 2, b._y + (int)b._height / 2 - 5);
  frameBuffer->println(b._text);
}

void paintButton(struct iconButton b) {
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



void paintButtonNoBuffer(struct onscreenButton b) {
  tft.fillRect(b._x, b._y, b._width, b._height, b._backgroundColor);
  tft.drawRect(b._x, b._y, b._width, b._height, b._color);
  tft.setTextColor(b._color);
  tft.setCursor(b._x + (int)(b._width - TextWidth(b)) / 2, b._y + (int)b._height / 2 - 5);
  tft.println(b._text);
}


void paintButtonNoBuffer(struct iconButton b) {
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



void paintButtonFull(struct onscreenButton b) {
  frameBuffer->fillRect(b._x, b._y, b._width, b._height, b._backgroundColor);
  frameBuffer->drawRect(b._x, b._y, b._width, b._height, b._color);
  frameBuffer->setTextColor(b._color);
  frameBuffer->setCursor(b._x + (int)(b._width - TextWidth(b)) / 2, b._y + (int)b._height / 2 - 5);
  frameBuffer->println(b._text);
}


void paintButtonFull(struct iconButton b) {
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

void pressButton(struct iconButton b) {
  frameBuffer->fillRect(b._x, b._y, b._width, b._height, b._color);
}

void pressButton(struct onscreenButton b) {
  frameBuffer->fillRect(b._x, b._y, b._width, b._height, b._color);
}

bool checkButtonPress(struct onscreenButton b) {
  struct point p = readTouch();
  return checkButtonPress(b, p.x, p.x);
}

bool checkButtonPress(struct iconButton b) {
  struct point p = readTouch();
  return checkButtonPress(b, p.x, p.x);
}

bool checkButtonPress(struct iconButton b, int x, int y) {
  if (x > b._x && x < b._x + b._width && y > b._y && y < b._y + b._height) {
    return true;
  } else {
    return false;
  }
}

bool checkButtonPress(struct onscreenButton b, int x, int y) {
  if (x > b._x && x < b._x + b._width && y > b._y && y < b._y + b._height) {
    return true;
  } else {
    return false;
  }
}

/********************************************************************
                           SELECTION WINDOW
 ********************************************************************/
/* SelectionWindow Class
  A simple Instanceable window which can have options dynamically added to it and returns
  the option selected by the user, intended to simply option selection code.
*/
void SelectionWindow::drawOptionsWindow()
{
  // frameBuffer -> drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);
  frameBuffer->fillRect(_x, _y, _width, _height, BACKGROUND_COLOR);

  int maxOptions = _height / 8;
  int scrollPosition = selection / maxOptions;

  frameBuffer->setTextSize(1);
  frameBuffer->setTextColor(INTERFACE_COLOR);

  for (int a = scrollPosition * maxOptions; a < (scrollPosition + 1)*maxOptions; a++)
  {
    frameBuffer->setCursor(_x + 1, _y + (a - scrollPosition * maxOptions) * 8 + 1);

    if (a == selection)
    {
      frameBuffer->setTextColor(BACKGROUND_COLOR);
      frameBuffer->fillRect(_x + 1, _y + (a - scrollPosition * maxOptions) * 8, _width - 16, 8, INTERFACE_COLOR);
    }
    else
    {
      frameBuffer->setTextColor(INTERFACE_COLOR);
      frameBuffer->fillRect(_x + 1, _y + (a - scrollPosition * maxOptions) * 8, _width - 16, 8, BACKGROUND_COLOR);
    }
    frameBuffer->print(getValue(options, FIELD_SEPARATOR, a));
  }
  frameBuffer->drawRect(_x, _y, _width, _height, INTERFACE_COLOR);
  paintButtonFull(UpArrowButton);
  paintButtonFull(okButton);
  paintButtonFull(DownArrowButton);

  tft.drawRGBBitmap(0, 0, frameBuffer->getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);
}


//focuses the selection window (essentially gives full control of the micro controller to this object)
int SelectionWindow::focus()
{

  focused = true;
  drawOptionsWindow();
  pauseTouchAreas();

  //don't allow the main thread to draw to the screen
  //for the duration of the selection screen being available
  //we need full control of UI Drawing.
  //this should be called outside of the focus function
  //  drawInLoop = false;

  while (focused)
  {
    touch();
    drawOptionsWindow();
  }
  //let the touch areas operate again
  resumeTouchAreas();
  return selection;
}

//adds option to the list, appending it to the options string with the delimiter
int SelectionWindow::addOption(String s)
{
  options += FIELD_SEPARATOR + s;
  totalOptions++;
  return totalOptions - 1; //default "cancel" option is automatically included so we can't count that one
}

//touch handling, checks buttons and handles functionality associated with them
void SelectionWindow::touch()
{
  //check if the screen is actually touched
  if (!digitalRead(TOUCH_IRQ))
  {
    struct point p = readTouch();

    if (checkButtonPress(UpArrowButton, p.x, p.y) && selection > 0)
    {

      selection--;
      drawOptionsWindow();
    }
    if (checkButtonPress(okButton, p.x, p.y))
    {
      focused = false;
    }
    if (checkButtonPress(DownArrowButton, p.x, p.y) && selection < totalOptions - 1)
    {
      selection++;
      drawOptionsWindow();
    }

  }
}

//constructor
SelectionWindow::SelectionWindow(int x, int y, int width, int height)
{
  //set size and position of the window, we can only go so small so this is just a catch in case of trouble
  if (height < 48 || width < SELECTION_WINDOW_BUTTON_WIDTH + 30)
  {
    _x = x;
    _y = y;
    _width = SELECTION_WINDOW_BUTTON_WIDTH + 30;
    _height = 48;
  }
  else
  {
    _x = x;
    _y = y;
    _width = width;
    _height = height;
  }

  int heightSpacing = (_height) / 3;
  UpArrowButton = {_x + _width - SELECTION_WINDOW_BUTTON_WIDTH, _y, SELECTION_WINDOW_BUTTON_WIDTH, heightSpacing, INTERFACE_COLOR, BACKGROUND_COLOR, {(0b00000001 << 8) | 0b10000000, (0b00000011 << 8) | 0b11000000, (0b00000111 << 8) | 0b11100000, (0b00001111 << 8) | 0b11110000, (0b00011111 << 8) | 0b11111000, (0b00111111 << 8) | 0b11111100, (0b01111111 << 8) | 0b11111110, (0b11111111 << 8) | 0b11111111, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000}};
  okButton = {_x + _width - SELECTION_WINDOW_BUTTON_WIDTH, heightSpacing + _y, SELECTION_WINDOW_BUTTON_WIDTH, heightSpacing, INTERFACE_COLOR, BACKGROUND_COLOR, "Ok"};
  DownArrowButton = {_x + _width - SELECTION_WINDOW_BUTTON_WIDTH, _y + heightSpacing * 2, SELECTION_WINDOW_BUTTON_WIDTH, heightSpacing, INTERFACE_COLOR, BACKGROUND_COLOR, {(0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b00001111 << 8) | 0b11110000, (0b11111111 << 8) | 0b11111111, (0b01111111 << 8) | 0b11111110, (0b00111111 << 8) | 0b11111100, (0b00011111 << 8) | 0b11111000, (0b00001111 << 8) | 0b11110000, (0b00000111 << 8) | 0b11100000, (0b00000011 << 8) | 0b11000000, (0b00000001 << 8) | 0b10000000}};

  //  //clear background and draw outline
  //  frameBuffer->fillRect(_x, _y, _width, _height, BACKGROUND_COLOR);
  //  frameBuffer->drawRect(_x, _y, _width, _height, INTERFACE_COLOR);
  //
  //init selection
  selection = 0;
  //
  //  //draw buttons
  //  paintButtonFull(DownArrowButton);
  //  paintButtonFull(UpArrowButton);
  //  paintButtonFull(okButton);

  drawOptionsWindow();
}

//taken from stack overflow https://stackoverflow.com/questions/9072320/split-string-into-string-array
String SelectionWindow::getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return  found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


/********************************************************************
                              ICONS
 ********************************************************************/
uint16_t HOME_ICON[] =  {  (0b00000000 << 8) | 0b00000000,  (0b00000000 << 8) | 0b00000000,  (0b00000001 << 8) | 0b10000000,  (0b00000011 << 8) | 0b11000000,  (0b00000111 << 8) | 0b11100000,  (0b00001111 << 8) | 0b11110000,  (0b00111111 << 8) | 0b11111100,  (0b01111111 << 8) | 0b11111110,  (0b11111111 << 8) | 0b11111111,  (0b00111111 << 8) | 0b11111100,  (0b00111110 << 8) | 0b00111100,  (0b00111110 << 8) | 0b00111100,  (0b00111110 << 8) | 0b00111100,  (0b00111110 << 8) | 0b00111100,  (0b00111110 << 8) | 0b00111100,  (0b00000000 << 8) | 0b00000000};
uint16_t NOTIFICATIONS_ICON[]  = { (0b00100001 << 8) | 0b10000100,    (0b01000111 << 8) | 0b11100010,    (0b10001100 << 8) | 0b00110001,    (0b10011000 << 8) | 0b00011001,    (0b10010000 << 8) | 0b00001001,    (0b00110000 << 8) | 0b00001100,    (0b00110000 << 8) | 0b00001100,    (0b00110000 << 8) | 0b00001100,    (0b00110000 << 8) | 0b00001100,    (0b00110000 << 8) | 0b00001100,    (0b00100000 << 8) | 0b00000100,    (0b01100000 << 8) | 0b00000110,    (0b11000000 << 8) | 0b00000011,    (0b11000000 << 8) | 0b00000011,    (0b11111111 << 8) | 0b11111111,    (0b00000011 << 8) | 0b11000000  };
uint16_t SETTINGS_ICON[]  =  {    (0b00000000 << 8) | 0b00001110,    (0b00000000 << 8) | 0b00011111,    (0b11111111 << 8) | 0b11011111,    (0b11111111 << 8) | 0b11011111,    (0b00000000 << 8) | 0b00001110,    (0b00001100 << 8) | 0b00000000,    (0b11011111 << 8) | 0b01111111,    (0b11011111 << 8) | 0b01111111,    (0b00011111 << 8) | 0b00000000,    (0b00001110 << 8) | 0b00001110,    (0b00000000 << 8) | 0b00011111,    (0b11111111 << 8) | 0b11011111,    (0b11111111 << 8) | 0b11011111,    (0b00000000 << 8) | 0b00001110,    (0b00000000 << 8) | 0b00000000,    (0b00000000 << 8) | 0b00000000  };
uint16_t MEDIA_PLAY_ICON[]  = {  (0b00000000 << 8) | 0b00000000,  (0b01100000 << 8) | 0b00000000,  (0b01111000 << 8) | 0b00000000,  (0b01111110 << 8) | 0b00000000,  (0b01111111 << 8) | 0b10000000,  (0b01111111 << 8) | 0b11100000,  (0b01111111 << 8) | 0b11111000,  (0b01111111 << 8) | 0b11111110,  (0b01111111 << 8) | 0b11111110,  (0b01111111 << 8) | 0b11111000,  (0b01111111 << 8) | 0b11100000,  (0b01111111 << 8) | 0b10000000,  (0b01111110 << 8) | 0b00000000,  (0b01111000 << 8) | 0b00000000,  (0b01100000 << 8) | 0b00000000,  (0b00000000 << 8) | 0b00000000};
uint16_t MEDIA_PAUSE_ICON[]  = {  (0b00000000 << 8) | 0b00000000,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b00000000 << 8) | 0b00000000};
uint16_t MEDIA_NEXT_ICON[]  = {  (0b00000000 << 8) | 0b00000000,  (0b01000000 << 8) | 0b01000000,  (0b01100000 << 8) | 0b01100000,  (0b01110000 << 8) | 0b01110000,  (0b01111000 << 8) | 0b01111000,  (0b01111100 << 8) | 0b01111100,  (0b01111110 << 8) | 0b01111110,  (0b01111111 << 8) | 0b01111111,  (0b01111111 << 8) | 0b01111111,  (0b01111110 << 8) | 0b01111110,  (0b01111100 << 8) | 0b01111100,  (0b01111000 << 8) | 0b01111000,  (0b01110000 << 8) | 0b01110000,  (0b01100000 << 8) | 0b01100000,  (0b01000000 << 8) | 0b01000000,  (0b00000000 << 8) | 0b00000000};
uint16_t MEDIA_LAST_ICON[]  = {  (0b00000000 << 8) | 0b00000000,  (0b00000010 << 8) | 0b00000010,  (0b00000110 << 8) | 0b00000110,  (0b00001110 << 8) | 0b00001110,  (0b00011110 << 8) | 0b00011110,  (0b00111110 << 8) | 0b00111110,  (0b01111110 << 8) | 0b01111110,  (0b11111110 << 8) | 0b11111110,  (0b11111110 << 8) | 0b11111110,  (0b01111110 << 8) | 0b01111110,  (0b00111110 << 8) | 0b00111110,  (0b00011110 << 8) | 0b00011110,  (0b00001110 << 8) | 0b00001110,  (0b00000110 << 8) | 0b00000110,  (0b00000010 << 8) | 0b00000010,  (0b00000000 << 8) | 0b00000000};
uint16_t UP_ARROW_ICON[]  = {(0b00000001 << 8) | 0b10000000,  (0b00000011 << 8) | 0b11000000,  (0b00000111 << 8) | 0b11100000,  (0b00001111 << 8) | 0b11110000,  (0b00011111 << 8) | 0b11111000,  (0b00111111 << 8) | 0b11111100,  (0b01111111 << 8) | 0b11111110,  (0b11111111 << 8) | 0b11111111,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000};
uint16_t RIGHT_ARROW_ICON[]  =  {(0b00000000 << 8) | 0b10000000,  (0b00000000 << 8) | 0b11000000,  (0b00000000 << 8) | 0b11100000,  (0b00000000 << 8) | 0b11110000,  (0b11111111 << 8) | 0b11111000,  (0b11111111 << 8) | 0b11111100,  (0b11111111 << 8) | 0b11111110,  (0b11111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111110,  (0b11111111 << 8) | 0b11111100,  (0b11111111 << 8) | 0b11111000,  (0b00000000 << 8) | 0b11110000,  (0b00000000 << 8) | 0b11100000,  (0b00000000 << 8) | 0b11000000,  (0b00000000 << 8) | 0b10000000};
uint16_t DOWN_ARROW_ICON[]  = {(0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b00001111 << 8) | 0b11110000,  (0b11111111 << 8) | 0b11111111,  (0b01111111 << 8) | 0b11111110,  (0b00111111 << 8) | 0b11111100,  (0b00011111 << 8) | 0b11111000,  (0b00001111 << 8) | 0b11110000,  (0b00000111 << 8) | 0b11100000,  (0b00000011 << 8) | 0b11000000,  (0b00000001 << 8) | 0b10000000};
uint16_t LEFT_ARROW_ICON[]  = { (0b00000001 << 8) | 0b00000000,  (0b00000011 << 8) | 0b00000000,  (0b00000111 << 8) | 0b00000000,  (0b00001111 << 8) | 0b00000000,  (0b00011111 << 8) | 0b11111111,  (0b00111111 << 8) | 0b11111111,  (0b01111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111111,  (0b01111111 << 8) | 0b11111111,  (0b00111111 << 8) | 0b11111111,  (0b00011111 << 8) | 0b11111111,  (0b00001111 << 8) | 0b00000000,  (0b00000111 << 8) | 0b00000000,  (0b00000011 << 8) | 0b00000000,  (0b00000001 << 8) | 0b00000000  };
