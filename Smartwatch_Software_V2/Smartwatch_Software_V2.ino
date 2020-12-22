#include "Declarations.h"

//BLE related variables
//UUID's for the services used by the android app (change as you please if you're building this yourself, just match them in the android app)
static BLEUUID serviceUUID("d3bde760-c538-11ea-8b6e-0800200c9a66");
static BLEUUID    charUUID("d3bde760-c538-11ea-8b6e-0800200c9a67");

//important variables used to establish BLE communication
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
static BLEClient*  pClient;
TaskHandle_t xConnect = NULL;
static volatile boolean connected = false;
static boolean registeredForCallback = false;

//function signatures for functions in BluetoothReceive.ino
String sendBLE(String command, bool hasReturnData);
void initBLE();
void xFindDevice(void * pvParameters ) ;
void formConnection(void * pvParameters) ;



void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  EEPROM.begin(EEPROM_SIZE);
  Wire.begin(I2C_SDA, I2C_SCL, 100000);
  initLCD();
  initBatMonitor();
  initTouch();

  //temporary until settings configuration option is pulled in
  setDataField(0, DAYLIGHT_SAVINGS);
}


void deviceSleep() {
  //re-enable touch wakeup
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); //1 = High, 0 = Low
  digitalWrite(LCD_LED, LOW);
  if (connected) {
    pClient->disconnect();
    while (connected) {
      delay(1);
    };
    printDebug("disconnected");
  }
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
  getRTCTime();
  printRTCTime();
  digitalWrite(LCD_LED, HIGH);
  //initalizes BLE connection in seperate thread
  //when connected will update the "connected" variable to true
  initBLE();
}


void active() {
  String notificationData = "";

  while (millis() < lastTouchTime + 10000) {

    if (connected && notificationData.length() < 10) {
      notificationData = sendBLE("/notifications", true); //gets current android notifications as a string
      updateTimeFromNotificationData(notificationData);
    }

    frameBuffer->drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

    drawTime(3, 5, 2, 0xFFFF, 1);
    drawDateCentered(20, 1);

    frameBuffer->setTextColor(0x0000);
    frameBuffer->setCursor(SCREEN_WIDTH - 6 * 4, 5 );
    frameBuffer->println(String(getBatteryPercentage()) + "%");

    frameBuffer->setTextColor(0xFFFF);
    frameBuffer->setCursor(SCREEN_WIDTH - 6 * 4 - 1, 5 - 1);
    frameBuffer->println(String(getBatteryPercentage()) + "%");


    drawNotifications(notificationData, 0, 30, 0xFFFF);

    tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  }
}


void drawNotifications(String notificationData, int x, int y, int color) {
  //count lines
  int lineCount = 0;
  for (int a = 0; a < notificationData.length(); a++) {
    if (notificationData[a] == '\n') {
      lineCount++;
    }
  }
  //last 2 lines are not notification data, they're time and EOM terminator
  lineCount = lineCount - 1;

  frameBuffer->setTextColor(color);
  frameBuffer->setCursor(x, y);

  for (int a = 0; a < lineCount; a++) {
    String line = getValue(notificationData, '\n', a);
    frameBuffer->println(getValue(line, ';', 0));
  }
}
