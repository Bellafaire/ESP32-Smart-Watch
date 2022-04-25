/****************************************************
 *                   Background
 ****************************************************/
class BoxesBackground : public Drawable
{
public:
    BoxesBackground(GFXcanvas16 *buffer_ptr)
        : Drawable(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, buffer_ptr, "Background")
    {
        row_height = _height / rows;
        for (int a = 0; a < columns; a++)
            for (int b = 0; b < rows; b++)
                widths[a][b] = _width / columns;
    }

    void draw()
    {

        // get random index in widths
        if (widthModification == 0)
        {
            // int index = random(0, rows * columns);
            // int add = random(-1, 2); // random is min to max-1
            // widths[index % columns][index / columns] += add;

            // pick a random index
            current_row = random(0, rows);
            current_column = random(0, columns);
            widthModification = random(0, 15); // random is min to max-1

            if (widths[current_column][current_row] > _width / columns)
                widthModification *= -1;
        }

        widths[current_column][current_row] += widthModification > 0 ? 1 : -1;
        if (current_column < columns - 1)
            widths[current_column + 1][current_row] += widthModification > 0 ? -1 : 1;
        else
            widths[current_column - 1][current_row] += widthModification > 0 ? -1 : 1;
        widthModification += (widthModification > 0) ? -1 : 1;

        // _buffer_ptr->drawRGBBitmap(0, 0, _img, _width, _height);
        for (int b = 0; b < rows; b++)
        {
            int current_pos = 0;
            for (int a = 0; a < columns; a++)
            {

                _buffer_ptr->fillRect(_x + current_pos, _y + b * row_height, widths[a][b], row_height, colors[(b * columns + a) % 6]);
                current_pos += widths[a][b];
                if (current_pos < _width)
                    _buffer_ptr->fillRect(_x + current_pos, _y + b * row_height, _width - current_pos, row_height, colors[(b * columns + a) % 6]);
                // Serial.printf("Drawing rectangle at x:%d y:%d with width:%d and height:%d\n", _x + current_pos, _y + b * row_height, widths[a][b], row_height);
            }
        }
    }

private:
    const int arr_size = (sizeof(colors) / sizeof((colors)[0]));
    const static int rows = 8;
    const static int columns = 4;
    int row_height;
    uint16_t colors[6] = {
        RGB_TO_BGR565(78, 0, 50),
        RGB_TO_BGR565(78, 0, 70),
        RGB_TO_BGR565(56, 0, 50),
        RGB_TO_BGR565(80, 0, 100),
        RGB_TO_BGR565(65, 0, 45),
        RGB_TO_BGR565(50, 0, 60)};
    int widths[columns][rows];

    int current_row = 0;
    int current_column = 0;
    int widthModification = 0;
};

class ImageBackground : public Drawable
{
public:
    ImageBackground(const uint16_t *img, GFXcanvas16 *buffer_ptr)
        : Drawable(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, buffer_ptr, "Background")
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
            if (!notificationsUpdated || !timeUpdated)
                _buffer_ptr->fillRect(_x, _y, _width, _height, RGB_TO_BGR565(0, 0, 255));
            else
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

        _buffer_ptr->fillRect(_x + 1, _y, _width - 1, _height, RGB_TO_BGR565(50, 45, 50));
        _buffer_ptr->fillRect(_x, _y + 2, _width, _height - 4, RGB_TO_BGR565(50, 45, 50));

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
        }

        if (notification_line_count == 0)
        {
            _buffer_ptr->setCursor(0, 0);
            _buffer_ptr->println("No Notifications...");
        }
        else
        {
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

/****************************************************
 *                    Button
 ****************************************************/

class Button : public Drawable
{
public:
    Button(int x, int y, int width, int height, String text, GFXcanvas16 *buffer_ptr)
        : Drawable(x, y, width, height, buffer_ptr, "Button:" + text)
    {
        setTouchable(true);
        _text = text;
    }

    Button()
        : Drawable(-1, -1, 0, 0, nullptr, "uninitalized Button")
    {
    }

    Button(int x, int y, int width, int height, const uint16_t *image, int imageWidth, int imageHeight, GFXcanvas16 *buffer_ptr)
        : Drawable(x, y, width, height, buffer_ptr, "ImageButton")
    {
        setTouchable(true);
        _imageWidth = imageWidth;
        _imageHeight = imageHeight;
        _img = image;
    }

    void setBackgroundColor(uint16_t color)
    {
        _backgroundColor = color;
    }
    void setForegroundColor(uint16_t color)
    {
        _foregroundColor = color;
    }

    void setBorder(boolean border)
    {
        _border = border;
    }

    String getText()
    {
        return _text;
    }

    void draw()
    {
        // border is optional, may not want this in cases such as image drawing
        if (_border)
        {
            _buffer_ptr->fillRect(_x, _y, _width, _height, _backgroundColor);
            _buffer_ptr->drawRect(_x, _y, _width, _height, _foregroundColor);
        }

        if (!_text.equals(""))
        {
            // calculate width of the text itself.
            int text_width = _text.length() * 6;

            // get center of box
            int center_x = _x + _width / 2;
            int center_y = _y + _height / 2;

            // start cursor at
            _buffer_ptr->setCursor(
                center_x - text_width / 2,
                center_y - 3);

            _buffer_ptr->setTextColor(_foregroundColor);
            _buffer_ptr->print(_text);
        }
        else if (_img != nullptr)
        {
            int start_x = _x + (_width / 2) - _imageWidth / 2;
            int start_y = _y + (_height / 2) - _imageHeight / 2;

            // now loop through each pixel, consider black to be transparent.
            for (int y = 0; y < _imageHeight; y++)
                for (int x = 0; x < _imageWidth; x++)
                    if (_img[x + y * _imageWidth] != 0x0000)
                        _buffer_ptr->drawPixel(x + start_x, y + start_y, _img[x + y * _imageWidth]);
        }
    }

private:
    // support either images or text
    String _text = "";
    boolean _border = true;
    int _imageWidth = -1;
    int _imageHeight = -1;
    const uint16_t *_img;
    uint16_t _backgroundColor = BACKGROUND_COLOR;
    uint16_t _foregroundColor = INTERFACE_COLOR;
};

/****************************************************
 *                    Calculator
 ****************************************************/
class Calculator : public Drawable
{
public:
    Calculator(GFXcanvas16 *buffer_ptr)
        : Drawable(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, buffer_ptr, "Calculator")
    {

        for (int a = 0; a < CALCULATOR_COLUMNS * CALCULATOR_ROWS; a++)
        {
            calculatorButtons[a] = Button(
                xButtonSpacing * (a % CALCULATOR_COLUMNS) + PADDING / 2,
                yButtonSpacing * ((a) / CALCULATOR_COLUMNS) + PADDING / 2 + yButtonSpacing,
                xButtonSpacing - PADDING,
                yButtonSpacing - PADDING,
                calculatorButtonLabels[a],
                _buffer_ptr);
        }
        setTouchable(true);
    }

    void draw()
    {

        // draw the
        _buffer_ptr->fillRect(_x + PADDING / 2, _y + PADDING / 2, _width - PADDING, yButtonSpacing - PADDING, BACKGROUND_COLOR);
        _buffer_ptr->drawRect(_x + PADDING / 2, _y + PADDING / 2, _width - PADDING, yButtonSpacing - PADDING, INTERFACE_COLOR);
        _buffer_ptr->setCursor(_x + _width - PADDING / 2 - 6 * (dispStr.length() + 1), _y + (yButtonSpacing - 6) / 2);
        _buffer_ptr->print(dispStr);

        for (int a = 0; a < CALCULATOR_COLUMNS * CALCULATOR_ROWS; a++)
            calculatorButtons[a].draw();

        // request to keep screen on for the next 250 milliseconds (this way the calculator can be kept going indefinitely)
        requestWakeLock(250);
    }

    void addToText()
    {
        printDebug("addToText()");
    }

    void onTouch(int x, int y)
    {
        if (lastTouch + TOUCH_COOLDOWN < millis())
        {
            for (int a = 0; a < TOTAL_BUTTONS; a++)
            {
                boolean touched = calculatorButtons[a].isTouched(x, y);
                if (touched)
                {
                    // dispStr += calculatorButtons[a].getText();
                    char character = calculatorButtons[a].getText()[0]; // get the character
                    switch (character)
                    {
                    case '=':
                        printDebug("Calculating");
                        performCalculation();
                        break;
                    case 'C':
                        dispStr = "";
                        firstValue = "";
                        secondValue = "";
                        op = "";
                        break;
                    case '<':
                        if (op.length() > 0)
                            secondValue = secondValue.substring(0, secondValue.length() - 1);
                        else
                            firstValue = firstValue.substring(0, firstValue.length() - 1);
                        dispStr = firstValue + " " + op + " " + secondValue;
                        break;
                    default:
                        addCharacter(character);
                    }
                }
            }
            lastTouch = millis();
        }
    }

    void addCharacter(char c)
    {
        if (c == '+' || c == '-' || c == '*' || c == '/')
            op = String(c);
        else if (op.length() > 0)
            secondValue += c;
        else
            firstValue += c;

        dispStr = firstValue + " " + op + " " + secondValue;
    }

    void performCalculation()
    {
        double first = firstValue.toDouble();
        double second = secondValue.toDouble();
        double result = 0;
        boolean error = false;
        switch (op[0])
        {
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
            if (second != 0)
            {
                result = first / second;
            }
            else
            {
                error = true;
            }
            break;
        default:
            error = true;
            break;
        }

        if (error)
        {
            firstValue = "NaN";
        }
        else
        {
            firstValue = String(result, 4);
        }
        secondValue = "";
        op = "";
        dispStr = firstValue + " " + op + " " + secondValue;
    }

private:
    static const int CALCULATOR_COLUMNS = 5;
    static const int CALCULATOR_ROWS = 4;
    static const int TOTAL_BUTTONS = CALCULATOR_COLUMNS * CALCULATOR_ROWS;
    static const int PADDING = 4;
    static const int TOUCH_COOLDOWN = 100;

    unsigned long lastTouch = 0;
    int xButtonSpacing = SCREEN_WIDTH / CALCULATOR_COLUMNS;
    int yButtonSpacing = SCREEN_HEIGHT / (CALCULATOR_ROWS + 1); // the actual result will be the top row

    String dispStr = "";
    String firstValue = "";
    String secondValue = "";
    String op = "";

    Button calculatorButtons[CALCULATOR_COLUMNS * CALCULATOR_ROWS];
    String calculatorButtonLabels[CALCULATOR_COLUMNS * CALCULATOR_ROWS] = {
        "7", "8", "9", "/", "<",
        "4", "5", "6", "*", "",
        "1", "2", "3", "-", "C",
        "0", ".", "", "+", "="};
};

/****************************************************
 *              CALIBRATION SCREEN
 ****************************************************/
class CalibrationScreen : public Drawable
{
public:
    CalibrationScreen(GFXcanvas16 *buffer_ptr)
        : Drawable(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, buffer_ptr, "CalibrationScreen")
    {
        lastTouch = millis();
    }

    void draw()
    {
        _buffer_ptr->fillScreen(BACKGROUND_COLOR);
        _buffer_ptr->setCursor(0, 10);
        _buffer_ptr->setTextColor(INTERFACE_COLOR);
        _buffer_ptr->print("Touch Calibration");
        if (state < 4)
        {
            _buffer_ptr->drawCircle(CALIBRATION_POINTS_X[state], CALIBRATION_POINTS_Y[state], 5, RGB_TO_BGR565(255, 0, 0));
            _buffer_ptr->drawCircle(CALIBRATION_POINTS_X[state], CALIBRATION_POINTS_Y[state], 1, RGB_TO_BGR565(255, 0, 0));
        }
        else
        {
            _buffer_ptr->setCursor(0, 20);
            _buffer_ptr->setTextColor(INTERFACE_COLOR);
            _buffer_ptr->print("Swipe up to confirm calibration and exit");
            struct point p = readTouch();
            _buffer_ptr->fillCircle(p.x, p.y, 5, RGB_TO_BGR565(255, 0, 0));
        }
        readTouchState();
    }

    // this operates differently from the onTouch method since it needs to bypass some of the checks that
    // prevent invalid data
    void readTouchState()
    {
        if ((lastTouch + TOUCH_COOLDOWN) < millis())
        {
            printDebug("touchstate");
            // we're just going to ignore the parameter x y and get the raw values ourselves
            struct point p = readTouchRaw();
            if (state < 4)
                if (!digitalRead(TOUCH_IRQ))
                {
                    printDebug("Raw Reading from touchscreen " + String(p.x) + "," + String(p.y));
                    calx[state] = p.x;
                    caly[state] = p.y;

                    state++;
                }

            if (state == TOTAL_CALIBRATION_POINTS)
            {
                // load calibration data into the EEPROM
                for (int a = 0; a < TOTAL_CALIBRATION_POINTS; a++)
                {
                    setDataField((byte)(calx[a] >> 8), CALIBRATION_X1 + a * 2);
                    setDataField((byte)(calx[a] & 0xFF), CALIBRATION_X1 + a * 2 + 1);
                    setDataField((byte)(caly[a] >> 8), CALIBRATION_Y1 + a * 2);
                    setDataField((byte)(caly[a] & 0xFF), CALIBRATION_Y1 + a * 2 + 1);
                }

                loadEEPROMSettings();
                CLEAR_TOUCH_CALIBRATION = false;
                state++;
            }
            lastTouch = millis();
        }
    }

private:
    uint16_t calx[4];
    uint16_t caly[4];
    static const int TOUCH_COOLDOWN = 1000;
    unsigned long lastTouch = 0;
    int state = 0;
};