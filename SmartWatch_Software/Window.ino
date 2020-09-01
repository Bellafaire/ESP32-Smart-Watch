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
/* due to the limited screen size this class is designed to be called in functions to display larger amounts of text to the user.
    Allows for nearly unlimted string data to be stored in the textbox. and then opened/closed
*/

boolean Window::isFocused()
{
  return focused;
}

//sets focus of the window (basically locks everything out except the window until the user touches off the window)
void Window::focus()
{
  focused = true;
  while (focused)
  {
    touch();
    delay(5);
  }
}

//blank constructor (you'll know when you need it)
Window::Window()
{}

//constructor, the width and height should be less than the actual screen. since the exiting method is to
//tap outside of the window
Window::Window(int x, int y, int width, int height, boolean scroll)
{
  _scroll = scroll;

  //if we allow the user to scroll through the text then we need to draw the screen differently
  if (scroll)
  {
    _x = x;
    _y = y;
    x_cursor = x + 2;
    y_cursor = y + 3;
    _width = width - 20; //and change the effective text area
    _height = height;

    //fill in background so we can actually read the text
    tft.fillRect(_x, _y, width, height, BACKGROUND_COLOR);
    tft.drawRect(_x, _y, _width, _height, INTERFACE_COLOR);

    //now add buttons to the right hand side, we use icon buttons for obvious reasons
    WindowUpArrowButton._x = x + width - 20;
    WindowUpArrowButton._width = 20;
    WindowUpArrowButton._height = height / 2;
    WindowUpArrowButton._y = y;

    WindowDownArrowButton._x = x + width - 20;
    WindowDownArrowButton._width = 20;
    WindowDownArrowButton._height = height / 2;
    WindowDownArrowButton._y = y + height / 2;

    //paint buttons
    paintButtonNoBuffer(WindowUpArrowButton);
    paintButtonNoBuffer(WindowDownArrowButton);
  }
  else
  {
    /*  if we don't allow scrolling then we really don't have nearly as much that needs to be
      done to draw the screen, still more-or-less the same steps though
    */
    _x = x;
    _y = y;
    x_cursor = x + 2;
    y_cursor = y + 1;
    _width = width;
    _height = height;
    tft.fillRect(_x, _y, width, height, BACKGROUND_COLOR);
    tft.drawRect(_x, _y, _width, _height, INTERFACE_COLOR);
  }
}

/*touch handling for all buttons and the screen itself. this function was designed to be completely encapsulated.
*/
void Window::touch()
{
  //check if the screen is actually touched
  if (!digitalRead(TOUCH_IRQ))
  {
    struct point p = readTouch();

#ifdef DEBUG
    Serial.println(String(p.xPos) + " " + String(p.yPos));
#endif
    //if we have scroll buttons then we check the touch status of each of those buttons
    if (_scroll)
    {
      if (checkButtonPress(WindowUpArrowButton, p.xPos, p.yPos))
      {
        if (scrollPosition > 0)
        {
          scrollPosition--;
          drawTextToWindow(true); //since we scrolled we need to redraw the text
        }
      }
      if (checkButtonPress(WindowDownArrowButton, p.xPos, p.yPos))
      {
        scrollPosition++;
        drawTextToWindow(true); //since we scrolled we need to redraw the text
      }

      //repaint buttons in case something went wrong and text overflowed
      paintButtonNoBuffer(WindowUpArrowButton);
      paintButtonNoBuffer(WindowDownArrowButton);

      //check if user tapped outside of window (this closes window)
      if (p.xPos < _x || p.yPos < _y || p.xPos > _x + _width + 32 || p.yPos > _y + _height)
      {
        focused = false;
      }
    }
    else
    {
      //if we don't have scroll arrows then we only need to check whether touches are inside the window.
      if (p.xPos < _x || p.yPos < _y || p.xPos > _x + _width || p.yPos > _y + _height)
      {
        focused = false;
      }
    }
  }
}

/* this function takes input strings and appends them to the object's textBuffer. this allows (nearly) unlimited data to be added to the
    window to be read by the user.
*/
void Window::print(String Text)
{
  textBuffer += Text;
  drawTextToWindow(false);
}

//draw the text to the screen (inside the window)
void Window::drawTextToWindow(boolean clr) {

}

//print but with line break
void Window::println(String Text)
{
  print(Text + '\n');
}

//taken from stack overflow https://stackoverflow.com/questions/9072320/split-string-into-string-array
String Window::getValue(String data, char separator, int index)
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

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
