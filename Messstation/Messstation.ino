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

//for VEML7700
float light = 0;

void setup() {
  Serial.begin(9600);
  setupVeml7700();
  setupBME280();
}

void loop() {
  getGroundValues();
  getLightValues();
  getAirValues();

  delay(1000);
}

void setupVeml7700(){
  while (!Serial) { delay(10); }
  Serial.println("Adafruit VEML7700 Test");

  if (!veml.begin()) {
    Serial.println("Sensor not found");
    while (1);
  }
  Serial.println("Sensor found");

  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_800MS);

  Serial.print(F("Gain: "));
  switch (veml.getGain()) {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
  }

  Serial.print(F("Integration Time (ms): "));
  switch (veml.getIntegrationTime()) {
    case VEML7700_IT_25MS: Serial.println("25"); break;
    case VEML7700_IT_50MS: Serial.println("50"); break;
    case VEML7700_IT_100MS: Serial.println("100"); break;
    case VEML7700_IT_200MS: Serial.println("200"); break;
    case VEML7700_IT_400MS: Serial.println("400"); break;
    case VEML7700_IT_800MS: Serial.println("800"); break;
  }

  //veml.powerSaveEnable(true);
  //veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);

  veml.setLowThreshold(10000);
  veml.setHighThreshold(20000);
  veml.interruptEnable(true);
}

void setupBME280(){
    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    unsigned status;
    
    // default settings
    status = bme.begin(0x76);  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }
    
    Serial.println("-- Default Test --");
    Serial.println();
}

void getGroundValues(){
  groundTemp = analogRead(groundTempPin);
  groundTemp = groundTemp * 2 * 3.3 /1024;
  groundTemp = ((groundTemp / 2) - 0.5) * 100;

  groundHum = analogRead(groundHumPin);
  groundHum = groundHum * 2 * 3.3 /1024;
  groundHum = groundHum / 2 * 50 / 3;

  Serial.print("Bodentemperatur: ");
  Serial.print(String(groundTemp));
  Serial.println("°C");

  Serial.print("Bodenfeuchtigkeit: ");
  Serial.print(String(groundHum));
  Serial.println("%");
}

void getLightValues(){
  light = veml.readLux();
  Serial.print("Lux: "); Serial.println(String(light));

  uint16_t irq = veml.interruptStatus();
  if (irq & VEML7700_INTERRUPT_LOW) {
    Serial.println("** Low threshold"); 
  }
  if (irq & VEML7700_INTERRUPT_HIGH) {
    Serial.println("** High threshold"); 
  }
}

void getAirValues(){
    Serial.print("Lufttemperatur: ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");

    Serial.print("Luftdruck: ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Luftfeuchtigkeit: ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}
