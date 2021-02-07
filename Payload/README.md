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

Zum Kompilieren des Tests bitte diese [Anleitung](https://code.visualstudio.com/docs/cpp/config-msvc) befolgen. Außerdem muss zuvor `npm run build:arduino` ausgeführt werden.