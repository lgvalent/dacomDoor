#ifndef APP_UTILS
#define APP_UTILS
#include "commons.cpp"

class Utils
{
public:
  static String weekDay(int day)
  {
    switch (day)
    {
    case 1:
      return "SUNDAY";
    case 2:
      return "MONDAY";
    case 3:
      return "THUESDAY"; // Fix typo?
    case 4:
      return "WEDNESDAY";
    case 5:
      return "THURSDAY";
    case 6:
      return "FRIDAY";
    case 7:
      return "SATURDAY";
    }
    return "SUNDAY";
  }

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

  static String padZero(int x) { return x > 9 ? String(x) : "0" + String(x); }

  static String currentDatetime()
  {
    time_t t = now();
    int h = hour(t);
    int m = minute(t);
    int s = second(t);
    int d = day(t);
    int mo = month(t);
    int y = year(t);

    String dStr = Utils::padZero(d);
    String moStr = Utils::padZero(mo);
    String hStr = Utils::padZero(h);
    String mStr = Utils::padZero(m);
    String sStr = Utils::padZero(s);

    String dateAndHour = String(y) + "-" + moStr + "-" + dStr + "T" + hStr + ":" +
                         mStr + ":" + sStr;

    return dateAndHour;
  }

  static String currentHour()
  {
    time_t t = now();
    int h = hour(t);
    int m = minute(t);
    int s = second(t);

    String hStr = Utils::padZero(h);
    String mStr = Utils::padZero(m);
    String sStr = Utils::padZero(s);

    String hour = hStr + ":" + mStr + ":" + sStr;

    return hour;
  }

  static String currentWeekDay()
  {
    return Utils::weekDay(weekday(now()));
  }
};

#endif
