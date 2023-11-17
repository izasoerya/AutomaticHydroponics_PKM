#ifndef RTC_H
#define RTC_H

#include "RTClib.h"
#include <Wire.h>
#include <SPI.h>

struct myTime {
    uint8_t day, month, year;
    uint8_t second, minute, hour;
};

class RTC {
private:
    RTC_DS3231 myRTC;
    bool century;
    bool h12, pm;

public:
    RTC();
    void begin();
    void fetchData(myTime &time);
};

#endif
