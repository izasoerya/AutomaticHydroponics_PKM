#include "modem.h"

ConnectWIFI::ConnectWIFI(const char* ssid, const char* password){
    this->ssid = ssid;
    this->password = password;
}

void ConnectWIFI::begin()
{
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


