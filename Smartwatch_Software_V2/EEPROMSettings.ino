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
/* Certain settings are best stored in EEPROM since we'll want to retain them regardless
    provided these settings aren't modified too often there shouldn't be any issues with the
    hardware side of things. The locations for various settings are defined in the declarations.h
    file */
void setDataField(byte data, int pos) {
  EEPROM.write(pos, data);
  EEPROM.commit();
  printDebug("Set datafield " + String(pos) + " to " + data);
}

byte readDataField(int pos) {
  byte data = (byte)EEPROM.read(pos);
  printDebug("Read datafield: " + String(pos) + " found value " + String(data));
  return data;
}

void clearEEPROM() {
  for (int a = 0; a < EEPROM_SIZE; a++) {
    EEPROM.write(a, 0);
  }
  EEPROM.commit();
  printDebug("EEPROM Cleared");
}

void loadEEPROMSettings() {
  SETTING_DAYLIGHT_SAVINGS = readDataField(DAYLIGHT_SAVINGS);
  SETTING_WAKEUP_TYPE = readDataField(WAKEUP_TYPE);
  SETTING_SCREEN_BRIGHTNESS = map(readDataField(SCREEN_BRIGHTNESS), 0, 100, 0, 255);
  printDebug("EEPROM Settings: \n Daylights Savings: " + String(SETTING_DAYLIGHT_SAVINGS) + "\n Accelerometer Wakeup: " + String(SETTING_WAKEUP_TYPE) + "\n Screen brightness: " + String(SETTING_SCREEN_BRIGHTNESS));
}
