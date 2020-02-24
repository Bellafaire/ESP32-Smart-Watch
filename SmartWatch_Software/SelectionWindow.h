#define optionDivider '`' //change this to anything you don't think will be used

/* a simple menu to allow a user to select from a list of choices, returns an integer for the selected value */
class SelectionWindow {
    int selection;
    int sectionWidth;
    boolean focused = false;
    int _x, _y, _width, _height;
    int totalOptions = 1;
    String options = "Cancel";
    iconButton RightArrowButton =   { 128, 0, 32, 48, INTERFACE_COLOR, BACKGROUND_COLOR,   {(0b00000000 << 8) | 0b10000000,  (0b00000000 << 8) | 0b11000000,  (0b00000000 << 8) | 0b11100000,  (0b00000000 << 8) | 0b11110000,  (0b11111111 << 8) | 0b11111000,  (0b11111111 << 8) | 0b11111100,  (0b11111111 << 8) | 0b11111110,  (0b11111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111110,  (0b11111111 << 8) | 0b11111100,  (0b11111111 << 8) | 0b11111000,  (0b00000000 << 8) | 0b11110000,  (0b00000000 << 8) | 0b11100000,  (0b00000000 << 8) | 0b11000000,  (0b00000000 << 8) | 0b10000000}};
    iconButton LeftArrowButton =    {    128, 0, 32, 48, INTERFACE_COLOR, BACKGROUND_COLOR,  {      (0b00000001 << 8) | 0b00000000,      (0b00000011 << 8) | 0b00000000,  (0b00000111 << 8) | 0b00000000,  (0b00001111 << 8) | 0b00000000,  (0b00011111 << 8) | 0b11111111,  (0b00111111 << 8) | 0b11111111,  (0b01111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111111,  (0b11111111 << 8) | 0b11111111,  (0b01111111 << 8) | 0b11111111,  (0b00111111 << 8) | 0b11111111,  (0b00011111 << 8) | 0b11111111,  (0b00001111 << 8) | 0b00000000,  (0b00000111 << 8) | 0b00000000,  (0b00000011 << 8) | 0b00000000,  (0b00000001 << 8) | 0b00000000    }  };
    button okButton =  {128, 0, 32, 48, INTERFACE_COLOR, BACKGROUND_COLOR, "Ok"};

  public:
    SelectionWindow(int x, int y, int width, int height);  //constructor determines form/size
    int addOption(String s); //adds option and returns option number
    int focus(); //focuses the window, will return integer representitive of the selected option
  private:
    void drawCurrentOption(); //draws the option's string to the screen
    String getValue(String data, char separator, int index); //splits the options string
    void touch(); //touch handler
};

void SelectionWindow::drawCurrentOption() {

  //first divide up the string to obtain the currently selected option
  String currentOption = getValue(options, optionDivider, selection);

  //clear the background and configure the text just to ensure there are no issues
  tft.fillRect(_x + 4, _y + _height / 3, _width - 8,8, BACKGROUND_COLOR);
  tft.setTextSize(1);
  tft.setTextColor(INTERFACE_COLOR);
  tft.setCursor(_x + 4, _y + _height / 3);

  //now, if the string can fit inside the window then just center it, otherwise chop off the end and add "..." to indicate overflow
  if (currentOption.length() * 6 < _width - 8) {
    tft.setCursor(
      _x + (_width - currentOption.length() * 6 ) / 2,
      _y +  _height / 3);
    tft.print(currentOption);
  } else {
    int maxFittingCharacters = (_width - 8) / 6; //how many characters can we fit then?
    tft.setCursor(
      _x + 4,
      _y +  _height / 3);
    for (int a = 0; a < maxFittingCharacters - 3; a++) {
      tft.print(currentOption[a]);
    }
    tft.print("...");
  }
}

//focuses the selection window (essentially gives full control of the micro controller to this object) 
int SelectionWindow::focus() {
  focused = true;
  while (focused) {
    touch();
    delay(5);
  }
  return selection;
}

//adds option to the list, appending it to the options string with the delimiter 
int SelectionWindow::addOption(String s) {
  options += optionDivider + s;
  totalOptions++;
  return totalOptions - 1;
}

//touch handling, checks buttons and handles functionality associated with them
void SelectionWindow::touch() {
  //check if the screen is actually touched
  if (ts.touched()) {
    int x, y;
    getTouchedPosition(&x, &y);

    if (checkButtonPress(RightArrowButton, x, y) && selection < totalOptions - 1) {
      selection ++;
      drawCurrentOption();
      delay(300);
    }
    if (checkButtonPress(okButton, x, y) ) {
      focused = false;
    }
    if (checkButtonPress(LeftArrowButton, x, y) && selection > 0 ) {
      selection--;
      drawCurrentOption();
      delay(300);
    }
  }
}

//constructor
SelectionWindow::SelectionWindow(int x, int y, int width, int height) {
  //set size and position of the window
  _x = x;
  _y = y;
  _width = width;
  _height = height;

  //clear background and draw outline
  tft.fillRect(_x, _y, _width, _height, BACKGROUND_COLOR);
  tft.drawRect(_x, _y, _width, _height, INTERFACE_COLOR);

  //init selection
  selection = 0;

  //set position and size of buttons (since they depend on the size of the window)
  LeftArrowButton._x = _x;
  LeftArrowButton._y = _y + 2 * _height / 3;
  LeftArrowButton._width = _width / 3;
  LeftArrowButton._height = _height / 3;

  RightArrowButton._x = _x + 2 * _width / 3;
  RightArrowButton._y = _y + 2 * _height / 3;
  RightArrowButton._width = _width / 3;
  RightArrowButton._height = _height / 3;

  okButton._x = _x + _width / 3;
  okButton._y = _y + 2 * _height / 3;
  okButton._width = _width / 3;
  okButton._height = _height / 3;

  //draw buttons
  paintButton(LeftArrowButton);
  paintButton(RightArrowButton);
  paintButton(okButton);
  drawCurrentOption();

}

//taken from stack overflow https://stackoverflow.com/questions/9072320/split-string-into-string-array
String SelectionWindow::getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
