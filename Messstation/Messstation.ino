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
 * Dieses Sketch ermittelt mittels folgender Sensoren: Veml7700, BME280, SMT50 die an einem Adafruit Feather M0 angeschlossen sind verschiedene Parameter                   
 * und übermittelt diese mittels LoRa an ein Gateway.
 * 
 * Version: 29.12.2020
 */

#include "Adafruit_VEML7700.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SleepyDog.h>

//for SMT50
#define groundTempPin A0
#define groundHumPin A1

// for BME280
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_VEML7700 veml = Adafruit_VEML7700();
Adafruit_BME280 bme;

//for SMT50
float groundTemp = 0;
float groundHum = 0;

//for BME280
float airTemp = 0;
float airHum = 0;
float airPress = 0;

//for VEML7700
float light = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  setupVeml7700();
  setupBME280();
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  
  getGroundValues();
  getLightValues();
  getAirValues();

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  for(int i = 0; i <= 2; i++){  //449 mal für eine Stunde
    Watchdog.sleep(8000);
  } 
}

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

void setupBME280(){
    unsigned status;
    
    // default settings
    status = bme.begin(0x76);
}

void getGroundValues(){
  groundTemp = analogRead(groundTempPin);
  groundTemp = groundTemp * 2 * 3.3 /1024;
  groundTemp = ((groundTemp / 2) - 0.5) * 100;

  groundHum = analogRead(groundHumPin);
  groundHum = groundHum * 2 * 3.3 /1024;
  groundHum = groundHum / 2 * 50 / 3;
}

void getLightValues(){
  light = veml.readLux();
}

void getAirValues(){
    airTemp = bme.readTemperature();
    airPress = bme.readPressure() / 100.0F;
    airHum = bme.readHumidity();
}
