#include <Arduino.h>
#include "smt50.h"

void smt50_init() {
    pinMode(SMT_50_GROUND_TEMP_PIN, INPUT);
    pinMode(SMT_50_GROUND_MOIST_PIN, INPUT);
    pinMode(SMT_50_GROUND_ENABLE_PIN, OUTPUT);
    digitalWrite(SMT_50_GROUND_ENABLE_PIN, HIGH);
}

SMT50Reading smt50_read() {
    // Enable sensor.
    digitalWrite(SMT_50_GROUND_ENABLE_PIN, LOW);

    // Wait until sensor has stabilized, takes max. 300 ms according to datasheet
    delay(SMT_50_GROUND_ENABLE_DELAY);

    float tempAnalog = analogRead(SMT_50_GROUND_TEMP_PIN);
    float moistAnalog = analogRead(SMT_50_GROUND_MOIST_PIN);
    
    // Disable sensor.
    digitalWrite(SMT_50_GROUND_ENABLE_PIN, HIGH);

    float voltageRatio = SMT_50_REFERENCE_VOLTAGE / 1024;
  
    float tempVoltage = tempAnalog * voltageRatio;
    float temp = (tempVoltage - 0.5) * 100;

    float moistVoltage = moistAnalog * voltageRatio;
    float moist = moistVoltage * 50 / 3;

    SMT50Reading reading;
    reading.temperature = temp;
    reading.moisture = moist;

    return reading;
}
