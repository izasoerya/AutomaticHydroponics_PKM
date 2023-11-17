#ifndef MODEM_H
#define MODEM_H

#include <WiFi.h>

class ConnectWIFI{
private:
    
public:
    ConnectWIFI();
    void begin();
    void scanNetwork();
};

#endif