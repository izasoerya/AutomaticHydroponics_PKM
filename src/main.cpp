#include <Arduino.h>
#include <EEPROM.h>
#include "rtc.h"
#include "turbidity.h"
#include "modem.h"
#include "models.h"
#include <WiFi.h>
#include "scheduler.h"

#include <WiFiClient.h>
#define BLYNK_TEMPLATE_ID       "TMPL6CmepA5vq"
#define BLYNK_TEMPLATE_NAME     "testLED"
#define BLYNK_AUTH_TOKEN        "yDlKAbJH59XMJbjPn55ePrnCDKGiMm9E"
#include <BlynkSimpleEsp32.h>

#define PIN_WATER_PUMP  0
#define PIN_LED_ROAD    2
#define PIN_FULL_TANK   27
#define PIN_A_PUPUK     25
#define PIN_B_PUPUK     26
#define PIN_AB_PUPUK    33

const char* ssid = "4G-UFI-0345";
const char* password = "336409000";

RTC rtc;
Turbidity tds;
myTime date;
ConnectWIFI mywifi;
drivenState state = MANUAL;

void setup () {
    const int pinTDS = 32;
    Serial.begin(115200);
    rtc.begin();
    tds.begin(pinTDS);
    mywifi.begin(ssid, password);  
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

    pinMode(PIN_WATER_PUMP, OUTPUT);
    pinMode(PIN_LED_ROAD,   OUTPUT);
    pinMode(PIN_A_PUPUK,    OUTPUT);
    pinMode(PIN_B_PUPUK,    OUTPUT);
    pinMode(PIN_AB_PUPUK,   OUTPUT);
    pinMode(PIN_FULL_TANK,   INPUT);
    digitalWrite(PIN_WATER_PUMP,    !LOW);
    digitalWrite(PIN_LED_ROAD,      !LOW);
}

BLYNK_WRITE(V6)     // Fill AB MIX
{
    int pinValue = param.asInt(); 
    if(!digitalRead(PIN_FULL_TANK)) {
        if (state == MANUAL) analogWrite(PIN_A_PUPUK, 100);
        if (state == MANUAL) analogWrite(PIN_B_PUPUK, 100);
        if (state == MANUAL) analogWrite(PIN_AB_PUPUK, 0);
    }
}

BLYNK_WRITE(V5)     // Release AB MIX
{
    int pinValue = param.asInt(); 
    if (state == MANUAL && pinValue == 1) analogWrite(PIN_AB_PUPUK, 100);
    else if (state == MANUAL && pinValue == 0) analogWrite(PIN_AB_PUPUK, 0);

    if (state == MANUAL) analogWrite(PIN_A_PUPUK, 0);
    if (state == MANUAL) analogWrite(PIN_B_PUPUK, 0);

}

BLYNK_WRITE(V7)     // LED LAMP
{
    int pinValue = param.asInt(); 
    if (state == MANUAL) digitalWrite(PIN_LED_ROAD, !pinValue);
}

BLYNK_WRITE(V8)     // WATER PUMP
{
    int pinValue = param.asInt(); 
    if (state == MANUAL) digitalWrite(PIN_WATER_PUMP, !pinValue); 
}

BLYNK_WRITE(V9)     // DRIVE STATE
{
    int pinValue = param.asInt(); 
    if (pinValue) state = AUTOMATIC;
    else {
        state = MANUAL;
        digitalWrite(PIN_LED_ROAD, !LOW);
        digitalWrite(PIN_WATER_PUMP,!LOW);
        analogWrite(PIN_A_PUPUK, 0);
        analogWrite(PIN_B_PUPUK, 0);
    }
}

void doWorkNow(float ppmValue);

void fetchRTC () {
    rtc.fetchData(date);
}

void fetchTDS () {
    float ppmValue;
    tds.updateData();
    ppmValue = tds.fetchData();
    doWorkNow(ppmValue);
}

void doWorkNow(float ppmValue) {
    char buffer[255];
    snprintf(buffer, sizeof(buffer),"PPM value is : %.2f, Time : %02d:%02d:%02d", ppmValue, date.hour, date.minute, date.second);
    Serial.println((String)buffer);
    
    Blynk.virtualWrite(V0, !digitalRead(PIN_WATER_PUMP));
    Blynk.virtualWrite(V2, !digitalRead(PIN_LED_ROAD));
    Blynk.virtualWrite(V3, ppmValue);
    Blynk.virtualWrite(V4, digitalRead(PIN_FULL_TANK));
}

void flagCondition () {
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

    if (digitalRead(PIN_FULL_TANK)) 
    {
        analogWrite(PIN_AB_PUPUK, 100);
        analogWrite(PIN_A_PUPUK, 0);
        analogWrite(PIN_B_PUPUK, 0);

        Serial.println("tai");
        delay(20000);
    }
    else 
    {
        Serial.println("not tai");
        analogWrite(PIN_AB_PUPUK, 0);
        analogWrite(PIN_A_PUPUK, 100);
        analogWrite(PIN_B_PUPUK, 100);
    }
}

TaskScheduler fetchDataRTC  (1, "RTC", 1000, fetchRTC);
TaskScheduler fetchDataTDS  (2, "TDS", 1000, fetchTDS);

void loop () {
    fetchDataRTC.runTask();
    fetchDataTDS.runTask();
    if (state == AUTOMATIC) flagCondition();
    if (digitalRead(PIN_FULL_TANK)) {       // Hard Shutdown Pump Fertilizer
        analogWrite(PIN_A_PUPUK, 0);
        analogWrite(PIN_B_PUPUK, 0);
    }
    Blynk.run();
}

