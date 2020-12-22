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
