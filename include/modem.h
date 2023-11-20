#ifndef MODEM_H
#define MODEM_H

#include <WiFi.h>

class ConnectWIFI{
private:
    const char* ssid;
    const char* password;
public:
    ConnectWIFI(const char* ssid, const char* password);
    void begin();
};

#endif