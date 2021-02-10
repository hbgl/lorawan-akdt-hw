/*
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
 * Authoren: Markus Pfeffermann, Kenan Ekmekci
 * 
 * Dieses Sketch ermittelt mittels Sensoren die an einem Adafruit Feather M0 angeschlossen sind verschiedene Parameter                   
 * und übermittelt diese mittels LoRa an ein Gateway.
 * 
 * Sensoren:  Veml7700  - Lux Sensor zur Messung des Lichts/der Helligkeit
 *            BME280    - Überirdischer Temperatur-, Feuchtigkeit- und Luftdrucksensor
 *            SMT50     - Unterirdischer Temperatur- und Feuchtigkeitssensor
 * 
 * Version: 08.02.2021
 */

/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network. It's pre-configured for the Adafruit
 * Feather M0 LoRa.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 *
 *******************************************************************************/
//#################################### Definitions ############################################
//definitions for LoRa
#define LMIC_DEBUG_LEVEL 2
#define LMIC_PRINTF_TO Serial

//definitions for SMT50
#define groundTempPin A0
#define groundHumPin A1

//definitions for BME280
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)

//for battery reading
#define VBATPIN A7

//##################################### Libraries #############################################
//libraries for LoRa
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "secret-config.h"

//libraries for sensors
#include "Adafruit_VEML7700.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SleepyDog.h>
#include "payload.h"

Adafruit_VEML7700 veml = Adafruit_VEML7700();
Adafruit_BME280 bme;

//##################################### Variables #############################################
auto loraloop = false;

auto measurement = Measurement();
auto payload = Payload();

//for SMT50
float groundTemp = 0;
float groundHum = 0;

//for BME280
float airTemp = 0;
float airHum = 0;
float airPress = 0;

//for VEML7700
float light = 0;

//for battery measurement
float batVolt;
int batPerc;
float volMax[] = {4.5, 4.2, 4.15, 4.1, 4.05, 4.0, 3.95, 3.9, 3.85, 3.8, 3.75, 3.7, 3.65, 3.6, 3.54, 3.5, 3.44, 3.4, 3.34, 3.3, 3.24};
float volMin[] = {4.2, 4.15, 4.1, 4.05, 4.0, 3.95, 3.9, 3.85, 3.8, 3.75, 3.7, 3.65, 3.6, 3.54, 3.5, 3.44, 3.4, 3.34, 3.3, 3.24, 3.19};
int percentage[] = {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 1, 1};

// checking secret-config.h configuration for LoRa. File needs to be in same folder as this.
#if !defined(APPEUI_BYTES) || !defined(DEVEUI_BYTES) || !defined(APPKEY_BYTES)
# error "Missing secret configuration"
#endif


// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]= { APPEUI_BYTES };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]= { DEVEUI_BYTES };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from the TTN console can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = { APPKEY_BYTES };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 60;

// Pin mapping
//
// Adafruit BSPs are not consistent -- m0 express defs ARDUINO_SAMD_FEATHER_M0,
// m0 defs ADAFRUIT_FEATHER_M0
//
#if defined(ARDUINO_SAMD_FEATHER_M0) || defined(ADAFRUIT_FEATHER_M0)
// Pin mapping for Adafruit Feather M0 LoRa, etc.
const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 6, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 8,              // LBT cal for the Adafruit Feather M0 LoRa, in dB
    .spi_freq = 8000000,
};
#else
# error "Unknown target"
#endif
/*
void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}
*/

void onEvent (ev_t ev) {
    switch(ev) {
        case EV_JOINING:
            //Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            //Serial.println(F("EV_JOINED"));
            {
              u4_t netid = 0;
              devaddr_t devaddr = 0;
              u1_t nwkKey[16];
              u1_t artKey[16];
              LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            }
        case EV_TXSTART:
            //Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCOMPLETE:
            //Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            loraloop = false;
            break;
        case EV_JOIN_TXCOMPLETE:
            //Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
            break;
        default:
            break;
    }
}

// sending data via LoRa if possible
void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        //Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, payload.data, 13, 0); // TODO: Calculate payload size
        //Serial.println(F("Packet queued"));
    }
}

// setting up Veml7700 sensor for light measurement
void setupVeml7700(){
  veml.begin();
  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_800MS);
  
  //veml.powerSaveEnable(true);
  //veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);

  veml.setLowThreshold(10000);
  veml.setHighThreshold(20000);
  veml.interruptEnable(true);
}

// setting up BME280 sensor for measurement
void setupBME280(){
    unsigned status;
    
    // default settings
    status = bme.begin(0x76);
}

// measurement of ground values with SMT50 sensor
void getGroundValues(){
  groundTemp = 0;
  groundHum = 0;
  
  // three measurements to smooth out deviation
  for (int i = 0; i <= 2; i++){
    groundTemp += analogRead(groundTempPin);
    groundHum += analogRead(groundHumPin);
  }

  groundTemp /= 3;
  groundTemp = groundTemp * 2 * 3.3 /1024;
  groundTemp = ((groundTemp / 2) - 0.5) * 100;

  groundHum /= 3;
  groundHum = groundHum * 2 * 3.3 /1024;
  groundHum = groundHum / 2 * 50 / 3;
}

// measurement of light with Veml7700 sensor
void getLightValues(){
  light = 0;
  
  // three measurements to smooth out deviation
  for (int i = 0; i <= 2; i++){
    light += veml.readLux();
  }
  light /= 3;
}

// measurement of air values with BME280 sensor
void getAirValues(){
  airTemp = 0;
  airPress = 0;
  airHum = 0;
  
  // three measurements to smooth out deviation
  for (int i = 0; i <= 2; i++){
    airTemp += bme.readTemperature();
    airPress += bme.readPressure() / 100.0F;
    airHum += bme.readHumidity();
  }
  
  airTemp /= 3;
  airPress /= 3;
  airHum /= 3;
}

// measureing voltage of the battery
void getBatVolt(){
  batVolt = analogRead(VBATPIN);
  batVolt *= 2;
  batVolt *= 3.3;
  batVolt /= 1024;
}

// converting voltage of battery to percentage
void getBatPerc(){
  for (int i = 0; i <= 20; i++){
    if(batVolt < volMax[i] && batVolt >= volMin[i]){
      batPerc = percentage[i];
    }
  }
}

void setup() {
    delay(5000);
//    while (! Serial)
//        ;
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    setupVeml7700();
    setupBME280();

    #ifdef VCC_ENABLE
    // For Pinoccio Scout boards
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
    #endif

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    LMIC_setLinkCheckMode(0);
    LMIC_setDrTxpow(DR_SF7,14);
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
  
    getGroundValues();
    getLightValues();
    getAirValues();
    getBatVolt();
    getBatPerc();

    measurement.timeOffset = 0;
    measurement.temperatureGround = groundTemp;
    measurement.moistureGround = groundHum;
    measurement.light = light;
    measurement.temperatureAir = airTemp;
    measurement.moistureAir = airHum;
    measurement.pressure = airPress;
    measurement.battery = batPerc;
    measurement.batteryVoltage = batVolt;
    payload.fill(&measurement, 1);

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);

    loraloop = true;
    while (loraloop) {
      os_runloop_once(); 
    }

    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);

    for(int i = 0; i <= 1; i++){  //449 mal für eine Stunde
      Watchdog.sleep(8000);
    } 
}
