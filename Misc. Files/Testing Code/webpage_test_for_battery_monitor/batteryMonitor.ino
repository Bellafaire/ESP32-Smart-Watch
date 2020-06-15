
void initBatMonitor()
{
  int StatusPOR = readRegister(BAT_MONITOR_ADDR, 0x00) & 0x0002;
  if (StatusPOR == 0)
  {
  }
  else
  {

    while (readRegister(BAT_MONITOR_ADDR, 0x3D) & 1)
    {
      delay(10);
    }
    int    HibCFG = readRegister(BAT_MONITOR_ADDR, 0xBA); //Store original HibCFG value
    sendWrite(BAT_MONITOR_ADDR, 0x60, 0x90);       // Exit Hibernate Mode step 1
    sendWrite(BAT_MONITOR_ADDR, 0xBA, 0x0);        // Exit Hibernate Mode step 2
    sendWrite(BAT_MONITOR_ADDR, 0x60, 0x0);        // Exit Hibernate Mode step 3

   sendWrite(BAT_MONITOR_ADDR, 0x18, designcap); // Write DesignCap
    sendWrite(BAT_MONITOR_ADDR, 0x1E, ichgterm);  // Write IchgTerm
    sendWrite(BAT_MONITOR_ADDR, 0x3A, vempty);    // Write VEmpty

    sendWrite(BAT_MONITOR_ADDR, 0xDB, 0x8000); // Write ModelCFG

    //Poll ModelCFG.Refresh(highest bit),
    //proceed to Step 3 when ModelCFG.Refresh=0.
    while (readRegister(BAT_MONITOR_ADDR, 0xDB) & 0x8000)
      delay(10);  //do not continue until ModelCFG.Refresh==0
    sendWrite(BAT_MONITOR_ADDR, 0xBA, HibCFG); // Restore Original HibCFG value
  }

  int Status = readRegister(BAT_MONITOR_ADDR, 0x00);   //Read Status
  WriteAndVerifyRegister(0x00, Status & 0xFFFD); //Write and Verify

  StatusPOR = readRegister(BAT_MONITOR_ADDR, 0x00) & 0x0002; //Read POR bit in Status Register

  int RepCap = readRegister(BAT_MONITOR_ADDR, 0x05); //Read RepCap
  Serial.print("RepCap 0x");
  Serial.println(RepCap, HEX);

  int RepSOC = readRegister(BAT_MONITOR_ADDR, 0x06); //Read RepSOC
  Serial.print("RepSOC 0x");
  Serial.println(RepSOC, HEX);

  int TTE = readRegister(BAT_MONITOR_ADDR, 0x11); //Read TTE
  Serial.print("TTE 0x");
  Serial.println(TTE, HEX);

  int VCELL = readRegister(BAT_MONITOR_ADDR, 0x09); //Read VCELL
  Serial.print("VCELL 0x");
  Serial.println(VCELL, HEX);
}


void WriteAndVerifyRegister(char RegisterAddress, int RegisterValueToWrite)
{
  int Attempt = 0;
  int RegisterValueRead;
  do
  {
    sendWrite(BAT_MONITOR_ADDR, RegisterAddress, RegisterValueToWrite);
    delay(1); //1ms
    RegisterValueRead = readRegister(BAT_MONITOR_ADDR, RegisterAddress);
  } while (RegisterValueToWrite != RegisterValueRead && Attempt++ < 3);
}


String readBatteryMonitor()
{
  String output = ""; 
  int a = readRegister(BAT_MONITOR_ADDR, 0x06);
  a = a >> 8;
  output += "        battery percentage = ";
  output += a;
  output += "%\n ";
  delay(1);

  a = readRegister(BAT_MONITOR_ADDR, 0x05);
  output += "        remaining capacity = ";
  output += a * 0.000833;
  output += "mAh \n";
  delay(1);

  a = readRegister(BAT_MONITOR_ADDR, 0x10);
  output += "        total Capacity = ";
  output += a * 0.000833;
  output += "mAh ";
  output += "\n";
  delay(1);

  a = readRegister(BAT_MONITOR_ADDR, 0x19);
  float voltage = (float)a * 0.000078125;
  output += "        battery voltage = ";
  output += voltage;
  output += "V ";
  output += "\n";
  delay(1);

//  signed int b = (float) ;
  float amperage = ((short)readRegister(BAT_MONITOR_ADDR, 0x0A)) * 0.00026041; // for 0.006 ohm shunt
  output += "        battery amperage = ";
  output += amperage;
  output += "A ";
  output += "\n";
  delay(1);


  output += "";
return output;
}

void sendWrite(byte deviceAddr, byte location, int d)
{
  Wire.beginTransmission(BAT_MONITOR_ADDR);
  Wire.write(location);
  Wire.write(d & 0x00FF);
  Wire.write(d >> 8);
  Wire.endTransmission();
}

int readRegister(byte deviceAddr, byte location)
{
  Wire.beginTransmission(deviceAddr); // select device with "beginTransmission()"
  Wire.write(location);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddr, 2, true); //request battery percentage

    int  a = Wire.read();
    a |= Wire.read() << 8;

//  int a = Wire.read() << 8 | Wire.read();
  return a;
}


int detectDevice(byte addr)
{
  delay(10);
  Wire.beginTransmission(addr);
  byte error = Wire.endTransmission();
  if (error == 0)
  {
    Serial.print("Device Detected at 0x");
    Serial.println(addr, HEX);
  }
  else
  {
    Serial.print("-------- FAILED TO DETECT DEVICE AT ");
    Serial.print(addr, HEX);
    Serial.println(" --------");
  }
  return error;
}
