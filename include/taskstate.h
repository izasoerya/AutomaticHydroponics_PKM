#ifndef MODEL_H
#define MODEL_H

// Include necessary headers here
#include <EEPROM.h>
#include "rtc.h"
#include "turbidity.h"
#include "models.h"
#include <BlynkSimpleEsp32.h>

RTC rtc;
Turbidity tds;
myTime date;

void fetchTDS()
{
    rtc.fetchData(date);
    tds.updateData();
    ppmValue = tds.fetchData();
}

void doWorkNow()
{
    char buffer[255];
    snprintf(buffer, sizeof(buffer), "PPM value is : %.2f, Time : %02d:%02d:%02d", ppmValue, date.hour, date.minute, date.second);
    Serial.println((String)buffer);

    Blynk.virtualWrite(V0, !digitalRead(PIN_WATER_PUMP));
    Blynk.virtualWrite(V2, !digitalRead(PIN_LED_ROAD));
    Blynk.virtualWrite(V3, ppmValue);
    Blynk.virtualWrite(V4, !digitalRead(PIN_FULL_TANK));
}

void flagCondition()
{
    if (date.hour > 8 && date.hour < 13)
    {
        digitalWrite(PIN_WATER_PUMP, !HIGH);
        digitalWrite(PIN_LED_ROAD, !HIGH);
    }
    else if (date.hour > 15 && date.hour < 20)
    {
        digitalWrite(PIN_WATER_PUMP, !HIGH);
        digitalWrite(PIN_LED_ROAD, !HIGH);
    }
}

void hardShutdown(int pinA, int pinB)
{
    analogWrite(pinA, 0);
    analogWrite(pinB, 0);
}

#endif // MODEL_H
