#include "Declarations.h"

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif

  Wire.begin(I2C_SDA, I2C_SCL, 100000);
  initLCD();
  initTouch();
}


void deviceSleep() {
  //re-enable touch wakeup
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); //1 = High, 0 = Low
  digitalWrite(LCD_LED, LOW);
  disconnectBLE();

  Serial.flush();
  esp_light_sleep_start();
}

void clearTask(TaskHandle_t handle) {
  if (handle != NULL) {
    vTaskDelete(handle);
  }
}


void loop() {
  onWakeup();
  active();
  deviceSleep();
}


void onWakeup() {
  digitalWrite(LCD_LED, HIGH);
  initBLE();
}


void active() {

  String data = "";
  boolean notifications = false;

  while (millis() < lastTouchTime + 20000) {

    if (connected && !xNotification ) {
      xTaskCreatePinnedToCore( updateNotifications, "updateNotifications", 4096, (void *) 1 , tskIDLE_PRIORITY, &xNotification, 0 );
    }

    frameBuffer->fillScreen(0x0000);
    frameBuffer->setCursor(0, 0);
    if (!digitalRead(TOUCH_IRQ)) {
      struct point p = readTouch();
      frameBuffer->fillCircle(p.x, p.y, 4, 0xFFFF);
    }
    frameBuffer->println("Smartwatch " + String(millis()));
    if (notificationData.length() > 10) {
      frameBuffer->println(notificationData);
    }
    tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  }
}
void updateNotifications(void * pvParameters) {
  printDebug("Obtaining Notifications");
  if (pRemoteCharacteristic) {
    notificationData = sendBLE("/notifications", true);
  }
  vTaskDelete(NULL);
}
