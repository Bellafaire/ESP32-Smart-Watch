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
  printDebug("TouchTask Triggerred");

  point p = readTouch();
  printDebug("x:" + String(p.x) + " y:" + String(p.y));

  checkAllTouchAreas(p.x, p.y);

  xTouch = NULL;
  vTaskDelete(NULL);
}

/*
  #define MAX_TOUCH_AREAS 25

  struct touchArea{
  int x, y, width, height;
  ((void*)()) action;
  int identifier;
  };

  int touchAreaIdentifierCount = 1; //keeps track of the number of touch areas created
  static struct activeTouchAreas[MAX_TOUCH_AREAS];
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

  return ta.identifier;
}

void initTouchAreas() {
  for (int a = 0; a < MAX_TOUCH_AREAS; a++) {
    struct touchArea t;
    t.identifier = 0;
    activeTouchAreas[a] = t;
  }
}


void deactivateTouchArea(int identifier) {
  for (int a = 0; a < MAX_TOUCH_AREAS; a++) {
    if (activeTouchAreas[a].identifier == identifier) {
      activeTouchAreas[a].identifier = 0;
      printDebug("Deactivating touchArea " + String(activeTouchAreas[a].identifier));
      break;
    }
  }
}

void checkAllTouchAreas(int x, int y) {
  for (int a = 0; a < MAX_TOUCH_AREAS; a++) {
    if (x >=  activeTouchAreas[a].x
        && x <=  activeTouchAreas[a].x + activeTouchAreas[a].width
        && y >=  activeTouchAreas[a].y
        && y <=  activeTouchAreas[a].y +  activeTouchAreas[a].height
        &&  activeTouchAreas[a].identifier) {
      printDebug("Registered touch for touchEvent: " + String(activeTouchAreas[a].identifier));
      //call the action associated with this touch area
      ((void(*)())activeTouchAreas[a].action)();
    }
  }
}
