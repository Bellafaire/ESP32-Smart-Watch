
String parseField(String data, char delim, int index)
{
    // basically we want to find the string between delim number index and index+1
    int delim1 = -1;
    int delim2 = -1;
    if (index == 0)
        delim1 = 0; //special case

    int delim_count = 0;
    for (int a = 0; a < data.length(); a++)
    {
        delim_count += data[a] == delim ? 1 : 0;
        if ((delim_count == index) && delim1 < 0)
            delim1 = a+1;
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