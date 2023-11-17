#include <Arduino.h>
#include "rtc.h"

RTC::RTC():myRTC(RTC_DS3231()){}

void RTC::begin()
{
    Wire.begin();
    myRTC.begin();
    // myRTC.adjust(DateTime(2023, 11, 17, 2, 22, 15));
}

void RTC::fetchData(myTime &time)
{
    DateTime now = myRTC.now();
    time.day    = now.day(), DEC;
    time.month  = now.month(), DEC;
    time.year   = now.year(), DEC;
    time.second = now.second(), DEC;
    time.minute = now.minute(), DEC;
    time.hour   = now.hour(), DEC;
}

