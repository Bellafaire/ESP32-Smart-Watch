
//manages drawables as a queue
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

/* Checks a touch event and triggers any required actions,
  this task is called within the touch interrupt defined in HardwareInterface.ino */
void TouchTask(void *pvParameters)
{
    unsigned long touchTaskStart = micros();
    printDebug("TouchTask Triggerred");

    point p = readTouch();
    printDebug("x:" + String(p.x) + " y:" + String(p.y));

    printDebug("TouchTask completed in: " + String(micros() - touchTaskStart) + "us");
    xTouch = NULL;
    vTaskDelete(NULL);
}