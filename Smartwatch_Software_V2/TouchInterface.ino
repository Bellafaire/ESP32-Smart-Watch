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

/* Checks a touch event and triggers any required actions,
  this task is called within the touch interrupt defined in HardwareInterface.ino */
void TouchTask(void * pvParameters ) {
  unsigned long touchTaskStart = micros();
  printDebug("TouchTask Triggerred");

  point p = readTouch();
  printDebug("x:" + String(p.x) + " y:" + String(p.y));

  //if you want to see a small circle everywhere you touch uncomment this, its useful sometimes
  //  tft.drawCircle(p.x, p.y, 2, TEXT_COLOR);

  if ( lastTouchAction + TOUCH_ACTION_COOLDOWN < millis()) {
    if (calculatorActive) {
      calculatorTouchHandler(p);
    } else if (useTouchAreas) {
      checkAllTouchAreas(p.x, p.y);
    }
    lastTouchAction = millis();
  }
  printDebug("TouchTask completed in: " + String(micros() - touchTaskStart) + "us, currently running: ");
  xTouch = NULL;
  vTaskDelete(NULL);
}

//pauses touch areas, after call no touch area will operate
void pauseTouchAreas() {
  useTouchAreas = false;
}

//resumes touch areas, after call all active touch areas will
//begin to function again.
void resumeTouchAreas() {
  useTouchAreas = true;
}

/* Creates a touch area which will be recognized when an area is touched
   each area has a designated function which it will trigger when pressed
*/
int createTouchArea(int x, int y, int width, int height, void* action) {
  struct touchArea ta;
  ta.x = x;
  ta.y = y;
  ta.width = width;
  ta.height = height;
  ta.action = action;
  ta.identifier = touchAreaIdentifierCount ++;

  for (int a = 0; a < MAX_TOUCH_AREAS; a++) {
    if (activeTouchAreas[a].identifier == 0) {
      activeTouchAreas[a] = ta;
      break;
    }
  }

  printDebug("Registered touch area: " + String(ta.identifier));

  return ta.identifier;
}

void printActiveTouchAreas() {
  for (int a = 0; a < MAX_TOUCH_AREAS; a++) {
    if (activeTouchAreas[a].identifier != 0) {
      printDebug("Touch Area " + String(activeTouchAreas[a].identifier) + " is Active");
      break;
    }
  }
}

boolean isTouchAreaActive(int id) {
  for (int a = 0; a < MAX_TOUCH_AREAS; a++) {
    if (activeTouchAreas[a].identifier == id) {
      return true;
    }
  }
  return false;
}

//fills touch area array with dummy values
void initTouchAreas() {
  for (int a = 0; a < MAX_TOUCH_AREAS; a++) {
    struct touchArea t;
    t.identifier = 0;
    activeTouchAreas[a] = t;
  }
}

//deactivates a touch area and allows for its place in memory to be reallocated
void deactivateTouchArea(int identifier) {
  for (int a = 0; a < MAX_TOUCH_AREAS; a++) {
    if (activeTouchAreas[a].identifier == identifier) {
      activeTouchAreas[a].identifier = 0;
      printDebug("Deactivating touchArea " + String(activeTouchAreas[a].identifier));
      break;
    }
  }
}

//deactivates all active touch areas
void deactivateAllTouchAreas() {
  for (int a = 0; a < MAX_TOUCH_AREAS; a++) {
    if (activeTouchAreas[a].identifier != 0) {
      printDebug("Deactivating touchArea " + String(activeTouchAreas[a].identifier));
    }
    activeTouchAreas[a].identifier = 0;
  }
}

//checks all touch areas that are currently active and triggers the appropriate function
void checkAllTouchAreas(int x, int y) {
  for (int a = 0; a < MAX_TOUCH_AREAS; a++) {
    if (x >=  activeTouchAreas[a].x
        && x <=  activeTouchAreas[a].x + activeTouchAreas[a].width
        && y >=  activeTouchAreas[a].y
        && y <=  activeTouchAreas[a].y +  activeTouchAreas[a].height
        &&  activeTouchAreas[a].identifier != 0
       ) {
      printDebug("Detected touch for touchEvent: " + String(activeTouchAreas[a].identifier) + " at x:" + String(x) + " y:" + String(y) + " At Index " + String(a) );
      //call the action associated with this touch area
      ((void(*)())activeTouchAreas[a].action)();
    }
  }
}
