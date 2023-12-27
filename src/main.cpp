#include <Arduino.h>
#include <EEPROM.h>
#include "rtc.h"
#include "turbidity.h"
#include "modem.h"
#include "models.h"
#include <WiFi.h>
#include <esp_system.h>
#include "scheduler.h"
#include <internet.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include <WiFiClient.h>
#define BLYNK_TEMPLATE_ID "TMPL6CmepA5vq"
#define BLYNK_TEMPLATE_NAME "testLED"
#define BLYNK_AUTH_TOKEN "yDlKAbJH59XMJbjPn55ePrnCDKGiMm9E"
#include <BlynkSimpleEsp32.h>

#define PIN_WATER_PUMP 0
#define PIN_LED_ROAD 2
#define PIN_FULL_TANK 27 // Active Low
#define PIN_A_PUPUK 25
#define PIN_B_PUPUK 26
#define PIN_AB_PUPUK 33

String username = "ihza.surya0302";
String password = "Makanmieayam4kali";

RTC rtc;
Turbidity tds;
myTime date;
InternetUGM inet(username, password);
drivenState state = MANUAL;
pumpState currentPumpState = AB_ISOFF;

void setup()
{
    Serial.begin(115200);
    rtc.begin();
    tds.begin();

    WiFiClientSecure wifi;
    HTTPClient http;
    inet.begin(wifi, http);
    Blynk.begin(BLYNK_AUTH_TOKEN, "UGM-Hotspot", "");

    pinMode(PIN_WATER_PUMP, OUTPUT);
    pinMode(PIN_LED_ROAD, OUTPUT);
    pinMode(PIN_A_PUPUK, OUTPUT);
    pinMode(PIN_B_PUPUK, OUTPUT);
    pinMode(PIN_AB_PUPUK, OUTPUT);
    pinMode(PIN_FULL_TANK, INPUT);
    digitalWrite(PIN_WATER_PUMP, !LOW);
    digitalWrite(PIN_LED_ROAD, !LOW);
    analogWrite(PIN_A_PUPUK, 0);
    analogWrite(PIN_B_PUPUK, 0);
    analogWrite(PIN_AB_PUPUK, 0);
}

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
    if (state == MANUAL)
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

float ppmValue;
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

TaskScheduler fetchDataTDS(1, "TDS", 1000, fetchTDS);
TaskScheduler sendDataBlynk(2, "BLYNK", 1000, doWorkNow);

void loop()
{
    fetchDataTDS.runTask();
    sendDataBlynk.runTask();
    if (state == AUTOMATIC)
        flagCondition();
    if (!digitalRead(PIN_FULL_TANK))
    { // Hard Shutdown Pump Fertilizer
        analogWrite(PIN_A_PUPUK, 0);
        analogWrite(PIN_B_PUPUK, 0);
    }
    if (date.hour == 0 && date.minute == 5 && (date.second == 50))
    {
        esp_restart();
    }
    Blynk.run();
}
