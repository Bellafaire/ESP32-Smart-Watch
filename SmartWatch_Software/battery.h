
//crudely determines the battery voltage by assuming the ESP32 is running at a fixed 3.3 volts (it is until the battery gets below 3.4V)
float getBatteryVoltage(){
  float v = analogRead(BATTERY_SENSE)/4096.0;
  v *= 2; // two 10k resistors act as a voltage divider for the sensing

  return  3.3 * v;   
}
