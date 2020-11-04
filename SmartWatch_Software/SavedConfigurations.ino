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

void config_EEPROM_Settings() {
  SelectionWindow w = SelectionWindow(0, 14, 160, 100);

  int appSelection[APP_NUMBER];

  w.addOption(DAYLIGHT_SAVINGS_STRING);
  w.addOption(USE_ACCELEROMETER_STRING);

  int selectedOption = w.focus() - 1; //the cancel option of the selection counts as option 0, since array is zero indexed we subtract

  switch (selectedOption) {
    case DAYLIGHT_SAVINGS:
      {
        SelectionWindow w2 = SelectionWindow(0, 14, 160, 100);
        w2.addOption("no");
        w2.addOption("yes");
        int option = w2.focus() - 1;
        setDataField(DAYLIGHT_SAVINGS, (byte)option);
      }
      break;
    case USE_ACCELEROMETER:
      {
        SelectionWindow w2 = SelectionWindow(0, 14, 160, 100);
        w2.addOption("no");
        w2.addOption("yes");
        int option = w2.focus() - 1;
        setDataField((byte)option, USE_ACCELEROMETER);
      }
      break;
  }

}

void setDataField(byte data, int pos) {
  EEPROM.write(data, pos);
  EEPROM.commit();
  printDebug("Set datafield " + String(pos) + " to " + data);
}

byte readDataField(int pos) {
  return (byte)EEPROM.read(pos);
}

void clearEEPROM() {
  for (int a = 0; a < EEPROM_SIZE; a++) {
    EEPROM.write(a, 0);
  }
  EEPROM.commit();
  printDebug("EEPROM Cleared");
}

