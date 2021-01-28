# TTN

## Payload-Dekodierfunktion erstellen

Der JavaScript-Code zum Dekodieren von LoRaWAN-Nachrichten für das TNN-Backend kann automatisch aus einer Definition der Payload-Felder generieren lassen.

```
npm run build
```

Die Felddefinitionen befindet sich in der Datei `versions.js`. Dort können neue Nachrichtenformate definiert werden können. Jedes Formate besitzt eine Versionsnummer und sollte nicht mehr geändert werden nachdem es einmal ausgerollt wurde. Stattdessen muss immer eine neue Version erstellt werden, wodurch sichergestellt wird, dass auch solche Endgeräte noch Nachrichten senden können, die mit einer älteren Version laufen.

Der generierte Code wird in die Datei `./dist/decoder.js` geschrieben und kann dann in die TTN-Console kopiert werden: 

![Screenshot TTN payload decoder](https://git.it.hs-heilbronn.de/jemaier/lorawan-akdt-hw/-/raw/develop/TTN/assets/screen-payload-decoder.jpg)
