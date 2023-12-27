#ifndef MODELS_H
#define MODELS_H

enum drivenState
{
    AUTOMATIC = 0,
    MANUAL = 1
};
drivenState state = drivenState::MANUAL;

enum fertilizerState
{
    AB_FULL = 0,
    AB_PARTIAL = 1,
    AB_EMPTY = 2
};

enum pumpState
{
    AB_ISON = 0,
    AB_ISOFF = 1
};
pumpState currentPumpState = pumpState::AB_ISON;

#define PIN_WATER_PUMP 0
#define PIN_LED_ROAD 2
#define PIN_FULL_TANK 27 // Active Low
#define PIN_A_PUPUK 25
#define PIN_B_PUPUK 26
#define PIN_AB_PUPUK 33

#endif