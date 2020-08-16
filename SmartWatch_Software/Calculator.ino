#define CALCULATOR_BUTTON_PADDING 4
#define CALCULATOR_BUTTON_COLUMNS 5
#define CALCULATOR_BUTTON_ROWS 4

#define CALCULATION_BOX_HEIGHT 16

boolean calculatorButtonsGenerated = false;
boolean enteringFirstOperand = true;
String firstOperand = "";
String secondOperand = "";
String calculatorOperation = "";

String calculatorButtonLabels[CALCULATOR_BUTTON_COLUMNS * CALCULATOR_BUTTON_ROWS] = {
  "7", "8", "9", "/", "X",
  "4", "5", "6", "*", "",
  "1", "2", "3", "-", "C",
  "0", ".", "", "+", "="
};

button calculatorButtons[CALCULATOR_BUTTON_COLUMNS * CALCULATOR_BUTTON_ROWS] =
{
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "},
  {0, 0, 0, 0, INTERFACE_COLOR, BACKGROUND_COLOR, " "}
};

//struct onscreenButton
//{
//  int _x, _y, _width, _height, _color, _backgroundColor;
//  String _text;
//};

void drawCalculator() {
  //if we haven't generated the buttons yet then we should do that now, generating the buttons
  //saves us a lot of trouble fiddling with exact placements and allows for the code to work
  //on different screen sizes
  if (!calculatorButtonsGenerated) {
    int xSpacePerButton = (SCREEN_WIDTH ) / CALCULATOR_BUTTON_COLUMNS;
    int ySpacePerButton = (SCREEN_HEIGHT  - CALCULATION_BOX_HEIGHT - CALCULATOR_BUTTON_PADDING  ) / CALCULATOR_BUTTON_ROWS;

    for (int a = 0; a < CALCULATOR_BUTTON_COLUMNS; a++) {
      for (int b = 0; b < CALCULATOR_BUTTON_ROWS; b++) {
        //        calculatorButtons[(a * CALCULATOR_BUTTON_COLUMNS) + b] = {xSpacePerButton * a, ySpacePerButton * b + CALCULATION_BOX_HEIGHT, xSpacePerButton, ySpacePerButton, INTERFACE_COLOR, BACKGROUND_COLOR, calculatorButtonLabels[(a * CALCULATOR_BUTTON_COLUMNS) + b]};
        calculatorButtons[(b * CALCULATOR_BUTTON_COLUMNS) + a]._x = xSpacePerButton * a ;
        calculatorButtons[(b * CALCULATOR_BUTTON_COLUMNS) + a]._y = ySpacePerButton * b + CALCULATION_BOX_HEIGHT + CALCULATOR_BUTTON_PADDING;
        calculatorButtons[(b * CALCULATOR_BUTTON_COLUMNS) + a]._width =  xSpacePerButton - CALCULATOR_BUTTON_PADDING;
        calculatorButtons[(b * CALCULATOR_BUTTON_COLUMNS) + a]._height = ySpacePerButton - CALCULATOR_BUTTON_PADDING;
        calculatorButtons[(b * CALCULATOR_BUTTON_COLUMNS) + a]._text = calculatorButtonLabels[(b * CALCULATOR_BUTTON_COLUMNS) + a];
      }
    }
    calculatorButtonsGenerated = true;
  }

  frameBuffer -> drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);
  frameBuffer-> fillRect(0, 0, SCREEN_WIDTH - CALCULATOR_BUTTON_PADDING, CALCULATION_BOX_HEIGHT, BACKGROUND_COLOR);
  frameBuffer-> drawRect(0, 0, SCREEN_WIDTH - CALCULATOR_BUTTON_PADDING, CALCULATION_BOX_HEIGHT, INTERFACE_COLOR);

  String output = firstOperand + " " + calculatorOperation + " " + secondOperand;

  frameBuffer->setCursor(SCREEN_WIDTH - output.length() * 6 - CALCULATOR_BUTTON_PADDING, (CALCULATION_BOX_HEIGHT - 6) / 2);
  frameBuffer->setTextSize(0);
  frameBuffer->print(output);

  for (int a = 0; a < CALCULATOR_BUTTON_COLUMNS * CALCULATOR_BUTTON_ROWS; a++) {
    //don't draw blank buttons, these are open slots that aren't being used
    paintButtonFull(calculatorButtons[a]);
  }

  tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
}

void calculatorTouchHandler(struct point p) {
  for (int a = 0;  a < CALCULATOR_BUTTON_COLUMNS * CALCULATOR_BUTTON_ROWS; a++) {
    if (checkButtonPress(calculatorButtons[a], p.xPos, p.yPos)) {
      //check whether or not the button pressed was an operation or a number, if it's an operation
      //we have other things that need to be done
      if (calculatorButtonLabels[a].equals("C") ||
          calculatorButtonLabels[a].equals("/") ||
          calculatorButtonLabels[a].equals("*") ||
          calculatorButtonLabels[a].equals("-") ||
          calculatorButtonLabels[a].equals("+") ||
          calculatorButtonLabels[a].equals("X") ||
          calculatorButtonLabels[a].equals("=")
         ) {
        switch (calculatorButtonLabels[a][0]) {
          case 'C':
            //clear the string data
            firstOperand = "";
            secondOperand = "";
            calculatorOperation = "";
            enteringFirstOperand = true;
            break;
          case 'X':
            currentPage = HOME;
            break;
          case '=':
            calculatorCalculate();
            break;
          default:
            calculatorOperation = calculatorButtonLabels[a];
            enteringFirstOperand = false;
            break;
        }
      } else {
        if (enteringFirstOperand) {
          firstOperand = firstOperand + calculatorButtonLabels[a];
        } else {
          secondOperand = secondOperand + calculatorButtonLabels[a];
        }
      }
    }
  }
}

void calculatorCalculate() {
  double first = firstOperand.toDouble();
  double second = secondOperand.toDouble();
  double result = 0;
  boolean error = false;
  switch (calculatorOperation[0]) {
    case '+':
      result = first + second;
      break;
    case '-':
      result = first - second;
      break;
    case '*':
      result = first * second;
      break;
    case '/':
      if (second != 0) {
        result = first / second;
      } else {
        error = true;
      }
      break;
    default:
      error = true;
      break;
  }

  if (error) {
    firstOperand = "NaN";
  } else {
    firstOperand = String(result, 4);
  }
  secondOperand = "";
  calculatorOperation = "";
}
