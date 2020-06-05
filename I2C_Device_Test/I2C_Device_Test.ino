//testing the I2C devices on the ESP32 Smartwatch project.
#include <Wire.h>
#define I2C_SDA 23
#define I2C_SCL 22

#define BAT_MONITOR_ADDR 0x36
#define TOUCH_ADDR 0x48

#define designcap 360   //300mAh (  (5uVh)/0.006 ohm) = 833uAh  ==> 300mAh/833uAh ~= 360 )
#define ichgterm 0x0640
#define vempty 0x9650
#define modelcfg 0x8400

void setup()
{
  // put your setup code here, to run once:
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  Wire.begin(I2C_SDA, I2C_SCL, 100000);
  Serial.begin(115200);

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

void readBatteryMonitor()
{
  int a = readRegister(BAT_MONITOR_ADDR, 0x06);
  a = a >> 8;
  Serial.print("        battery percentage = ");
  Serial.print(a);
  Serial.print("% ");
  Serial.print("(0x");
  Serial.print(a, HEX);
  Serial.println(")");
  delay(1);

  a = readRegister(BAT_MONITOR_ADDR, 0x05);
  Serial.print("        remaining capacity = ");
  Serial.print(a * 0.000833);
  Serial.print("mAh ");
  Serial.print("(0x");
  Serial.print(a, HEX);
  Serial.println(")");
  delay(1);

  a = readRegister(BAT_MONITOR_ADDR, 0x10);
  Serial.print("        total Capacity = ");
  Serial.print(a * 0.000833);
  Serial.print("mAh ");
  Serial.print("(0x");
  Serial.print(a, HEX);
  Serial.print(")");
  Serial.println("");
  delay(1);

  a = readRegister(BAT_MONITOR_ADDR, 0x19);
  float voltage = (float)a * 0.000078125;
  Serial.print("        battery voltage = ");
  Serial.print(voltage, 3);
  Serial.print("V ");
  Serial.print("(0x");
  Serial.print(a, HEX);
  Serial.print(")");
  Serial.println("");
  delay(1);

//  signed int b = (float) ;
  float amperage = ((short)readRegister(BAT_MONITOR_ADDR, 0x0A)) * 0.00026041; // for 0.006 ohm shunt
  Serial.print("        battery amperage = ");
  Serial.print(amperage, 3);
  Serial.print("A ");
  Serial.print("(0x");
  Serial.print(readRegister(BAT_MONITOR_ADDR, 0x0A), HEX);
  Serial.print(")");
  Serial.println("");
  delay(1);


  Serial.println("");
}

void loop()
{
  // put your main code here, to run repeatedly:
  readBatteryMonitor();
  delay(1000);
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
