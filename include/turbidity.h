#ifndef TURBIDITY_H
#define TURBIDITY_H

#include "GravityTDS.h" 

class Turbidity {
private:
    GravityTDS tds;
public:
    Turbidity();
    void begin(int pinTDS);
    void begin(int pinTDS, int pinTemperature);
    void updateData();
    float fetchData();
};

#endif