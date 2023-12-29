#ifndef TASK_H
#define TASK_H

#include <Arduino.h>
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

#endif