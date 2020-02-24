
void switchToNotifications() {
  SweepClear();
  paintButtonFull(homeButton);
  page = NOTIFICATIONS;
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.print("No Notifications...");

}


void drawNotifications() {
  paintButton(homeButton);
  tft.setTextSize(1);
  tft.setCursor(0, 3);
  tft.print("No Notifications...");
}


void NotificationsTouchHandler(int x, int y) {
  if (checkButtonPress(homeButton, x, y) ) {
    switchToHome();
  }
}
