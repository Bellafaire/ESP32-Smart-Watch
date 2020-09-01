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

/* Calender just pulls and reformats the string provided from the BLE companion app
   then puts those values into a Scroll Window for viewing*/

void viewCalendar() {
  //default state is not connected
  Window w = Window(0, 14, 160, 100, true);
  String calendarData;

  //String getValue(String data, char separator, int index)

  //if we are connected then we can
  if (connected) {

    //get calendar data
    calendarData = sendBLE("/calender", true);

    //count the number of new lines, we'll need this to parse out the data
    int lineCount = 0;
    for (int a = 0; a < calendarData.length(); a++) {
      if (calendarData[a] == '\n') {
        lineCount++;
      }
    }

    //each line contains an event so we format the data on a per-line basis
    //calendar data is recieved in format "title;description;startDate;startTime;endTime;eventLocation;"
    for (int a = 0; a < lineCount; a++) {
      String currentLine = getValue(calendarData, '\n', a);
      w.print(
        getValue(currentLine, FIELD_SEPARATOR, 0)      //title
        + "\n_"
        + getValue(currentLine, FIELD_SEPARATOR, 3)    //start time
        + "-"
        + getValue(currentLine, FIELD_SEPARATOR, 4)    //end time
        + " "
        + getValue(currentLine, FIELD_SEPARATOR, 2)    //start date
        + "_\n"
      );
    }

    w.focus();
  } else {
    calendarData = "Not Connected to Android Device";
    w.println(String(calendarData));
  }


}
