
// manages drawables as a queue
#define MAX_DRAWABLES 16
int current_drawables = 0;
Drawable *drawableItems[MAX_DRAWABLES];

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
    {
        printDebug("Touch Input - x:" + String(p.x) + " y:" + String(p.y));

        for (int a = current_drawables - 1; a > 0; a--)
        {
            if (drawableItems[a]->isTouched(p.x, p.y))
                break;
        }
    }
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