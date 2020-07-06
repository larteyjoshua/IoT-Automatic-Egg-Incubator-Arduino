#include "RTClib.h"
RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
void setup () 
{
  Serial.begin(9600);
  if (! rtc.begin()) 
  {
    Serial.print("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) 
  {
    Serial.print("RTC is NOT running!");
  }
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));// to set the time manualy 
}
void loop () 
{
    DateTime now = rtc.now();
    Serial.print("TIME");
    Serial.print(" ");
    Serial.print(now.hour());
    Serial.print(':');
    Serial.print(now.minute());
    Serial.print(':');
    Serial.print(now.second());
    Serial.print("  ");
    delay(3000);

   
    Serial.print("DATE");
    Serial.print(" ");
    //Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    //Serial.print(" ");
    Serial.print(now.day());
    Serial.print('/');
    Serial.print(now.month());
    Serial.print('/');
    Serial.print(now.year());
    Serial.print("  ");   
}
