#include "drawable.h"

Drawable::Drawable(int x, int y, int width, int height, GFXcanvas16 *buffer_ptr)
{
    _x = x;
    _y = y;
    _width = width;
    _height = height;
    _buffer_ptr = buffer_ptr;
}
void Drawable::draw() { _buffer_ptr->fillRect(_x, _y, _width, _height, 0xFFFF); _buffer_ptr->drawRect(_x, _y, _width, _height, 0x0000); }
int Drawable::getx() { return _x; }
int Drawable::gety() { return _y; }
int Drawable::getWidth() { return _width; }
int Drawable::getHeight() { return _height; }
