#include <Adafruit_GFX.h> 
#ifndef DRAWABLE
#define DRAWABLE

class Drawable
{
public:
    Drawable(int x, int y, int width, int height, GFXcanvas16 *buffer_ptr);
    virtual void draw();
    int getx();
    int gety();
    int getWidth();
    int getHeight();

protected:
    int _x;
    int _y;
    int _width;
    int _height;
    GFXcanvas16 *_buffer_ptr;
};
#endif