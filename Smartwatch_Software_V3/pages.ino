Drawable d = Drawable(10, 50, 32, 32, frameBuffer);
Background b = Background(background, frameBuffer);
Time timedisp = Time(13, 5, frameBuffer);
NotificationPrinter np = NotificationPrinter(0, 30, 100, 60, &notificationData, frameBuffer);
NotificationPage notification_page = NotificationPage(&notificationData, frameBuffer);

void setHomePage()
{
    printDebug("Switched to home page");
    clearDrawables();
    clearSwipeActions();
    registerDrawable(&b);
    np.setDims(0, 30, 100, 60);
    registerDrawable(&np);
    registerDrawable(&timedisp);
    // registerDrawable(&d);
    // d.setTouchable(true);
    setSwipeAction(SWIPE_LEFT, setNotificationsPage);
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