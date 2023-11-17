#include <Arduino.h>
#include "turbidity.h"

Turbidity::Turbidity():tds(GravityTDS()){}

void Turbidity::begin(int pinTDS)
{
    tds.setPin(pinTDS);
    tds.setAref(3.3);       //reference voltage on ADC, default 5.0V on Arduino UNO
    tds.setAdcRange(4096);  //1024 for 10bit ADC;4096 for 12bit ADC
    tds.begin();            //initialization
}

void Turbidity::begin(int pinTDS, int pinTemperature)
{
    tds.setPin(pinTDS);
    tds.setAref(3.3);       //reference voltage on ADC, default 5.0V on Arduino UNO
    tds.setAdcRange(4096);  //1024 for 10bit ADC;4096 for 12bit ADC
    tds.begin();            //initialization
}

void Turbidity::updateData()
{
    tds.update();
}

float Turbidity::fetchData()
{
    float temperature = 25;
    tds.setTemperature(temperature);
    return tds.getTdsValue();
}

