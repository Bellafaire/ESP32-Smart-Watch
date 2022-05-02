Drawable d = Drawable(10, 50, 32, 32, frameBuffer);
BoxesBackground b = BoxesBackground(frameBuffer);
Time timedisp = Time(13, 5, frameBuffer);
NotificationPrinter np = NotificationPrinter(0, 30, 100, 60, &notificationData, frameBuffer);
NotificationPage notification_page = NotificationPage(&notificationData, frameBuffer);
BatteryIcon bat = BatteryIcon(SCREEN_WIDTH - 14, 3, frameBuffer);
ConnStatus con = ConnStatus(SCREEN_WIDTH - 14, 10, frameBuffer);
Calculator calculator_page = Calculator(frameBuffer);
CalibrationScreen calibration_page = CalibrationScreen(frameBuffer);
// AppIconItem app_icon = AppIconItem(10,30, "snapchat", frameBuffer);
// AppIconItem app_icon2 = AppIconItem(10 + 36,30, "gmail", frameBuffer);
// AppIconItem app_icon3 = AppIconItem(10 + 36*2,30, "tasks", frameBuffer);
ApplicationNotification app_not = ApplicationNotification(10, 30, "notification maker", &notificationData, frameBuffer);
// NotificationGrid notification_grid = NotificationGrid(0, 30, 160, 60, &notificationData, frameBuffer);

void setHomePage()
{
    printDebug("Switched to home page");
    clearDrawables();
    clearSwipeActions();
    registerDrawable(&b);
    np.setDims(0, 30, 100, 60);
    registerDrawable(&np);
    registerDrawable(&timedisp);
    registerDrawable(&bat);
    bat.updateBatteryPercentage();
    registerDrawable(&con);
    // registerDrawable(&d);
    // d.setTouchable(true);

    // registerDrawable(&app_icon);
    // registerDrawable(&app_icon2);
    // registerDrawable(&app_icon3);
    registerDrawable(&app_not);

    setSwipeAction(SWIPE_LEFT, setNotificationsPage);
    setSwipeAction(SWIPE_UP, setCalculatorPage);
}

void nextNotificationPageItem()
{
    notification_page.nextItem();
}

void previousNotificationPageItem()
{
    notification_page.previousItem();
}

void setNotificationsPage()
{
    printDebug("Switched to notifications page");
    clearDrawables();
    clearSwipeActions();
    registerDrawable(&b);
    registerDrawable(&notification_page);
    setSwipeAction(SWIPE_RIGHT, setHomePage);
    setSwipeAction(SWIPE_UP, nextNotificationPageItem);
    setSwipeAction(SWIPE_DOWN, previousNotificationPageItem);
}

void setCalculatorPage()
{
    printDebug("Switched to calculator");
    clearDrawables();
    clearSwipeActions();
    registerDrawable(&b);               // background
    registerDrawable(&calculator_page); // actual calculator page
    setSwipeAction(SWIPE_DOWN, setHomePage);
}

void setCalibrationPage()
{
    printDebug("Switched to calibration page");
    clearDrawables();
    clearSwipeActions();
    registerDrawable(&calibration_page);
    setSwipeAction(SWIPE_UP, setHomePage);
}