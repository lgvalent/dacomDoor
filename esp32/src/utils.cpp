#ifndef APP_UTILS
#define APP_UTILS
#include <unordered_map>
#include <ctime>
#include <Arduino.h>

class Utils
{
public:
  static time_t now(){
    return time(NULL);
  }

  static String weekDay(uint8_t day)
  {
    switch (day)
    {
    case 0:
      return "SUNDAY";
    case 1:
      return "MONDAY";
    case 2:
      return "TUESDAY";
    case 3:
      return "WEDNESDAY";
    case 4:
      return "THURSDAY";
    case 5:
      return "FRIDAY";
    case 6:
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

  static u_int32_t uidBytesToInt(byte *array, byte n)
  {
    u_int32_t uid = 0;
    for (int i = 0; i < n; i++)
    {
      uid = (uid << 8) + array[i];
    }

    return uid;
  }

  static String padZero(int x) { return x > 9 ? String(x) : "0" + String(x); }

  static String datetimeToString(time_t time)
  {
    struct tm *ti = localtime(&time);  
    String dStr = Utils::padZero(ti->tm_mday);
    String moStr = Utils::padZero(ti->tm_mon);
    String hStr = Utils::padZero(ti->tm_hour);
    String mStr = Utils::padZero(ti->tm_min);
    String sStr = Utils::padZero(ti->tm_sec);

    String dateAndHour = String(ti->tm_year) + "-" + moStr + "-" + dStr + "T" + hStr + ":" +
                         mStr + ":" + sStr;

    return dateAndHour;
  }

  static time_t stringToDatetime(String time)
  {

    time_t result = 0;
    /** TODO */
    return result;
  }

  static String currentDatetime()
  {
    return datetimeToString(Utils::now());
  }

  static String timeToString(time_t time)
  {
    struct tm *ti = localtime(&time);  

    String hStr = Utils::padZero(ti->tm_hour);
    String mStr = Utils::padZero(ti->tm_min);
    String sStr = Utils::padZero(ti->tm_sec);

    return hStr + ":" + mStr + ":" + sStr;
  }

  static String currentHour()
  {
    return timeToString(Utils::now());
  }

  static String currentWeekDay()
  {
    time_t time = Utils::now();
    struct tm *ti = localtime(&time);  
    return Utils::weekDay(ti->tm_wday);
  }

  template <typename T>  static T findEnumByValue(std::unordered_map<T, String>& enumMap, const String& value) {
    for (const auto& pair : enumMap) {
        if (pair.second == value) {
            return pair.first;
        }
    }
    //TODO Show in log this exception
    throw std::exception();
  }
};

#endif
