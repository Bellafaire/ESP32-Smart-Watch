Drawable d = Drawable(10, 10, 10, 10, frameBuffer);
Background b = Background(background, frameBuffer);
Time timedisp = Time(13, 5, frameBuffer);
NotificationPrinter np = NotificationPrinter(0, 30, 100, 60, &notificationData, frameBuffer);

void setHomePage(){
    clearDrawables();
    registerDrawable(&b); 
    // registerDrawable(&d); 
    registerDrawable(&np);
    registerDrawable(&timedisp);
}