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

/* Pages are what the user ultimately engages the most with, each page should have an init() function
    and a normal run function. The code is set up in such a way that for a page to be redrawn the programmer
    only needs to set the void pointer "currentPage" to the current function to be run on the screen. This way
    pages have minimal interaction with eachother and can more-or-less be written independently.

*/


//AnimationCircle:: AnimationCircle(int _x, int _y, int _rotationRadius, int _circleRadius, int _ringColor, int _color, float _speed, int _circleNumber)
//animation circles, I kind of made these awhile ago as a cool visual element but now they're fairly essential to the entire UI design
AnimationCircle circ1 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 20, 2, RING_COLOR, RGB_TO_BGR565(0, 0, 0), 3.5, 2);
AnimationCircle circ2 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 25, 2, RING_COLOR, RGB_TO_BGR565(0, 0, 0), -3, 3);
AnimationCircle circ3 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 31, 2, RING_COLOR, RGB_TO_BGR565(0, 0, 0), 2.5, 4);
AnimationCircle circ4 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 38, 2, RING_COLOR, RGB_TO_BGR565(0, 0, 0), -2, 5);
AnimationCircle circ5 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 45, 2, RING_COLOR, RGB_TO_BGR565(0, 0, 255), 1.5, 6);

RoundButton homeButton, settingButton, notificationsButton, homeMediaButton, homeNextMediaButton, homePreviousMediaButton, homePauseMediaButton, upButton, downButton, okButton, leftButton, calendarButton, calculatorButton;


/********************************************************************
                              Time Only
 ********************************************************************/
void initTimeOnly() {
  //remove all previous touch areas, this should be done in every init function
  deactivateAllTouchAreas();
  createTouchArea(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (void*) switchToHome);
  currentPage = (void*)timeOnly;
}

void timeOnly() {
  frameBuffer->drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);
  drawTime(12, 30, 2, 0xFFFF, 1);
  drawDateCentered(50, 1);
}



/********************************************************************
                                 HOME
 ********************************************************************/
int homeTouchArea;
boolean songCheck = false;

void switchToHome() {
  currentPage = (void*)initHome;
}

//initalizes the home page
void initHome() {
  printDebug("initializing home");

  songCheck = false;

  //re-init animation circles
  circ1 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 15, 3, RING_COLOR, RING_COLOR, 3.5, 2);
  circ2 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 22, 2, RING_COLOR, RING_COLOR, -3, 3);
  circ3 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 28, 2, RING_COLOR, RING_COLOR, 2.5, 4);
  circ4 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 34, 2, RING_COLOR, RING_COLOR, -2, 5);
  circ5 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 40, 3, RING_COLOR, 0xcbff00, 1.5, 6);

  //remove all previous touch areas, this should be done in every init function
  deactivateAllTouchAreas();

  //create touch area for going to the navigation page
  homeTouchArea = createTouchArea(SCREEN_WIDTH - 50, SCREEN_HEIGHT - 50, 100, 100, (void*) transitionToNav);
  homeMediaButton = RoundButton(16, SCREEN_HEIGHT - 16, 16, MEDIA_PLAY_ICON, (void*)switchToHomeMedia);
  homeMediaButton.deactivate();

  //set current page to home, initialization is complete so we're good to start drawing the normal home page
  currentPage = (void*)home;
}

//we don't want to spam the phone if we can help it, polling is easier so lets check every second
long lastSongCheck = 0;
#define SONG_CHECK_INTERVAL 1000 //how many miliseconds between checking the current song

//draws the home screen
void home() {

  //draw the background image declared in Declarations.h
  frameBuffer->drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

  //animate all the circles (the class will handle all of that for us as long as we call it)
  circ5.animateAndDraw(frameBuffer);
  circ4.animateAndDraw(frameBuffer);
  circ3.animateAndDraw(frameBuffer);
  circ2.animateAndDraw(frameBuffer);
  circ1.animateAndDraw(frameBuffer);

  //draw time and date (final parameter indicates shadow distance)
  drawTime(3, 5, 2, 0xFFFF, 1);
  drawDateCentered(20, 1);

  //  draw battery percentage with a slight shadow to make it pop from the background
  frameBuffer->setTextColor(0x0000);
  frameBuffer->setCursor(SCREEN_WIDTH - 6 * 4, 5 );
  frameBuffer->println(String(batteryPercentage) + "%");

  if (isCharging()) {
    frameBuffer->setTextColor(RGB_TO_BGR565(0, 255, 0));
  }
  else {
    frameBuffer->setTextColor(0xFFFF);
  }
  frameBuffer->setCursor(SCREEN_WIDTH - 6 * 4 - 1, 5 - 1);
  frameBuffer->println(String(batteryPercentage) + "%");

  //draw the notification data to the screen
  drawNotifications(notificationData, 0, 35, 0xFFFF);

  //display connection status on the innermost circle,
  if (connected) {
    circ1.setColor(RGB_TO_BGR565(0, 255, 0));
  }  else {
    circ1.setColor(RGB_TO_BGR565(255, 0, 0));
  }

  if (spotifyIsPlaying()) {
    homeMediaButton.activate();

  }

  //draw the media play button (if the button is deactivated then the button will not be drawn)
  homeMediaButton.draw(frameBuffer);
}




/* Quick and dirty animation to transition to the navigation page.
  we essentially expand one of the circles until it fills the entire screen
*/
int navTransitionState = 0;
void homeTransitionToNav() {
  if (navTransitionState < 8) {
    home();

    circ5.animateAndDraw(frameBuffer);
    frameBuffer->drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  } else if (navTransitionState < 16) {
    int a = navTransitionState - 8;
    home();

    circ5.setRadius(45 + a * (SCREEN_WIDTH / 8));
    circ5.animateAndDraw(frameBuffer);
    frameBuffer->drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  } else {
    currentPage = (void*)initNavigation;
  }
  navTransitionState++;
}

//initalizes the navigation transition from the homescreen
void transitionToNav() {
  navTransitionState = 0;
  currentPage = (void*)homeTransitionToNav;
}

void switchToHomeMedia() {
  currentPage = (void*)initHomeMedia;
}

/* Home Media
   technically just an extension of home activated by tapping the play icon
*/
void initHomeMedia() {
  //set the page to homeMedia
  currentPage = (void*)homeMedia;

  //clear all touch areas since we don't need them while the media controller is active
  deactivateAllTouchAreas();

  //create all the buttons we need for the media control
  homeMediaButton = RoundButton(16, SCREEN_HEIGHT - 16, 16, MEDIA_PLAY_ICON, (void*)playMusic);
  homeNextMediaButton = RoundButton(56, SCREEN_HEIGHT - 56, 16, MEDIA_NEXT_ICON, (void*)nextSong);
  homePreviousMediaButton = RoundButton(16, SCREEN_HEIGHT - 66, 16, MEDIA_LAST_ICON, (void*)lastSong);
  homePauseMediaButton = RoundButton(66, SCREEN_HEIGHT - 16, 16, MEDIA_PAUSE_ICON, (void*)pauseMusic);

  updateCurrentSong();

  //create new touch area which is used to close the media controls, this is placed directly on top of the circles of the homescreen
  homeTouchArea = createTouchArea(SCREEN_WIDTH - 50, SCREEN_HEIGHT - 50, 100, 100, (void*) switchToHome);
}

//draw the home Media controls, in this case it basically just adds to the home screen so we can basically just draw on top of the home function
void homeMedia() {
  updateCurrentSong();

  home();

  frameBuffer->fillRect(8, 8, SCREEN_WIDTH - 16, 33, BACKGROUND_COLOR);
  frameBuffer->drawRect(8, 8, SCREEN_WIDTH - 16, 33, INTERFACE_COLOR);

  printInsideOf(10, 10, SCREEN_WIDTH - 10, 45, currentSong, frameBuffer);

  homeNextMediaButton.draw(frameBuffer);
  homePreviousMediaButton.draw(frameBuffer);
  homePauseMediaButton.draw(frameBuffer);
  homeMediaButton.draw(frameBuffer);
}



/********************************************************************
                              Navigation
 ********************************************************************/

//initalizes the navigation page with the appropriate buttons
void initNavigation() {
  deactivateAllTouchAreas();

  //create homeButton
  homeButton = RoundButton(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 16, HOME_ICON, (void*)switchToHome);
  settingButton = RoundButton(25, 35, 16, SETTINGS_ICON, (void*)switchToSettings);
  notificationsButton = RoundButton(75, 35, 16, NOTIFICATIONS_ICON, (void*)switchToNotifications);
  calendarButton = RoundButton(125, 35, 16, CALENDAR_ICON, (void*)switchToCalendar);
  calculatorButton = RoundButton(25, 75, 16, CALCULATOR_ICON, (void*)switchToCalculator);

  currentPage = (void*)navigation;
}

//normal animations and stuff when navigation is active.
void navigation() {
  //draw the background image declared in Declarations.h
  frameBuffer->drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

  frameBuffer->setCursor(0, 0);
  frameBuffer->println("Navigation");

  //draw the animation circles
  circ4.animateAndDraw(frameBuffer);
  circ3.animateAndDraw(frameBuffer);
  circ2.animateAndDraw(frameBuffer);
  circ1.animateAndDraw(frameBuffer);

  homeButton.draw(frameBuffer);
  settingButton.draw(frameBuffer);
  notificationsButton.draw(frameBuffer);
  calendarButton.draw(frameBuffer);
  calculatorButton.draw(frameBuffer);
}

/********************************************************************
                              Settings
 ********************************************************************/
void switchToSettings() {
  currentPage = (void*)initSettings;
}

void initSettings() {

  //this entire function needs to more-or-less run on its own without the framework around it
  pauseTouchAreas();
  drawInLoop = false;

  printDebug("Entering Settings");
  frameBuffer->setCursor(0, 100);

  //create selection window of the required size to display on screen.
  SelectionWindow w2 = SelectionWindow(0, 14, 160, 100);

  //options for the selection window
  w2.addOption("DayLights Savings");
  w2.addOption("Accelerometer");
  w2.addOption("Screen Brightness");
  w2.addOption("Calibrate Touch");

  int option = w2.focus() - 1;

  switch (option) {
    //configure daylights savings
    case 0:
      {
        SelectionWindow w3 = SelectionWindow(0, 14, 160, 100);
        w3.addOption("no");
        w3.addOption("yes");

        byte ret = (byte)(w3.focus());
        if (ret != 0) {
          setDataField(ret - 1, DAYLIGHT_SAVINGS);
        }
      }
      currentPage = (void*)initNavigation;
      break;
    case 1:
      {
        SelectionWindow w3 = SelectionWindow(0, 14, 160, 100);
        w3.addOption("Touch Only");
        w3.addOption("Wake");
        w3.addOption("Display Time");
        byte ret = (byte)(w3.focus());
        if (ret != 0) {
          setDataField(ret - 1, WAKEUP_TYPE);
        }
      }
      currentPage = (void*)initNavigation;
      break;
    case 2:
      {
        SelectionWindow w3 = SelectionWindow(0, 14, 160, 100);
        for (int a = 10; a <= 100; a += 10) {
          w3.addOption(String(a) + "%");
        }

        byte ret = (byte)(w3.focus());
        if (ret != 0) {
          setDataField(ret * 10, SCREEN_BRIGHTNESS);
        }
      }
      currentPage = (void*)initNavigation;
      break;
    case 3:
      switchToCalibration();
      break;
    default:
      currentPage = (void*)initNavigation;
      break;
  }

  //reload settings from eeprom
  loadEEPROMSettings();

  drawInLoop = true;
  printDebug("Exiting Settings");
  resumeTouchAreas();
}


/********************************************************************
                            Notifications
 ********************************************************************/
int notificationScrollPosition = 0;

void switchToNotifications() {
  currentPage = (void*)initNotifications;
}

void initNotifications() {
  //draw the background image declared in Declarations.h
  frameBuffer->drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

  notificationScrollPosition = 0;

  deactivateAllTouchAreas();

  upButton = RoundButton(SCREEN_WIDTH - 20, 15, 14, UP_ARROW_ICON, (void*)lastNotification);
  okButton = RoundButton(SCREEN_WIDTH - 20, 45, 14, CHECK_MARK_ICON, (void*)showNotification);
  downButton = RoundButton(SCREEN_WIDTH - 20, 75 , 14, DOWN_ARROW_ICON, (void*)nextNotification);
  homeButton = RoundButton(SCREEN_WIDTH - 20, 105, 14, HOME_ICON, (void*)switchToHome);

  leftButton = RoundButton(15, 15, 14, LEFT_ARROW_ICON, (void*)switchToNotifications);
  leftButton.deactivate();

  currentPage = (void*)notifications;
}

void showNotification() {

  currentPage = (void*)notificationDisplay;

  deactivateAllTouchAreas();

  //activate the return button
  leftButton.activate();

}

void notificationDisplay() {
  //draw the background image declared in Declarations.h
  frameBuffer->drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

  String line = getValue(notificationData, '\n', notificationScrollPosition);

  frameBuffer->setTextColor(INTERFACE_COLOR);

  //draw title field
  frameBuffer->setCursor(35, 3);
  frameBuffer->println(getValue(getValue(line, FIELD_SEPARATOR, 0), ',', 0));

  //draw subtitle field
  frameBuffer->setCursor(35, 3 + WINDOW_CHARACTER_HEIGHT);
  frameBuffer->println(getValue(getValue(line, FIELD_SEPARATOR, 0), ',', 1));

  //draw description
  frameBuffer->setTextColor(GRAYED);
  frameBuffer->setCursor(0, 35);
  frameBuffer->println("Description");
  frameBuffer->setTextColor(INTERFACE_COLOR);
  frameBuffer->println(getValue(line, FIELD_SEPARATOR, 5));

  //draw return button
  leftButton.draw(frameBuffer);
}

void nextNotification() {
  if (notificationScrollPosition < getNumberOfLines(notificationData)) {
    notificationScrollPosition++;
  }
}

void lastNotification() {
  if (notificationScrollPosition > 0) {
    notificationScrollPosition--;
  }
}


void notifications() {
  //draw the background image declared in Declarations.h
  frameBuffer->drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

  //count lines
  int lineCount = getNumberOfLines(notificationData);


  for (int a = 0; a < lineCount; a++) {
    //choose position for the current line
    frameBuffer->setCursor(0, a * WINDOW_CHARACTER_HEIGHT);

    //if the current item is selected then draw a white box behind it and make the text black
    //if the current item is not selected then treat it normally
    if (a == notificationScrollPosition) {
      frameBuffer->fillRect(0, a * WINDOW_CHARACTER_HEIGHT, SCREEN_WIDTH, WINDOW_CHARACTER_HEIGHT , INTERFACE_COLOR);
      frameBuffer->setTextColor(BACKGROUND_COLOR);
    } else {
      frameBuffer->fillRect(0, a * WINDOW_CHARACTER_HEIGHT,  SCREEN_WIDTH, WINDOW_CHARACTER_HEIGHT, BACKGROUND_COLOR);
      frameBuffer->setTextColor(INTERFACE_COLOR);
    }

    //whole lot of string operations, we only really want the very first item.
    String line = getValue(notificationData, '\n', a);
    frameBuffer->println(getValue(getValue(line, FIELD_SEPARATOR, 0), ',', 0));
  }

  okButton.draw(frameBuffer);
  upButton.draw(frameBuffer);
  homeButton.draw(frameBuffer);
  downButton.draw(frameBuffer);
}

/********************************************************************
                              CALENDAR
 ********************************************************************/

String calendarData = "";
int calendarScrollPosition = 0;
boolean calendarDataObtained = false;

void switchToCalendar() {
  currentPage = (void*)initCalendar;
}

void initCalendar() {
  calendarScrollPosition = 0;
  frameBuffer->fillScreen(BACKGROUND_COLOR);
  calendarDataObtained = false;

  deactivateAllTouchAreas();

  //buttons
  upButton = RoundButton(SCREEN_WIDTH - 20, 15, 14, UP_ARROW_ICON, (void*)lastCalendar);
  downButton = RoundButton(SCREEN_WIDTH - 20, 75 , 14, DOWN_ARROW_ICON, (void*)nextCalendar);
  homeButton = RoundButton(SCREEN_WIDTH - 20, 105, 14, HOME_ICON, (void*)switchToHome);


  boolean success = sendBLE("/calendar", &calendarData, false);
  if (success)
    calendarDataObtained = true;

  //onto the next page
  currentPage = (void*) calendar;
}

void nextCalendar() {
  if (calendarScrollPosition < getNumberOfLines(calendarData) - 1) {
    calendarScrollPosition++;
    delay(250);
  }
}

void lastCalendar() {
  if (calendarScrollPosition > 0) {
    calendarScrollPosition--;
    delay(250);
  }
}

//draws calendar information from smartphone
void calendar() {
  if (!calendarDataObtained) {
    boolean success = sendBLE("/calendar", &calendarData, false);
    if (success)
      calendarDataObtained = true;
  }
  //draw the background image declared in Declarations.h
  frameBuffer->drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);
  int firstEventStart = 0;

  //count lines
  int lineCount = getNumberOfLines(calendarData);

  for (int a = 0; a < lineCount; a++) {

    //parsing data out of the calendar string
    String line = getValue(calendarData, '\n', a);
    struct calendarEvent ce = parseCalendarEvent(line);

    //since "technically" the day starts at 0:00 we want the event viewer on the left
    //to start at the first event and scale everything appropriately, so we record the
    //start time
    if (a == 0) {
      firstEventStart = ce.timeStart;
    }

    //start position of the left hand nav and the height
    //These equations are a mess but they work...
    int startPos = ((ce.timeStart - firstEventStart) / ((24.0 - (firstEventStart / 60.0)) * 60.0)) * SCREEN_HEIGHT;
    int height = ((ce.timeEnd  - ce.timeStart ) / ((24.0 - (firstEventStart / 60.0)) * 60.0)) * SCREEN_HEIGHT ;

    //if the current calendar item is the selected one
    if (a == calendarScrollPosition) {

      //make it's navigation box stick out a bit more and turn slightly green
      frameBuffer->fillRoundRect(0, startPos, 15, height, 3, 0xFF00);

      //draw the title
      frameBuffer->setCursor(20, 3);
      frameBuffer->println(ce.title);

      //draw the time
      frameBuffer->setCursor(20, 15);
      frameBuffer->println(ce.time);

      //draw the description
      frameBuffer->setTextColor(GRAYED);
      frameBuffer->setCursor(20, 25);
      frameBuffer->print("Description");
      frameBuffer->setTextColor(INTERFACE_COLOR);
      printInsideOf(20, 35, SCREEN_WIDTH - 40, SCREEN_HEIGHT, ce.description, frameBuffer);

    } else {
      //if this event is not the current event then just draw the nav item
      // so the user can see that there is an event.
      frameBuffer->fillRoundRect(0, startPos, 10, height, 3, 0xF800);
    }

  }

  //draw buttons.
  upButton.draw(frameBuffer);
  downButton.draw(frameBuffer);
  homeButton.draw(frameBuffer);
}


/********************************************************************
                              Calculator
 ********************************************************************/
#define CALCULATOR_BUTTON_PADDING 4
#define CALCULATOR_BUTTON_COLUMNS 5
#define CALCULATOR_BUTTON_ROWS 4

#define CALCULATION_BOX_HEIGHT 16

boolean calculatorButtonsGenerated = false;
boolean enteringFirstOperand = true;
String firstOperand = "";
String secondOperand = "";
String calculatorOperation = "";

void switchToCalculator() {
  printDebug("Switching to calculator");
  currentPage = (void*) initCalculator;
}

void initCalculator() {
  //this entire page needs to more-or-less run on its own without the framework around it
  deactivateAllTouchAreas();
  calculatorActive = true;
  currentPage = (void*) calculator;
}

void calculator() {
  drawCalculator();
  vTaskDelay(100);
}


String calculatorButtonLabels[CALCULATOR_BUTTON_COLUMNS * CALCULATOR_BUTTON_ROWS] = {
  "7", "8", "9", "/", "X",
  "4", "5", "6", "*", "",
  "1", "2", "3", "-", "C",
  "0", ".", "", "+", "="
};

onscreenButton calculatorButtons[CALCULATOR_BUTTON_COLUMNS * CALCULATOR_BUTTON_ROWS] =
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
        if (!calculatorButtonLabels[(b * CALCULATOR_BUTTON_COLUMNS) + a].equals("")) {
          calculatorButtons[(b * CALCULATOR_BUTTON_COLUMNS) + a]._x = xSpacePerButton * a ;
          calculatorButtons[(b * CALCULATOR_BUTTON_COLUMNS) + a]._y = ySpacePerButton * b + CALCULATION_BOX_HEIGHT + CALCULATOR_BUTTON_PADDING;
          calculatorButtons[(b * CALCULATOR_BUTTON_COLUMNS) + a]._width =  xSpacePerButton - CALCULATOR_BUTTON_PADDING;
          calculatorButtons[(b * CALCULATOR_BUTTON_COLUMNS) + a]._height = ySpacePerButton - CALCULATOR_BUTTON_PADDING;
          calculatorButtons[(b * CALCULATOR_BUTTON_COLUMNS) + a]._text = calculatorButtonLabels[(b * CALCULATOR_BUTTON_COLUMNS) + a];
        }
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
}

void calculatorTouchHandler(struct point p) {
  printDebug("Checking calculator buttons for: x-" + String(p.x) + " y-" + String(p.y));
  for (int a = 0;  a < CALCULATOR_BUTTON_COLUMNS * CALCULATOR_BUTTON_ROWS; a++) {
    if (checkButtonPress(calculatorButtons[a], p.x, p.y)) {
      printDebug("Calculator, User pressed: " + calculatorButtonLabels[a]);
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
            calculatorActive = false;
            switchToHome();
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

/********************************************************************
                              CALIBRATION
      Allows the user to automatically calibrate their touch screen
      by touching some points. This helps with scaling to different
      touch screens and accounts for variations between any two resistive
      touch screens.
 ********************************************************************/
//calibration state controls which dot is active for the user to press
//and determines when it's possible to save the resulting calibration data
int calibrationState = 0;
point calibrationPoints[4];
unsigned long calibrationCompleteTime;

//switches current page
void switchToCalibration() {
  printDebug("Switching to Calibration");
  currentPage = (void*) initCalibration;
}

//initalizes the calibration screen
void initCalibration() {
  deactivateAllTouchAreas();
  calibrationState = 0;

  //create touch area that covers all possible touch values.
  createTouchArea(-32768, -32768, 65535, 65535, (void*) calibrationAction);

  currentPage = (void*) calibration;
}

void calibrationAction() {
  point p = readTouchRaw();
  printDebug("Calibration Values xraw:" + String(p.x) + " yraw:" + String(p.y));
  if (calibrationState < 4) {
    calibrationPoints[calibrationState] = p;

    printDebug("Raw Data Points:");
    for (int a = 0; a < 4; a++) {
      printDebug(String(a) + " x:" + String(calibrationPoints[a].x) + " y:" + String(calibrationPoints[a].y));
    }
    calibrationState++;
  } else {
    //anything we need to handle when calibration is done
  }
}


//calibrate at 4 points
void calibration() {
  if (calibrationState <= 4) {
    frameBuffer-> fillScreen(0xFFFF);
    frameBuffer->setTextColor(0x0000);
    frameBuffer->setCursor(10, SCREEN_HEIGHT / 2 - 3);
    frameBuffer->println("TouchScreen Calibration");

    //depending on the state we place the "calibration touch point" at different locations on the screen
    //the state determines where the point is and stores its data in the appropriate location when touched
    switch (calibrationState) {
      case 0:
        frameBuffer->drawCircle(10, 10, 5, RGB_TO_BGR565(255, 0, 0));
        frameBuffer->drawCircle(10, 10, 1, RGB_TO_BGR565(255, 0, 0));
        break;
      case 1:
        frameBuffer->drawCircle(SCREEN_WIDTH - 10, 10, 5, RGB_TO_BGR565(255, 0, 0));
        frameBuffer->drawCircle(SCREEN_WIDTH - 10, 10, 1, RGB_TO_BGR565(255, 0, 0));
        break;
      case 2:
        frameBuffer->drawCircle(10, SCREEN_HEIGHT - 10, 5, RGB_TO_BGR565(255, 0, 0));
        frameBuffer->drawCircle(10, SCREEN_HEIGHT - 10, 1, RGB_TO_BGR565(255, 0, 0));
        break;
      case 3:
        frameBuffer->drawCircle(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10, 5, RGB_TO_BGR565(255, 0, 0));
        frameBuffer->drawCircle(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10, 1, RGB_TO_BGR565(255, 0, 0));
        break;
      case 4:
        //do calibration calculations, it's basically just a direct mapping
        SETTING_X_MIN = map(0, 10, SCREEN_WIDTH - 10, (calibrationPoints[0].x + calibrationPoints[2].x) / 2, (calibrationPoints[1].x + calibrationPoints[3].x) / 2  );
        SETTING_X_MAX = map(SCREEN_WIDTH, 10, SCREEN_WIDTH - 10, (calibrationPoints[0].x + calibrationPoints[2].x) / 2, (calibrationPoints[1].x + calibrationPoints[3].x) / 2 );

        SETTING_Y_MIN = map(0,  10, SCREEN_HEIGHT - 10, (calibrationPoints[0].y + calibrationPoints[1].y) / 2, (calibrationPoints[2].y + calibrationPoints[3].y) / 2);
        SETTING_Y_MAX = map(SCREEN_HEIGHT, 10, SCREEN_HEIGHT - 10, (calibrationPoints[0].y + calibrationPoints[1].y) / 2, (calibrationPoints[2].y + calibrationPoints[3].y) / 2 );

        //print the debug values
        printDebug("Calibration Values Obtained: ");
        printDebug("   xmin:" + String(SETTING_X_MIN));
        printDebug("   xmax:" + String(SETTING_X_MAX));
        printDebug("   ymin:" + String(SETTING_Y_MIN));
        printDebug("   ymax:" + String(SETTING_Y_MAX));



        deactivateAllTouchAreas();
        //create a home button that can be pressed to exit the calibration screen
        homeButton = RoundButton(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 16, CHECK_MARK_ICON, (void*)confirmCalibration);
        calibrationCompleteTime = millis();
        calibrationState++;
        break;
    }
  } else {
    frameBuffer-> fillScreen(0xFFFF);

    //calibration is complete. let the user draw on the screen to establish that the calibration is correct
    //the user has 10 seconds to confirm the calibration before calibration restarts.
    homeButton.draw(frameBuffer);

    frameBuffer->setTextColor(0x0000);
    frameBuffer->setCursor(10, SCREEN_HEIGHT / 2 - 3);
    frameBuffer->println("Confirm Calibration");
    frameBuffer->println("  Restarting Calibration \n  in " + String(10 - (millis() - calibrationCompleteTime) / 1000));

    if ((millis() - calibrationCompleteTime) > 10000) {
      currentPage = (void*) initCalibration;
    }

    point p = readTouch();
    frameBuffer->fillCircle(p.x, p.y, 3, RGB_TO_BGR565(255, 0, 0));
  }
}

//only when the user taps the check mark will the device save the calibration setting.
//the goal here is to prevent a case where someone messed up calibration and can no longer use their device
//properly.
void confirmCalibration() {
  //load into EEPROM
  setDataField(SETTING_X_MIN >> 8, X_MIN);
  setDataField(SETTING_X_MIN & 0x00FF , X_MIN + 1);

  setDataField(SETTING_X_MAX >> 8, X_MAX);
  setDataField(SETTING_X_MAX & 0x00FF , X_MAX + 1);

  setDataField(SETTING_Y_MIN >> 8, Y_MIN);
  setDataField(SETTING_Y_MIN & 0x00FF , Y_MIN + 1);

  setDataField(SETTING_Y_MAX >> 8, Y_MAX);
  setDataField(SETTING_Y_MAX & 0x00FF , Y_MAX + 1);

  //now that the data is set in the EEPROM reload it so that it's in the normal memory
  loadEEPROMSettings();
  switchToHome();
}
