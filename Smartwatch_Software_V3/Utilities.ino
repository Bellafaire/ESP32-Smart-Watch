String parseField(String data, char delim, int index)
{
    // basically we want to find the string between delim number index and index+1
    int delim1 = -1;
    int delim2 = -1;
    if (index == 0)
        delim1 = 0; // special case

    int delim_count = 0;
    for (int a = 0; a < data.length(); a++)
    {
        delim_count += data[a] == delim ? 1 : 0;
        if ((delim_count == index) && delim1 < 0)
            delim1 = a + 1;
        if ((delim_count == index + 1) && delim2 < 0)
            delim2 = a;
    }
    if (delim2 == -1 && delim1 > 0)
        delim2 = data.length();
    return data.substring(delim1, delim2);
}

int getLineCount(String data)
{
    // count number of '\n' characters
    int count = 0;
    for (int a = 0; a < data.length(); a++)
    {
        count += data[a] == '\n' ? 1 : 0;
    }
    return count;
}

uint8_t iconchar[2 * 100 * 100];
uint8_t icon_decode_buffer[2 * 32 * 32];
String iconBuffer = "";

// current request locks the getIconBLE function to only attempting to request one icon at a time, it can happen that the
// icons get jumbled with half of it being from one app and the other half being from another.
String currentRequest = "";

boolean writeIconToFile(String appname, String data)
{
    String path = "/" + appname + ".txt"; // saving these as base64 because the EOF terminator can ruin our day.
    printDebug("Writing icon to file " + path);

    File file = SPIFFS.open(path, FILE_WRITE);
    if (file)
    {
        boolean result = file.print(data);
        if (result)
            printDebug(" -- Wrote icon to file");
        else
            printDebug(" -- Failed to write icon to file");
        file.close();
        return true;
    }
    else
        printDebug(" -- Failed to open file");
    return false;
}

boolean loadIconFromFile(String appname, uint16_t *des)
{
    String path = "/" + appname + ".txt";
    printDebug("Attempting to read app icon from file \"" + path + "\"");
    File f = SPIFFS.open(path);
    if (f)
    {
        printDebug(" -- File found ");
        int position = 0;
        while (f.available())
        {
            char c = f.read();
            iconchar[position] = c;
            position++;
        }
        f.close();

        if (position > 2000)
        {
            printDebug(" -- Read data from file \"" + path + "\"");
            decode_base64(iconchar, position, icon_decode_buffer);
            for (int a = 0; a < 32 * 32; a++)
                des[a] = (icon_decode_buffer[a * 2] << 8) | icon_decode_buffer[a * 2 + 1];

            printDebug(" -- Loaded Icon");
            return true;
        }
        else
        {
            printDebug(" -- Found file was too small, requesting from device");
            return getIconBLE(appname, des);
        }
    }
    else
    {
        printDebug(" -- Icon not found, requesting from phone");
        return getIconBLE(appname, des);
    }
}

boolean getIconBLE(String appname, uint16_t *des)
{
    if (appname.equals(currentRequest) || currentRequest.equals(""))
    {
        currentRequest = appname;
        if (connected && timeUpdated && notificationsUpdated)
        {
            tft.setCursor(0, SCREEN_HEIGHT - 20);
            tft.fillRect(0, SCREEN_HEIGHT - 25, SCREEN_WIDTH, 25, 0x0000);
            tft.println("Downloading Icon: " + appname);
            boolean success = sendBLE("/icon:" + appname, &iconBuffer, true);
            if (success && iconBuffer.length() > 1000)
            {
                printDebug(" -- Saving app icon to file");
                writeIconToFile(appname, iconBuffer);
                printDebug(" -- Decoding");
                iconBuffer.toCharArray((char *)iconchar, iconBuffer.length() * 2);
                decode_base64(iconchar, iconBuffer.length() * 2, icon_decode_buffer);
                for (int a = 0; a < 32 * 32; a++)
                    des[a] = (icon_decode_buffer[a * 2] << 8) | icon_decode_buffer[a * 2 + 1];
                currentRequest = "";
                return true;
            }
            else
                printDebug("  -- Something went wrong requesting image over BLE");
        }
        printDebug(" -- cannot get icon, notifications or time still need to be updated");
    }
    else
        printDebug(" -- failed to get icon, lock is maintained by another app requesting an icon");
    return false;
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = SPIFFS.open(dirname);
    if (!root)
    {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
