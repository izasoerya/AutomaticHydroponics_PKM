#ifndef TASK_H
#define TASK_H

#include <Arduino.h>
#define BLYNK_TEMPLATE_ID "TMPL6CmepA5vq"
#define BLYNK_TEMPLATE_NAME "testLED"
#define BLYNK_AUTH_TOKEN "yDlKAbJH59XMJbjPn55ePrnCDKGiMm9E"
#include <BlynkSimpleEsp32.h>
#include "models.h"

BLYNK_WRITE(V6) // Fill AB MIX
{
    int pinValue = param.asInt();
    if (digitalRead(PIN_FULL_TANK) && pinValue == 1 && currentPumpState == AB_ISOFF)
    {
        analogWrite(PIN_A_PUPUK, 200);
        analogWrite(PIN_B_PUPUK, 200);
        analogWrite(PIN_AB_PUPUK, 0);
    }
    else if (pinValue == 0 || !digitalRead(PIN_FULL_TANK))
    { // if already full stop
        analogWrite(PIN_A_PUPUK, 0);
        analogWrite(PIN_B_PUPUK, 0);
    }
}

BLYNK_WRITE(V5) // Release AB MIX
{
    int pinValue = param.asInt();
    if (state == MANUAL && pinValue == 1)
    {
        analogWrite(PIN_AB_PUPUK, 200);
        currentPumpState = AB_ISON;
    }
    else if (state == MANUAL && pinValue == 0)
    {
        analogWrite(PIN_AB_PUPUK, 0);
        currentPumpState = AB_ISOFF;
    }
    analogWrite(PIN_A_PUPUK, 0);
    analogWrite(PIN_B_PUPUK, 0);
}

BLYNK_WRITE(V7) // LED LAMP
{
    int pinValue = param.asInt();
    if (state == MANUAL)
        digitalWrite(PIN_LED_ROAD, !pinValue);
}

BLYNK_WRITE(V8) // WATER PUMP
{
    int pinValue = param.asInt();
    if (state == MANUAL && ppmValue < 400)
        digitalWrite(PIN_WATER_PUMP, !pinValue);
}

BLYNK_WRITE(V9) // DRIVE STATE
{
    int pinValue = param.asInt();
    if (pinValue)
        state = AUTOMATIC;
    else
    {
        state = MANUAL;
        digitalWrite(PIN_LED_ROAD, !LOW);
        digitalWrite(PIN_WATER_PUMP, !LOW);
    }
}

#include <EEPROM.h>
#include "rtc.h"
#include "turbidity.h"

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

#endif