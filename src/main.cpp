#include <Arduino.h>
#include <EEPROM.h>
#include "rtc.h"
#include "turbidity.h"
#include "modem.h"
#include "models.h"
#include "task.h"
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

String username = "ihza.surya0302";
String password = "Makanmieayam4kali";

RTC rtc;
Turbidity tds;
myTime date;
InternetUGM inet(username, password);

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
