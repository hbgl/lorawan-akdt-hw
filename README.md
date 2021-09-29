# LoRaWAN im Stadtwald

Ziel dieses Projekts ist es, Messstation in den Heilbronner Stadtwald aufzustellen, welche Umweltdaten sammeln, welche Schulklassen mittels eines Online-Dashboard angezeigt werden. Die Messstationen übertragen die Werte per LoRaWAN an das The Things Network (TTN), welches die Daten an den Webserver weiterleitet, der das Dashboard hostet.

Dieses Repository enthält zum einen den Quellcode für die Messstationen und daneben ein Tool zum erstellen von Payload-Encoder und -Decoder. Die Dokumentation der beiden Komponenten befindet sich in den jeweiligen Unterordnern `./Messstation` und `./Payload`.

![Datenfluss](./Meta/LoRaWAN%20im%20Stadtwald.png)
