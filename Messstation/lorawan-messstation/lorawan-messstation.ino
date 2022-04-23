/*******************************************************************************************************************************
 * Projekt: LoRaWan im Stadtwald
 * Kooperation: Forstamt Heilbronn    (Immanuel Schmutz: immanuel.schmutz@heilbronn.de)
 *              Stadt Heilbronn       (Louis Krasniqi: Louis.Krasniqi@heilbronn.de, Thomas Laue: Thomas.Laue@heilbronn.de) 
 *              Hochschule Heilbronn  (Prof. Dr. rer. nat. Nicole Ondrusch: nicole.ondrusch@hs-heilbronn.de)
 * 
 * Gruppenmitglieder: Lukas Ertl lertl@stud.hs-heilbronn.de
 *                    Jeffrey Maier jemaier@stud.hs-heilbronn.de
 *                    Max Stricker stricker@stud.hs-heilbronn.de
 *                    Kenan Ekmekci kekmekci@stud.hs-heilbronn.de
 *                    Markus Pfeffermann mpfeffer@stud.hs-heilbronn.de
 * 
 * Autoren: Markus Pfeffermann, Kenan Ekmekci
 * 
 * Dieses Sketch ermittelt mittels Sensoren die an einem Adafruit Feather M0 angeschlossen sind verschiedene Parameter                   
 * und übermittelt diese mittels LoRaWAN über ein Gateway an das TTN.
 * 
 * Sensoren:  Veml7700  - Lux Sensor zur Messung des Lichts/der Helligkeit
 *            BME280    - Lufttemperatur-, Luftfeuchtigkeit- und Luftdrucksensor
 *            SMT50     - Unterirdischer Temperatur- und Feuchtigkeitssensor
 * 
 * Version 2.0.0
 * 
 * Changelog:
 * 
 * v2.0.0 (22.09.21)
 * - Angepasst an TTN V3, sendet ACK Uplinks für MAC Commands.
 * 
 * v1.0.0 (08.02.2021)
 * - Initialer Release für TTN V2.
 * 
 ******************************************************************************************************************************/
 
/*******************************************************************************
 * The Things Network - Sensor Data Example
 *
 * Example of sending a valid LoRaWAN packet with DHT22 temperature and
 * humidity data to The Things Networ using a Feather M0 LoRa.
 *
 * Learn Guide: https://learn.adafruit.com/the-things-network-for-feather
 *
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 * Copyright (c) 2018 Brent Rubell, Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *******************************************************************************/

// Define at the top so all includes see it.
#define APP_DEBUG false

#include <lmic.h>
#include <hal/hal.h>

#include "Adafruit_VEML7700.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SleepyDog.h>

#include "config.h"
#include "debug.h"
#include "smt50.h"
#include "battery.h"
#include "payload.h"

/**
 * App configuration.
 */
#define APP_BLINK_LED true
#define APP_DISABLE_LINK_CHECK_VALIDATION true

// Use regular delay while debugging to be able to print
// to the serial port without losing serial connection.
#define APP_SLEEP_DELAY false
#define APP_SLEEP_DELAY_MS 60000

// Use watchdog in production to save power.
#define APP_SLEEP_WATCHDOG true
#define APP_SLEEP_WATCHDOG_MS 16000
#define APP_SLEEP_WATCHDOG_MULTIPLIER 112

// Reset config
#define RESET_PIN A5
#define RESET_ON_FATAL_ERROR true
#define RESET_BLINK_COUNT 10

// BME280 configuration.
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)
#define BME_I2C_ADDR 0x76

/** 
 * Globals 
 */
auto veml = Adafruit_VEML7700();
auto bme = Adafruit_BME280();
auto payload = Payload();
static osjob_t sendjob;

void blink_error() {
#if APP_BLINK_LED
    // Blink short, short, short if the application encounters an unrecoverable error.
    digitalWrite(LED_BUILTIN, LOW);
    delay(3000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
#endif
}

int error_handler() {
#if APP_DEBUG
    Serial.println("Fatal error encountered.");
#endif
    
#if RESET_ON_FATAL_ERROR
    // Blink a couple of times then reset.
    for (int i = RESET_BLINK_COUNT; i > 0; i--) {
    #if APP_DEBUG
        Serial.print(i);
        Serial.println(" blinks before reset.");
    #endif
        blink_error();
    }
#if APP_DEBUG
    Serial.println("Reset now.");
#endif
    // Reset
    digitalWrite(RESET_PIN, LOW);
    // If the reset pin is not connected then we fall through and just blink.
#endif

#if APP_DEBUG
    Serial.println("Blink forever.");
#endif
    while (1) {
        blink_error();
    }
    return 0;
}

void onEvent (ev_t ev) {
    switch(ev) {
        case EV_JOINED:
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
            // size, we don't use it in this example.
        #if APP_DISABLE_LINK_CHECK_VALIDATION
            LMIC_setLinkCheckMode(0);
        #endif
            break;
    }
#if APP_DEBUG
    app_debug_onEvent(ev);
#endif
}

bool app_is_txrx() {
    return (LMIC.opmode & OP_TXRXPEND) != 0;
}

bool app_is_joined() {
    return LMIC.devaddr != 0;
}

bool app_is_busy(ostime_t time) {
    if (app_is_txrx()) {
        return true;
    }
    if (os_queryTimeCriticalJobs(ms2osticks(time))) {
        return true;
    }
    return false;
}

void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (app_is_txrx()) {
    #if APP_DEBUG
        Serial.println(F("OP_TXRXPEND, not sending"));
    #endif
        return;
    }

    // Read ground sensors.
    auto smt50Reading = smt50_read();

    // Read air sensors.
    bme.takeForcedMeasurement();
    float airTemp = bme.readTemperature();
    float airPress = bme.readPressure() / 100.0F;
    float airHum = bme.readHumidity();

    // Read luminance.
    float light = veml.readLux();

    // Read battery.
    auto batteryReading = battery_read();

    // Do a quick sanity check. Sometimes the sensors return NaNs.
    if (std::isnan(airTemp) || std::isnan(airPress) || std::isnan(airHum) || std::isnan(light)) {
    #if APP_DEBUG
        Serial.println("Abort measurement because of NaN value.");
    #endif
        return;;
    }

    // Fill measurement and subsquently the payload.
    auto measurement = Measurement();
    measurement.timeOffset = 0;
    measurement.temperatureGround = smt50Reading.temperature;
    measurement.moistureGround = smt50Reading.moisture;
    measurement.light = light;
    measurement.temperatureAir = airTemp;
    measurement.moistureAir = airHum;
    measurement.pressure = airPress;
    measurement.battery = batteryReading.percentage;
    measurement.batteryVoltage = batteryReading.voltage;
    payload.fill(&measurement, 1);

#if APP_DEBUG
    // Print measurement.
    Serial.print("Ground temp: ");
    Serial.println(smt50Reading.temperature);
    Serial.print("Ground moisture: ");
    Serial.println(smt50Reading.moisture);
    Serial.print("Air temp: ");
    Serial.println(airTemp);
    Serial.print("Air pressure: ");
    Serial.println(airPress);
    Serial.print("Air humidity: ");
    Serial.println(airHum);
    Serial.print("Light: ");
    Serial.println(light);
    Serial.print("BatV: ");
    Serial.println(batteryReading.voltage);
    Serial.print("Bat%: ");
    Serial.println(batteryReading.percentage);
    auto payloadByteCount = payload.get_byte_count();
    Serial.print("Payload:");
    Serial.println(payloadByteCount);
    for (size_t i = 0; i < payloadByteCount; i++) {
        if (i != 0) {
            Serial.print("-");
        }
        app_debug_printHex2(payload.data[i]);
    }
    Serial.println("");
#endif

    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, payload.data, payload.get_byte_count(), 0);
#if APP_DEBUG
    Serial.println(F("Packet queued"));
#endif
}

void bme280_init() {
    if (!bme.begin(BME_I2C_ADDR)) {
    #if APP_DEBUG
        Serial.println("Could not initialize BME280");
    #endif
        error_handler(); // Never returns
        return;
    }
    // We are measuring once every couple of minutes. Therefore it is
    // okay to use the highest oversampling X16 modes.
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X16, // temperature
                    Adafruit_BME280::SAMPLING_X16, // pressure
                    Adafruit_BME280::SAMPLING_X16, // humidity
                    Adafruit_BME280::FILTER_OFF);
}


void veml7700_init(){
    if (!veml.begin()) {
    #if APP_DEBUG
        Serial.println("Could not initialize VEML7700");
    #endif
        error_handler(); // Never returns
        return;
    }
    veml.setGain(VEML7700_GAIN_1);
    veml.setIntegrationTime(VEML7700_IT_800MS);
    veml.powerSaveEnable(true);
    veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);
    veml.setLowThreshold(10000);
    veml.setHighThreshold(20000);
    veml.interruptEnable(true);
}

void setup() {
    // Init reset pin.
    digitalWrite(RESET_PIN, HIGH);
    pinMode(RESET_PIN, OUTPUT); 
    
    delay(2000);
    
    // Turn off LED.
    digitalWrite(LED_BUILTIN, LOW);

    delay(3000);

    // Init sensors.
    battery_init();
    smt50_init();
    veml7700_init();
    bme280_init();
    
#if APP_DEBUG
    while (! Serial);
    Serial.begin(9600);
    Serial.println(F("Starting"));
#endif

    // LMIC init.
    os_init();
    
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    
    // Disable link-check mode and ADR, because ADR tends to complicate testing.
#if APP_DISABLE_LINK_CHECK_VALIDATION
    LMIC_setLinkCheckMode(0);
#endif
    
    // Set the data rate to Spreading Factor 7.  This is the fastest supported rate for 125 kHz channels, and it
    // minimizes air time and battery power. Set the transmission power to 14 dBi (25 mW).
    LMIC_setDrTxpow(DR_SF7,14);

#if APP_DEBUG
    Serial.println("Setup complete.");
#endif

#if APP_BLINK_LED
    // Blink short, short after setup.
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
#endif

    // Start job (sending automatically starts OTAA too).
    do_send(&sendjob);
}

void loop() {    
    // We call the LMIC's runloop processor. This will cause things to happen based on events and time. One
    // of the things that will happen is callbacks for transmission complete or received messages. We also
    // use this loop to queue periodic data transmissions.  You can put other things here in the `loop()` routine,
    // but beware that LoRaWAN timing is pretty tight, so if you do more than a few milliseconds of work, you
    // will want to call `os_runloop_once()` every so often, to keep the radio running.
    os_runloop_once();
    
#if APP_DEBUG
    app_debug_opmode();
#endif

    if (!app_is_joined() || app_is_busy(17000)) {
        return;
    }

    // If we reach this point, we are free to do whatever we want
    // because there are no pending LoRaWAN tasks.
    
#if APP_DEBUG
    Serial.println("Go to sleep");
    Serial.println("---------------------------------------------------------------------");
#endif

#if APP_BLINK_LED
    // Blink long before sleeping.
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
#endif

    // Sleep.
#if APP_SLEEP_DELAY
    // Use regular delay.
    delay(APP_SLEEP_DELAY_MS);
#elif APP_SLEEP_WATCHDOG
    // Use real deep sleep.
    for (int i = 0; i <= APP_SLEEP_WATCHDOG_MULTIPLIER; i++) {
      Watchdog.sleep(APP_SLEEP_WATCHDOG_MS);
    }
#endif
    
#if APP_DEBUG
    Serial.println("Woken up");
#endif

#if APP_BLINK_LED
    // Blink short, short after waking.
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
#endif

    // Send next job.
    do_send(&sendjob);
}
