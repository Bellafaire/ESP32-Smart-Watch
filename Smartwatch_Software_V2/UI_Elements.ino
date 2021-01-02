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


/********************************************************************
                         Animation Circles
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
