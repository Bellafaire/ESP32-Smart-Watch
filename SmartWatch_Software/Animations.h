void SweepClear() {
  for (int a = 0; a < SCREEN_WIDTH + 1; a++) {
    tft.drawFastVLine(a, 0, SCREEN_HEIGHT, INTERFACE_COLOR);
    tft.drawFastVLine(a - 1, 0, SCREEN_HEIGHT, BACKGROUND_COLOR);
  }
}
