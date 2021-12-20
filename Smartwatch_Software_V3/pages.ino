Drawable d = Drawable(10, 50, 32, 32, frameBuffer);
Background b = Background(background, frameBuffer);
Time timedisp = Time(13, 5, frameBuffer);
NotificationPrinter np = NotificationPrinter(0, 30, 100, 60, &notificationData, frameBuffer);

void setHomePage()
{
    clearDrawables();
    registerDrawable(&b);
    registerDrawable(&np);
    registerDrawable(&timedisp);
    registerDrawable(&d);
    d.setTouchable(true);
}