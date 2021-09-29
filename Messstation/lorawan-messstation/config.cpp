#include <Arduino.h>
#include "config.h"
#include "secret-config.h"

static const u1_t PROGMEM APPEUI[8] = { APPEUI_BYTES };
static const u1_t PROGMEM DEVEUI[8] = { DEVEUI_BYTES };
static const u1_t PROGMEM APPKEY[16] = { APPKEY_BYTES };

void os_getArtEui (u1_t* buf) {
    memcpy_P(buf, APPEUI, 8);
}

void os_getDevEui (u1_t* buf) {
    memcpy_P(buf, DEVEUI, 8);
}

void os_getDevKey (u1_t* buf) {
    memcpy_P(buf, APPKEY, 16);
}

// Pin mapping for Adafruit Feather M0 LoRa.
const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 6, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 8, // LBT cal for the Adafruit Feather M0 LoRa, in dB.
    .spi_freq = 8000000,
};
