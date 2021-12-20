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
        : Drawable(x, y, width, height, buffer_ptr,"Notification Area")
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
            _buffer_ptr->print( parseField(line, ',', 0));
        }
    }

private:
    String *data;
};