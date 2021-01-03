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
 * should be to ease development of user interaction. 
 * 
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
                              ICONS
 ********************************************************************/
uint16_t HOME_ICON[] =  {  (0b00000000 << 8) | 0b00000000,  (0b00000000 << 8) | 0b00000000,  (0b00000001 << 8) | 0b10000000,  (0b00000011 << 8) | 0b11000000,  (0b00000111 << 8) | 0b11100000,  (0b00001111 << 8) | 0b11110000,  (0b00111111 << 8) | 0b11111100,  (0b01111111 << 8) | 0b11111110,  (0b11111111 << 8) | 0b11111111,  (0b00111111 << 8) | 0b11111100,  (0b00111110 << 8) | 0b00111100,  (0b00111110 << 8) | 0b00111100,  (0b00111110 << 8) | 0b00111100,  (0b00111110 << 8) | 0b00111100,  (0b00111110 << 8) | 0b00111100,  (0b00000000 << 8) | 0b00000000};
uint16_t NOTIFICATIONS_ICON[]  = { (0b00100001 << 8) | 0b10000100,    (0b01000111 << 8) | 0b11100010,    (0b10001100 << 8) | 0b00110001,    (0b10011000 << 8) | 0b00011001,    (0b10010000 << 8) | 0b00001001,    (0b00110000 << 8) | 0b00001100,    (0b00110000 << 8) | 0b00001100,    (0b00110000 << 8) | 0b00001100,    (0b00110000 << 8) | 0b00001100,    (0b00110000 << 8) | 0b00001100,    (0b00100000 << 8) | 0b00000100,    (0b01100000 << 8) | 0b00000110,    (0b11000000 << 8) | 0b00000011,    (0b11000000 << 8) | 0b00000011,    (0b11111111 << 8) | 0b11111111,    (0b00000011 << 8) | 0b11000000  };
uint16_t SETTINGS_ICON[]  =  {    (0b00000000 << 8) | 0b00001110,    (0b00000000 << 8) | 0b00011111,    (0b11111111 << 8) | 0b11011111,    (0b11111111 << 8) | 0b11011111,    (0b00000000 << 8) | 0b00001110,    (0b00001100 << 8) | 0b00000000,    (0b11011111 << 8) | 0b01111111,    (0b11011111 << 8) | 0b01111111,    (0b00011111 << 8) | 0b00000000,    (0b00001110 << 8) | 0b00001110,    (0b00000000 << 8) | 0b00011111,    (0b11111111 << 8) | 0b11011111,    (0b11111111 << 8) | 0b11011111,    (0b00000000 << 8) | 0b00001110,    (0b00000000 << 8) | 0b00000000,    (0b00000000 << 8) | 0b00000000  };
uint16_t MEDIA_PLAY_ICON[]  = {  (0b00000000 << 8) | 0b00000000,  (0b01100000 << 8) | 0b00000000,  (0b01111000 << 8) | 0b00000000,  (0b01111110 << 8) | 0b00000000,  (0b01111111 << 8) | 0b10000000,  (0b01111111 << 8) | 0b11100000,  (0b01111111 << 8) | 0b11111000,  (0b01111111 << 8) | 0b11111110,  (0b01111111 << 8) | 0b11111110,  (0b01111111 << 8) | 0b11111000,  (0b01111111 << 8) | 0b11100000,  (0b01111111 << 8) | 0b10000000,  (0b01111110 << 8) | 0b00000000,  (0b01111000 << 8) | 0b00000000,  (0b01100000 << 8) | 0b00000000,  (0b00000000 << 8) | 0b00000000};
uint16_t MEDIA_PAUSE_ICON[]  = {  (0b00000000 << 8) | 0b00000000,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b01111110 << 8) | 0b01111110,  (0b00000000 << 8) | 0b00000000};
uint16_t MEDIA_NEXT_ICON[]  = {  (0b00000000 << 8) | 0b00000000,  (0b01000000 << 8) | 0b01000000,  (0b01100000 << 8) | 0b01100000,  (0b01110000 << 8) | 0b01110000,  (0b01111000 << 8) | 0b01111000,  (0b01111100 << 8) | 0b01111100,  (0b01111110 << 8) | 0b01111110,  (0b01111111 << 8) | 0b01111111,  (0b01111111 << 8) | 0b01111111,  (0b01111110 << 8) | 0b01111110,  (0b01111100 << 8) | 0b01111100,  (0b01111000 << 8) | 0b01111000,  (0b01110000 << 8) | 0b01110000,  (0b01100000 << 8) | 0b01100000,  (0b01000000 << 8) | 0b01000000,  (0b00000000 << 8) | 0b00000000};
uint16_t MEDIA_LAST_ICON[]  = {  (0b00000000 << 8) | 0b00000000,  (0b00000010 << 8) | 0b00000010,  (0b00000110 << 8) | 0b00000110,  (0b00001110 << 8) | 0b00001110,  (0b00011110 << 8) | 0b00011110,  (0b00111110 << 8) | 0b00111110,  (0b01111110 << 8) | 0b01111110,  (0b11111110 << 8) | 0b11111110,  (0b11111110 << 8) | 0b11111110,  (0b01111110 << 8) | 0b01111110,  (0b00111110 << 8) | 0b00111110,  (0b00011110 << 8) | 0b00011110,  (0b00001110 << 8) | 0b00001110,  (0b00000110 << 8) | 0b00000110,  (0b00000010 << 8) | 0b00000010,  (0b00000000 << 8) | 0b00000000};
