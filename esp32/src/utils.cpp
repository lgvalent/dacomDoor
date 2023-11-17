#ifndef APP_UTILS
#define APP_UTILS
#include "commons.cpp"
#include <time.h>
#include <cstring>

class Utils
{
public:

  static String uidBytesToString(byte *array, byte n)
  {
    char uid[64] = {0};
    int i, k;
    byte x;

    for (k = i = 0; i < n; i++)
    {
      x = array[i];

      if (x > 15)
      {
        uid[k] = (x >> 4) + '0';
        if (uid[k] > '9')
          uid[k] += 7;
        k++;

        uid[k] = (x & 0x0f) + '0';
        if (uid[k] > '9')
          uid[k] += 7;
        k++;
      }
      else
      {
        uid[k] = x + '0';
        if (uid[k] > '9')
          uid[k] += 7;
        k++;
      }

      uid[k] = ':';
      k++;
    }
    uid[k - 1] = '\0';

    return String(uid);
  }

  static String currentDatetime()
  {
    struct tm timeNow;
    getLocalTime(&timeNow);
    char buffer[40];
    strftime(buffer, 40, "%G-%m-%dT%H:%M:%S", &timeNow);
    String dateAndHour(buffer);
    return dateAndHour;
  }

  static String currentHour()
  {
    struct tm timeNow;
    getLocalTime(&timeNow);
    char buffer[40];
    strftime(buffer, 40, "%H:%M:%S", &timeNow);
    String hour(buffer);
    return hour;
  }

  static String currentWeekDay()
  {
    struct tm timeNow;
    getLocalTime(&timeNow);
    char buffer[10];
    strftime(buffer, 10, "%A", &timeNow);
    String weekday(strupr(buffer));
    // Until issue #11 is resolved, keep the following.
    if(weekday.compareTo("TUESDAY")){ 
      return "THUESDAY";
    }
    return weekday;
  }
};

#endif
