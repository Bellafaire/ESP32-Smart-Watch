/********************************************************************
                        Drawable manager
 ********************************************************************/

// variables for swiping
point swipe_start;
point swipe_end;
int last_touch_state = 1;

int current_drawables = 0;
Drawable *drawableItems[MAX_DRAWABLES];
void (*swipeCallbacks[4])();

void registerDrawable(Drawable *visual)
{
    if (current_drawables < MAX_DRAWABLES)
    {
        drawableItems[current_drawables] = visual;
        current_drawables++;
    }
    else
    {
        printDebug("Drawables error: number of drawables not supported!!!!");
    }
}

void setSwipeAction(int dir, void (*cb)())
{
    swipeCallbacks[dir] = cb;
    printDebug("Registered callback for swipe direction " + String(dir));
}

void clearSwipeActions()
{
    for (int a = 0; a < 4; a++)
        swipeCallbacks[a] = nullptr;
}

void drawFrame()
{
#ifndef USE_TOUCH_HANDLING_TASK
    checkTouch();
#endif
    for (int a = 0; a < current_drawables; a++)
    {
        drawableItems[a]->draw();
    }
    tft.drawRGBBitmap(0, 0, frameBuffer->getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);
}

void clearDrawables()
{
    current_drawables = 0;
}

void checkTouch()
{
    point p = readTouch();

    if (p.x != -1 && p.y != -1)
        printDebug("Touch Input - x:" + String(p.x) + " y:" + String(p.y));

    if (last_touch_state && !digitalRead(TOUCH_IRQ))
    {
        for (int a = current_drawables - 1; a > 0; a--)
        {
            if (drawableItems[a]->isTouched(p.x, p.y))
                break;
        }

        swipe_start = p;
    }
    else if (digitalRead(TOUCH_IRQ) && !last_touch_state)
    {
        // now we interpret the swipe

        int swipe_dir = -1;
        point swipe;
        swipe.x = (swipe_end.x - swipe_start.x);
        swipe.y = (swipe_end.y - swipe_start.y);

        int length = (int)sqrt(
            pow(swipe.x, 2) + pow(swipe.y, 2));

        boolean valid_swipe = (swipe_end.x != -1) && (swipe_end.y != -1);

        if ((length > SWIPE_DISTANCE_THRESHOLD) && valid_swipe)
        {
            // determine direction
            double theta = atan2(swipe.y, swipe.x) + PI;
            swipe_dir = round(2 * theta / PI) - 1;
            printDebug("Swipe detected in direction " + String(swipe_dir) + " from theta of " + String(theta, 2) + " start (" + String(swipe_start.x) + "," + String(swipe_start.y) + ") End (" + String(swipe_end.x) + "," + String(swipe_end.y) + ")");

            if (swipeCallbacks[swipe_dir] != nullptr && swipe_dir >= 0)
                swipeCallbacks[swipe_dir]();
        }
    }
    else
    {
        // keep saving the swipe. when the screen is no longer touched we want the last value
        // that's what we'll use to calculate the swipe direction.
        swipe_end = p;
    }

    last_touch_state = digitalRead(TOUCH_IRQ);
}

/* Checks a touch event and triggers any required actions,
  this task is called within the touch interrupt defined in HardwareInterface.ino */
void TouchTask(void *pvParameters)
{
    unsigned long touchTaskStart = micros();
    printDebug("TouchTask Triggerred");

    checkTouch();

    printDebug("TouchTask completed in: " + String(micros() - touchTaskStart) + "us");
    xTouch = NULL;
    vTaskDelete(NULL);
}