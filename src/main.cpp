#include <Arduino.h>
#include "models.h"
#include "taskblynk.h"
#include "taskstate.h"
#include <esp_system.h>
#include "scheduler.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <internet.h>

#define BLYNK_TEMPLATE_ID "TMPL6CmepA5vq"
#define BLYNK_TEMPLATE_NAME "testLED"
#define BLYNK_AUTH_TOKEN "yDlKAbJH59XMJbjPn55ePrnCDKGiMm9E"
#include <BlynkSimpleEsp32.h>

String username = "patrickrinoparulianhutahaean";
String password = "Patrick6a";
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
    digitalWrite(PIN_WATER_PUMP, !LOW); // Active Low
    digitalWrite(PIN_LED_ROAD, !LOW);   // Active Low
    analogWrite(PIN_A_PUPUK, 0);
    analogWrite(PIN_B_PUPUK, 0);
    analogWrite(PIN_AB_PUPUK, 0);
}

TaskScheduler fetchDataTDS(1, "TDS", 1000, fetchTDS);
TaskScheduler sendDataBlynk(2, "BLYNK", 1000, doWorkNow);

void loop()
{
    fetchDataTDS.runTask();
    sendDataBlynk.runTask();
    if (state == AUTOMATIC)
        flagCondition();
    if (!digitalRead(PIN_FULL_TANK)) // Active Low
        hardShutdown(PIN_A_PUPUK, PIN_B_PUPUK);
    if (date.hour == 0 && date.minute == 5 && (date.second == 50))
        esp_restart();
    if (ppmValue > 400)
        digitalWrite(PIN_WATER_PUMP, !LOW);
    Blynk.run();
}
