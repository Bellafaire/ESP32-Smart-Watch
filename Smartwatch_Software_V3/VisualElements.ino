/****************************************************
 *                   Background
 ****************************************************/
class Background : public Drawable
{
public:
    Background(const uint16_t *img, GFXcanvas16 *buffer_ptr)
        : Drawable(0, 0, 160, 128, buffer_ptr, "Background")
    {
        _img = img;
    }

    void draw()
    {
        _buffer_ptr->drawRGBBitmap(0, 0, _img, _width, _height);
    }

private:
    const uint16_t *_img;
};

/****************************************************
 *                    Scrollbox
 ****************************************************/
class Scrollbox : public Drawable
{
public:
    Scrollbox(int x, int y, int width, int height, GFXcanvas16 *buffer_ptr)
        : Drawable(x, y, width, height, buffer_ptr, "Scrollbox")
    {
        setTouchable(true);
    }

    void setString(String str)
    {
        _str = str;
    }

    void onTouch(int x, int y)
    {
        // printDebug("onTouch called in " + toString());
        _touched = (x >= _x) && (y >= _y) && (x <= _x + _width) && (y <= _y + _height) && _touchable;
        if (_touched)
        {
            if (y > (_y + _height / 2))
                _scroll += 2;
            else
                _scroll -= 2;
            // printDebug("Scroll: " + String(_scroll));
        }
    }

    void draw()
    {
        int characters_per_line = ((_width - 2 * _padding) / _character_spacing);
        int xpos = _x + _padding;
        int ypos = _y;

        int length = _str.length();
        int character_position = (characters_per_line) * (_scroll / _line_spacing);

        if (character_position < 0)
            character_position = 0;

        while (character_position < length && ypos < _y + _height)
        {

            _buffer_ptr->setCursor(xpos, ypos);
            _buffer_ptr->print(_str[character_position]);
            xpos += _character_spacing;
            if ((xpos >= (_x + _width) - _padding) || _str[character_position] == '\n')
            {
                xpos = _x + _padding;
                ypos += _line_spacing;
            }
            character_position++;
        }
    }

    void resetScroll()
    {
        _scroll = 0;
    }

private:
    String _str = "";
    const int _padding = 6; // this could be a define but it's better here.
    const int _character_spacing = 6;
    const int _line_spacing = 8;
    int _scroll = 0;
};

/****************************************************
 *               connection status
 ****************************************************/
class ConnStatus : public Drawable
{
public:
    ConnStatus(int x, int y, GFXcanvas16 *buffer_ptr)
        : Drawable(x, y, 10, 4, buffer_ptr, "BatteryIcon")
    {
    }

    void draw()
    {

        if (connected)
            _buffer_ptr->fillRect(_x, _y, _width, _height, RGB_TO_BGR565(0, 255, 0));
        else
            _buffer_ptr->fillRect(_x, _y, _width, _height, RGB_TO_BGR565(255, 0, 0));
        _buffer_ptr->drawRect(_x, _y, _width, _height, RGB_TO_BGR565(0, 0, 0));
    }

private:
};

/****************************************************
 *                     Battery
 ****************************************************/
class BatteryIcon : public Drawable
{
public:
    BatteryIcon(int x, int y, GFXcanvas16 *buffer_ptr)
        : Drawable(x, y, 10, 6, buffer_ptr, "BatteryIcon")
    {
        batteryPercentage = getBatteryPercentage();
    }

    void draw()
    {
        // debug code!!!
        // batteryPercentage--;
        // if (batteryPercentage < 0)
        //     batteryPercentage = 100;

        _buffer_ptr->fillRect(_x + 1, _y, _width - 1, _height, RGB_TO_BGR565(100, 100, 100));
        _buffer_ptr->fillRect(_x, _y + 2, _width, _height - 4, RGB_TO_BGR565(100, 100, 100));

        int batsliver = batteryPercentage * (_width - 3) / 100;
        if (batsliver < 1)
            batsliver = 1; // always want at least a little on screen

        if (isCharging() && batteryPercentage < 100)
            batsliver = batsliver - ((animation_count / 25) % 2);

        uint16_t color;

        if (batteryPercentage < 20)
            color = RGB_TO_BGR565(255, 0, 0); // red
        else if (batteryPercentage < 40)
            color = RGB_TO_BGR565(252, 186, 3); // yellow
        else
            color = RGB_TO_BGR565(0, 255, 0); // green

        _buffer_ptr->fillRect(_x + (_width - batsliver - 1), _y + 1, batsliver, _height - 2, color);
        animation_count++;
    }

    void updateBatteryPercentage()
    {
        batteryPercentage = getBatteryPercentage();
        printDebug("Battery Percentage " + String(batteryPercentage));
    }

private:
    int batteryPercentage = 0;
    int animation_count = 0;
};

/****************************************************
 *                      Time
 ****************************************************/
class Time : public Drawable
{
public:
    Time(int x, int y, GFXcanvas16 *buffer_ptr)
        : Drawable(x, y, 32, 16, buffer_ptr, "Time Display")
    {
        mjd_set_timezone_est();
    }

    void draw()
    {
        time_t s = getRTCTime();
        _tmi = localtime(&s);

        String Hour = String(_tmi->tm_hour, DEC);
        String Minute = String(_tmi->tm_min, DEC);
        String Second = String(_tmi->tm_sec, DEC);

        byte hour, minute, second = 0;
        hour = _tmi->tm_hour;
        minute = (_tmi->tm_min);
        second = _tmi->tm_sec;

        char timestr[12] = "00:00:00 XM";
        if (_tmi->tm_hour > 12)
        {
            timestr[0] = '0' + ((hour - 12) / 10);
            timestr[1] = '0' + ((hour - 12) % 10);
            timestr[9] = 'P';
        }
        else if (_tmi->tm_hour == 12)
        {
            timestr[0] = '1';
            timestr[1] = '2';
            timestr[9] = 'P';
        }
        else if (_tmi->tm_hour == 0)
        {
            timestr[0] = '1';
            timestr[1] = '2';
            timestr[9] = 'A';
        }
        else
        {
            timestr[0] = '0' + (_tmi->tm_hour / 10);
            timestr[1] = '0' + (_tmi->tm_hour % 10);
            timestr[9] = 'A';
        }

        timestr[3] = '0' + (_tmi->tm_min / 10);
        timestr[4] = '0' + (_tmi->tm_min % 10);

        timestr[6] = '0' + (_tmi->tm_sec / 10);
        timestr[7] = '0' + (_tmi->tm_sec % 10);

        _buffer_ptr->setTextSize(textSize);

        _buffer_ptr->setTextColor(color);

        for (int a = 0; a < 11; a++)
        {
            _buffer_ptr->setTextColor(BACKGROUND_COLOR);
            _buffer_ptr->setCursor(_x + a * 6 * textSize + shadowOffset, _y + shadowOffset);
            _buffer_ptr->print(timestr[a]);

            _buffer_ptr->setTextColor(color);
            _buffer_ptr->setCursor(_x + a * 6 * textSize, _y);
            _buffer_ptr->print(timestr[a]);
        }
        _buffer_ptr->setTextSize(1);
    }

private:
    struct tm *_tmi;
    int shadowOffset = 1;
    int textSize = 2;
    int color = TEXT_COLOR;
};

/****************************************************
 *                 Notifications
 ****************************************************/
class NotificationPrinter : public Drawable
{
public:
    NotificationPrinter(int x, int y, int width, int height, String *notificationData, GFXcanvas16 *buffer_ptr)
        : Drawable(x, y, width, height, buffer_ptr, "Notification Area")
    {
        data = notificationData;
    }

    void draw()
    {
        _buffer_ptr->setCursor(_x, _y);
        _buffer_ptr->setTextSize(1);

        int lines = getLineCount(*data);

        for (int a = 0; a < lines; a++)
        {
            String line = parseField(*data, '\n', a);
            String name = parseField(line, ';', 0);
            _buffer_ptr->println(parseField(line, ',', 0));
        }
    }

private:
    String *data;
};

class NotificationPage : public Drawable
{
public:
    NotificationPage(String *notificationData, GFXcanvas16 *buffer_ptr)
        : Drawable(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, buffer_ptr, "Notification Page")
    {
        data = notificationData;
        setTouchable(true);
    }

    void draw()
    {
        // draw all the notifications on the left side first
        _buffer_ptr->setCursor(_x, _y);
        _buffer_ptr->setTextSize(1);

        notification_line_count = getLineCount(*data);

        for (int a = 0; a < notification_line_count; a++)
        {
            String line = parseField(*data, '\n', a);
            String appName = parseField(line, ',', NOTIFICATION_APP_NAME);

            if (a == currentSelection)
            {
                _buffer_ptr->setTextColor(BACKGROUND_COLOR);
                _buffer_ptr->fillRect(_x, _y + a * 8, app_name_width, 8, INTERFACE_COLOR);
            }
            else
                _buffer_ptr->setTextColor(INTERFACE_COLOR);

            int b = 0;
            while (b < appName.length() && b * 6 < app_name_width)
            {
                _buffer_ptr->setCursor(b * 6, a * 8);
                _buffer_ptr->print(appName[b]);
                b++;
            }

            if (a == currentSelection)
            {
                String title = parseField(line, ';', NOTIFICATION_APP_NAME);
                String description = parseField(line, ';', NOTIFICATION_DESCRIPTION);
                String extra = parseField(line, ';', NOTIFICATION_EXTRA_TEXT);

                _buffer_ptr->setTextColor(INTERFACE_COLOR);
                _buffer_ptr->setCursor(_x + app_name_width, _y);
                _buffer_ptr->print(title.substring(0, ((_width) - (_x + app_name_width)) / 6));

                scroll.setString("Subtitle:\n" + extra + "\nDescription:\n" + description);
                scroll.draw();
            }

            _buffer_ptr->drawFastVLine(app_name_width, 0, SCREEN_HEIGHT, INTERFACE_COLOR);
        }
    }

    void onTouch(int x, int y)
    {
        scroll.onTouch(x, y);
    }

    void nextItem()
    {
        scroll.resetScroll();
        if (currentSelection < notification_line_count)
            currentSelection++;
        printDebug("Next notification item " + String(currentSelection));
    }

    void previousItem()
    {
        scroll.resetScroll();
        if (currentSelection > 0)
            currentSelection--;
        printDebug("Previous notification item " + String(currentSelection));
    }

private:
    String *data;
    const int app_name_width = 36;
    Scrollbox scroll = Scrollbox(app_name_width, 10, SCREEN_WIDTH - app_name_width, SCREEN_HEIGHT - 10, _buffer_ptr);
    int currentSelection = 0;
    int notification_line_count = 0;
};