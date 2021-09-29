#pragma once

/**
 * SMT50 datasheet http://www.truebner.de/download/Anleitung_SMT50.pdf
 */

// Configuration
#ifndef SMT_50_GROUND_TEMP_PIN
    #define SMT_50_GROUND_TEMP_PIN A0
#endif

#ifndef SMT_50_GROUND_MOIST_PIN
    #define SMT_50_GROUND_MOIST_PIN A1
#endif

#ifndef SMT_50_GROUND_ENABLE_PIN
    #define SMT_50_GROUND_ENABLE_PIN A2
#endif

#ifndef SMT_50_GROUND_ENABLE_DELAY
    #define SMT_50_GROUND_ENABLE_DELAY 350
#endif

#ifndef SMT_50_REFERENCE_VOLTAGE
    #define SMT_50_REFERENCE_VOLTAGE 3.3f
#endif

class SMT50Reading {
public:
    float temperature;
    float moisture;
};

void smt50_init();

SMT50Reading smt50_read();
