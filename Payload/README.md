# Payload

## TTN

## Nachrichtenfelder definieren

Die Felddefinitionen befindet sich in der Datei `versions.js`. Dort können neue Nachrichtenformate definiert werden können. Jedes Format besitzt eine Versionsnummer und sollte nicht mehr geändert werden, nachdem es einmal ausgerollt wurde. Stattdessen muss immer eine neue Version erstellt werden, wodurch sichergestellt wird, dass auch solche Endgeräte noch Nachrichten senden können, die mit einer älteren Version laufen.

### Payload-Dekodierfunktion erstellen

```
npm run build:ttn
```

Der JavaScript-Code zum Dekodieren von LoRaWAN-Nachrichten für das TNN-Backend wird anhand der Payload-Felder erstellt. Die Funktion ist in der Lage alle definierten Nachrichtenversionen zu dekodieren. Der generierte Code wird in die Datei `ttn/dist/decoder.js` geschrieben und kann daraus in die TTN-Console kopiert werden: 

![Screenshot TTN payload decoder](https://git.it.hs-heilbronn.de/jemaier/lorawan-akdt-hw/-/raw/develop/Payload/assets/screen-payload-decoder.jpg)

### Testen

```
npm run test:ttn
```

## Messstation

### Enkodierfunktion erstellen

Für die neueste Version des Nachrichtenformats:
```
npm run build:arduino
```

Oder spezifische Version:
```
npm run build:arduino -- 1
```

Der Code zum Kodieren einer LoRaWAN-Nachricht auf der Messstation-Seite wird ebenfalls anhand der Payload-Felder erstellt. Die generierte Funktion unterstützt nur das eine angegebene Nachrichtenformat. Der generierte Code befindet sich im Order `arduino/dist` und kann in das Arduinoprojekt kopiert werden.

### Testen

```
npm run test:arduino
```

Zum Kompilieren des Tests bitte diese [Anleitung](https://code.visualstudio.com/docs/cpp/config-msvc) befolgen.

## Nachrichtenformate
<!---#VERSIONS--->

### Version 1
<br>

Nachricht
| Bezeichnung      | Bits      | Anmerkung                                  |
| ---------------- | --------- | ------------------------------------------ |
| Version          | 8         | Versionsnummer des Nachrichtenformats.     |
| Anzahl Messungen | 8         | Anzahl der Messungen in dieser Nachricht.  |
| Messungen        | 75 {1, 5} | 1 bis 5 Messungen pro Nachricht je 75 Bits |
<br>

Messung
| Bezeichnung             | Bits | Sensor   | Einheit | Bereich           | Genauigkeit | Auflösung |
| ----------------------- | ---- | -------- | ------- | ----------------- | ----------- | --------- |
| Measurement time offset | 12   |          | min     | 0min .. 4095min   | 1           | +- 0 min  |
| Ground temperature      | 8    | SMT50    | °C      | -20°C .. 85°C     | 0.5         | +- 0.8 °C |
| Ground water content    | 6    | SMT50    | %       | 0% .. 50%         | 1           | +- 3 %    |
| Illuminance             | 17   | VEML7700 | lx      | 0lx .. 120000lx   | 1           | +- 10%    |
| Air temperature         | 8    | BME280   | °C      | -40°C .. 80°C     | 0.5         | +- 0.5 °C |
| Relative air humidity   | 7    | BME280   | %       | 0% .. 100%        | 1           | +- 3 %    |
| Barometric pressure     | 10   | BME280   | hPa     | 300hPa .. 1100hPa | 1           | +- 1 hPa  |
| Battery charge level    | 7    |          | %       | 0% .. 100%        | 1           | +- 0 %    |
<br>

Beispiel:
```json
{
    "version": 1,
    "measurements": [
        {
            "timeOffset": "2048",
            "temperatureGround": "42.5",
            "moistureGround": "25",
            "light": "60000",
            "temperatureAir": "40.0",
            "moistureAir": "50",
            "pressure": "550",
            "battery": "50"
        }
    ]
}
```
<!---#ENDVERSIONS--->

## Dokumentation erstellen

Aktualisieren der Versionsdokumentation in diesem README:
```
npm run buid:dox
```

## Weitere Befehle

Kurzform zum Erstellen aller Komponenten und Dokumentation:
```
npm run build
```

Kurzform zum Testen aller Komponenten:
```
npm run test
```

Alle erstellen und temporären Dateien löschen:
```
npm run clean
```