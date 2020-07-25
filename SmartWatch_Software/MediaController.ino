/*Simple HID media controller, pair with device for play/pause and switching tracks
*/

void mediaController() {
  frameBuffer -> drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);
  frameBuffer->setTextSize(2);
  frameBuffer-> setCursor(0, 0);
  frameBuffer-> setTextColor(INTERFACE_COLOR);
  frameBuffer->print("Media Control");

  frameBuffer->setTextSize(1);
  frameBuffer-> setCursor(0, 30);
  frameBuffer-> setTextColor(INTERFACE_COLOR);
  frameBuffer-> print("Connecting..");

  tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);


  esp_ble_adv_data_t test_adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x20,
    .max_interval = 0x40,
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 32,
    .p_service_uuid = NULL, //test_service_uuid128, (for Android devices patch)
    //.flag = 0x6, //02 no conectable,06 conectable
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
  };

  esp_ble_gap_config_adv_data( &test_adv_data);


  BleKeyboard bleKeyboard("ESP32 Media Controller", "Espressif", 100);
  bleKeyboard.begin();


  button nextSong =   {SCREEN_WIDTH - 48,  30, 48, 48, INTERFACE_COLOR, BACKGROUND_COLOR, "Next"};
  button play =  {SCREEN_WIDTH / 2 - 32, 30, 64, 48, INTERFACE_COLOR, BACKGROUND_COLOR, "Play/Pause"};
  button previous =  {0, 30, 48, 48, INTERFACE_COLOR, BACKGROUND_COLOR, "Prev"};

  boolean active = true;

  frameBuffer -> drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);
  frameBuffer->setTextSize(2);
  frameBuffer-> setCursor(0, 0);
  frameBuffer-> setTextColor(INTERFACE_COLOR);
  frameBuffer->print("Media Control");

  frameBuffer->setTextSize(1);
  paintButtonFull(homeButton);
  paintButtonFull(nextSong);
  paintButtonFull(play);
  paintButtonFull(previous);

  tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);


  while (active) {
    if (bleKeyboard.isConnected()) {
      if (checkButtonPress(nextSong)) {
        //next song
        bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
        delay(200);
        bleKeyboard.releaseAll();
      } else if (checkButtonPress(play)) {
        //play
        bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
        delay(200);
        bleKeyboard.releaseAll();
      }  else if (checkButtonPress(previous)) {
        //previous
        bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
        delay(200);
        bleKeyboard.releaseAll();
      }

      frameBuffer -> drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);
      frameBuffer->setTextSize(2);
      frameBuffer-> setCursor(0, 0);
      frameBuffer-> setTextColor(INTERFACE_COLOR);
      frameBuffer->print("Media Control");
      frameBuffer->setTextColor(0x01A4);
      frameBuffer->setCursor(0, SCREEN_HEIGHT - 10);
      frameBuffer->setTextSize(1);
      frameBuffer->print("Connected");

      frameBuffer->setTextSize(1);
      paintButtonFull(homeButton);
      paintButtonFull(nextSong);
      paintButtonFull(play);
      paintButtonFull(previous);
      tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
    } else {
      frameBuffer -> drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);
      frameBuffer->setTextSize(2);
      frameBuffer-> setCursor(0, 0);
      frameBuffer-> setTextColor(INTERFACE_COLOR);
      frameBuffer->print("Media Control");
      frameBuffer->setTextColor(0x001F);
      frameBuffer->setCursor(0, SCREEN_HEIGHT - 10);
      frameBuffer->setTextSize(1);
      frameBuffer->print("Disconnected");

      frameBuffer->setTextSize(1);
      paintButtonFull(homeButton);
      paintButtonFull(nextSong);
      paintButtonFull(play);
      paintButtonFull(previous);
      tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
    }


    if (checkButtonPress(homeButton)) {
      active = false;
    }
  }

}
