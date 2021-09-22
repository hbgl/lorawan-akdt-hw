#pragma once

#ifndef BATTERY_PIN
    #define BATTERY_PIN A7
#endif

#ifndef BATTERY_REFERENCE_VOLTAGE
    #define BATTERY_REFERENCE_VOLTAGE 3.3f
#endif

class BatteryReading {
public:
    float voltage;
    int percentage;
};

void battery_init();

BatteryReading battery_read();
