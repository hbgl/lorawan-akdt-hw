#pragma once

#include <lmic.h>
#include <hal/hal.h>

extern const lmic_pinmap lmic_pins;

void os_getArtEui (u1_t* buf);
void os_getDevEui (u1_t* buf);
void os_getDevKey (u1_t* buf);
