#pragma once

#include <lmic.h>

#if APP_DEBUG

void app_debug_opmode();

void app_debug_onEvent(ev_t ev);

void app_debug_printHex2(unsigned v);

#endif;
