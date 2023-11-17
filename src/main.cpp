#include <Arduino.h>
#include <EEPROM.h>
#include "rtc.h"
#include "turbidity.h"
#include "modem.h"
#include <WiFi.h>

#include <WiFiClient.h>
#define BLYNK_TEMPLATE_ID       "TMPL6CmepA5vq"
#define BLYNK_TEMPLATE_NAME     "testLED"
#define BLYNK_AUTH_TOKEN        "yDlKAbJH59XMJbjPn55ePrnCDKGiMm9E"
#include <BlynkSimpleEsp32.h>

#define PIN_WATER_PUMP  0
#define PIN_LED_ROAD    2
#define PIN_FULL_TANK   22
#define PIN_A_PUPUK     25
#define PIN_B_PUPUK     26
#define PIN_AB_PUPUK    33

const char* ssid = "arkadewanalog";
const char* password = "arkapresiden2024";

RTC rtc;
Turbidity tds;
myTime date;
ConnectWIFI mywifi;

void setup() 
{
    const int pinTDS = 32;

    Serial.begin(115200);
    rtc.begin();
    tds.begin(pinTDS);
    mywifi.begin();  
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

    pinMode(PIN_WATER_PUMP ,  OUTPUT);
    pinMode(PIN_LED_ROAD ,  OUTPUT);
}

unsigned long previousMillis1 = 0;      // Store the last time a task was executed
unsigned long previousMillis2 = 0;      // Store the last time a task was executed
unsigned long previousMillis3 = 0;      // Store the last time a task was executed
const unsigned long interval1 = 1000;   // Task 1 interval in milliseconds
const unsigned long interval2 = 1000;   // Task 2 interval in milliseconds
const unsigned long interval3 = 1000;   // Task 3 interval in milliseconds

BLYNK_WRITE(V6)     // Fill AB MIX
{
    int pinValue = param.asInt(); 
    digitalWrite(PIN_A_PUPUK, pinValue);
    digitalWrite(PIN_B_PUPUK, pinValue);
}

BLYNK_WRITE(V5)     // Release AB MIX
{
    int pinValue = param.asInt(); 
    digitalWrite(PIN_AB_PUPUK, pinValue);
}

float ppmValue;
char buffer[255];
unsigned long currentMillis;
bool state = false;

void loop() 
{
    memset(buffer, 0, sizeof(buffer));
    currentMillis = millis();  // Get the current time
    tds.updateData();

    if (currentMillis - previousMillis1 >= interval1) {
        rtc.fetchData(date);
        previousMillis1 = currentMillis;  
    }

    if (currentMillis - previousMillis2 >= interval2) {
        ppmValue = tds.fetchData();
        previousMillis2 = currentMillis;  
    }

    if (currentMillis - previousMillis3 >= interval3) {
        snprintf(buffer, sizeof(buffer),"PPM value is : %.2f, Time : %02d:%02d:%02d", ppmValue, date.hour, date.minute, date.second);
        Serial.println((String)buffer);
        
        Blynk.virtualWrite(V0, digitalRead(PIN_WATER_PUMP));
        Blynk.virtualWrite(V2, digitalRead(PIN_LED_ROAD));
        Blynk.virtualWrite(V3, ppmValue);
        // Blynk.virtualWrite(V4, digitalRead(PIN_FULL_TANK));

        previousMillis3 = currentMillis;  // Save the last time this task was executed
    }

    if (date.hour > 8 && date.hour < 13)
    {
        digitalWrite(PIN_WATER_PUMP, HIGH);
    }
    else digitalWrite(PIN_WATER_PUMP, LOW);
    
    Blynk.run();

}

