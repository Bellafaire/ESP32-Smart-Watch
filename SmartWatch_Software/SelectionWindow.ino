#define optionDivider '`' //change this to anything you don't think will be used
#define SELECTION_WINDOW_BUTTON_WIDTH 20

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

  for (int a = scrollPosition; a < maxOptions; a++)
  {
    frameBuffer->setCursor(_x + 1, _y + a * 8 + 1);

    if (a == selection)
    {
      frameBuffer->setTextColor(BACKGROUND_COLOR);
      frameBuffer->fillRect(_x + 1, _y + a * 8, _width - 16, 8, INTERFACE_COLOR);
    }
    else
    {
      frameBuffer->setTextColor(INTERFACE_COLOR);
      frameBuffer->fillRect(_x + 1, _y + a * 8, _width - 16, 8, BACKGROUND_COLOR);
    }
    frameBuffer->print(getValue(options, optionDivider, a));
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
  while (focused)
  {
    touch();
    delay(5);
  }
  delay(200);
  return selection;
}

//adds option to the list, appending it to the options string with the delimiter
int SelectionWindow::addOption(String s)
{
  options += optionDivider + s;
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

    if (checkButtonPress(UpArrowButton, p.xPos, p.yPos) && selection > 0)
    {

      selection--;
      drawOptionsWindow();
      delay(80);
    }
    if (checkButtonPress(okButton, p.xPos, p.yPos))
    {
      focused = false;
    }
    if (checkButtonPress(DownArrowButton, p.xPos, p.yPos) && selection < totalOptions - 1)
    {
      selection++;
      drawOptionsWindow();
      delay(80);
    }
    drawOptionsWindow();
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

  //clear background and draw outline
  tft.fillRect(_x, _y, _width, _height, BACKGROUND_COLOR);
  tft.drawRect(_x, _y, _width, _height, INTERFACE_COLOR);

  //init selection
  selection = 0;

  //draw buttons
  paintButtonNoBuffer(DownArrowButton);
  paintButtonNoBuffer(UpArrowButton);
  paintButtonNoBuffer(okButton);
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

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
