#include <Arduino.h>
#include "battery.h"

float voltages[] = { 4.2, 4.15, 4.1, 4.05, 4.0, 3.95, 3.9, 3.85, 3.8, 3.75, 3.7, 3.65, 3.6, 3.54, 3.5, 3.44, 3.4, 3.34, 3.3, 3.24, 3.19 };
int percentages[] = { 100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 1, 1 };

void battery_init() {
    // Assert that the arrays have the same length. Typos happen.
    // TODO: Should use std::extent, but it is not available in this compiler version.
    static_assert((sizeof(voltages) / sizeof(float)) == (sizeof(percentages) / sizeof(int)));

    pinMode(BATTERY_PIN, INPUT);
}

BatteryReading battery_read() {
    float voltage = analogRead(BATTERY_PIN) * (BATTERY_REFERENCE_VOLTAGE / 1024);
    int percentage = 0;

    size_t length = sizeof(voltages) / sizeof(float);
    for (size_t i = 0; i < length; i++) {
        if (voltage >= voltages[i]) {
            percentage = percentages[i];
            break;
        }
    }
    
    BatteryReading reading;
    reading.voltage = voltage;
    reading.percentage = percentage;
    return reading;
}
