/*
 * ESP32 AJAX Demo
 * Updates and Gets data from webpage without page refresh
 * https://circuits4you.com
 */
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Wire.h>
 
#include "index.h"  //Web page header file

 #define I2C_SDA 23
#define I2C_SCL 22

#define BAT_MONITOR_ADDR 0x36
#define TOUCH_ADDR 0x48

#define designcap 360   //300mAh (  (5uVh)/0.006 ohm) = 833uAh  ==> 300mAh/833uAh ~= 360 )
#define ichgterm 0x0640
#define vempty 0x9650
#define modelcfg 0x8400

WebServer server(80);
 
//Enter your SSID and PASSWORD
const char* ssid = "Bellafaire Family Wifi";  // Enter your SSID here
const char* password = "cashewbunny";  //Enter your Password here
 
//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
 
void handleADC() {
 String adcValue = readBatteryMonitor();
 
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}
 
//===============================================================
// Setup
//===============================================================
 
void setup(void){
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
 
  Wire.begin(I2C_SDA, I2C_SCL, 100000);

  for (int a = 0; a < 10; a++)
  {
    Serial.println(""); //ESP32 throws a lot of junk to the terminal on startup, just want to get away from that
  }

  Serial.println("Self-test for I2C Devices of ESP32 Smartwatch");

  Serial.println("Checking for connection to I2C devices");

  Serial.print("Battery Monitor: ");
  detectDevice(BAT_MONITOR_ADDR);
  Serial.print("Touch Controller: ");
  detectDevice(TOUCH_ADDR);

  initBatMonitor();
/*
//ESP32 As access point
  WiFi.mode(WIFI_AP); //Access Point mode
  WiFi.softAP(ssid, password);
*/
//ESP32 connects to your wifi -----------------------------------
  WiFi.mode(WIFI_STA); //Connectto your wifi
  WiFi.begin(ssid, password);
 
  Serial.println("Connecting to ");
  Serial.print(ssid);
 
  //Wait for WiFi to connect
  while(WiFi.waitForConnectResult() != WL_CONNECTED){      
      Serial.print(".");
    }
    
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
//----------------------------------------------------------------
 
  server.on("/", handleRoot);      //This is display page
  server.on("/readADC", handleADC);//To get update of ADC Value only
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}
 
//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void loop(void){
  server.handleClient();
  delay(1);
}
