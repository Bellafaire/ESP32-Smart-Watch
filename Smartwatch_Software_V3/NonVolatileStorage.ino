/*****************************************************************************
  The MIT License (MIT)
  Copyright (c) 2020 Matthew James Bellafaire
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
******************************************************************************/
/* Certain settings are best stored in Flash since we'll want to retain them regardless
    provided these settings aren't modified too often there shouldn't be any issues with the
    hardware side of things. The locations for various settings are defined in the declarations.h
    file */
#include <Preferences.h>

Preferences pref;

void setDataField(int16_t data, String key)
{
  const char *key_char = key.c_str();

  pref.begin("smartwatch", false);
  pref.putShort(key_char, data);
  pref.end();
}

int16_t readDataField(String key)
{
  int16_t value;
  const char *key_char = key.c_str();
  pref.begin("smartwatch", false);
  value = pref.getShort(key_char);
  pref.end();
  return value;
}

void clearEEPROM()
{
  for (int a = 0; a < EEPROM_SIZE; a++)
  {
    EEPROM.write(a, 0);
  }
  EEPROM.commit();
  printDebug("EEPROM Cleared");
}

void loadEEPROMSettings()
{

  pref.begin("smartwatch", false);

  SETTING_DAYLIGHT_SAVINGS = pref.getShort("daylight_sav", 0);
  SETTING_WAKEUP_TYPE = pref.getShort("wake_type", 0);
  SETTING_SCREEN_BRIGHTNESS = map(pref.getShort("brightness", 0), 0, 100, 0, 255);

  // load touch calibration data from memory
  for (int a = 0; a < TOTAL_CALIBRATION_POINTS; a++)
  {
    calibrationX[a] = pref.getShort(("calx" + String(a)).c_str(), -1);
    calibrationY[a] = pref.getShort(("caly" + String(a)).c_str(), -1);
  }

  pref.end();

#ifdef DEBUG
  Serial.println("EEPROM Settings: \n Daylights Savings: " + String(SETTING_DAYLIGHT_SAVINGS) + "\n Accelerometer Wakeup: " + String(SETTING_WAKEUP_TYPE) + "\n Screen brightness: " + String(SETTING_SCREEN_BRIGHTNESS));
  Serial.println("Touch Calibration:");
  for (int a = 0; a < 4; a++)
    Serial.printf("   (%d, %d)\n", calibrationX[a], calibrationY[a]);
#endif
}
