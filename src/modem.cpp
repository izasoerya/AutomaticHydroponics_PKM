#include "modem.h"

ConnectWIFI::ConnectWIFI(){}

void ConnectWIFI::begin()
{
    const char* ssid = "arkadewanalog";
    const char* password = "arkapresiden2024";
    WiFi.begin(ssid, password, 6);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) 
    { 
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected!");
}

// void ConnectWIFI::scanNetwork()
// {
//     int n = WiFi.scanNetworks();
//     Serial.println("Scan done");
//     if (n == 0) 
//     {
//         Serial.println("no networks found");
//         return;
//     }
// }


