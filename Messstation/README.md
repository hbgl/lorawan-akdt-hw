# LoRaWAN Messstation

Dieser Unterordner enthält das Arduino-Projekt für die Messstation. Das Projekt wurde für das [Adafruit Feather M0 LoRa](https://www.adafruit.com/product/3178) entwickelt, sollte sich aber auch relativ einfach auf ähnliche Hardware portieren lassen.

## Hardware

- Adafruit Feather M0 LoRa 900 MHz ([Herstellerseite](https://www.adafruit.com/product/3178))
- BME280 Breakout Board (z.B. [SparkFun](https://www.sparkfun.com/products/13676) oder [Aliexpress](https://www.aliexpress.com/item/1005003227145786.html?spm=a2g0o.productlist.0.0.4776865dqCH1no&algo_pvid=953f8ec4-0cee-4029-8f4e-af67a7270361&algo_exp_id=953f8ec4-0cee-4029-8f4e-af67a7270361-12&pdp_ext_f=%7B%22sku_id%22%3A%2212000024747871385%22%7D))
- VEML7700 Breakout Board (z.B. [Adafruit](https://www.adafruit.com/product/4162))
- SMT50 Sensor für Bodenfeuchtigkeit und -temperatur ([Herstellerseite](http://www.truebner.de/de/smt50.php))
- 900 MHz LoRa-Antenne mit Kabel (z.B. [Adafruit](https://www.adafruit.com/product/3340))
- 3.6V Lithium-Ionen-Akku mit JST PH Anschluss (z.B. [Adafruit](https://www.adafruit.com/product/2011) oder [NL1829LTHP](https://flashlight.nitecore.com/product/nl1829lthp)), mehr dazu unter dem Punkt [Stromversorgung](#Stromversorgung)
- P-Channel MOSFET und >=50kOhm Pull-Up Widerstand (z.B. [TSM500P02CX](https://www.taiwansemi.com/assets/uploads/datasheet/TSM500P02CX_B1811.pdf)), mehr dazu unter dem Punkt [Stromsparen](#Stromsparen)

## Arduino-Projekt einrichten

Zuerst muss das Adafruit Feather Board in der Arduino IDE installiert werden. Die Schritte sind in dieser [Anleitung](https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module/setup) beschrieben.

Zusätzlich müssen folgende Arduino-Code-Bibliotheken installiert werden:

- [MCCI LoRaWAN LMIC library](https://github.com/mcci-catena/arduino-lmic) v4.0.0
- [Adafruit VEML7700 Library](https://github.com/adafruit/Adafruit_VEML7700) v1.1.1
- [Adafruit BME280 library](https://github.com/adafruit/Adafruit_BME280_Library) v2.1.2
- [Adafruit SleepyDog Library](https://github.com/adafruit/Adafruit_SleepyDog) v1.5.0

### Konfiguration

Die Datei `project_config\lmic_project_config.h` der MCCI LoRaWAN LMIC library muss geöffnet und entsprechend angepasst werden ([siehe Doku](https://github.com/mcci-catena/arduino-lmic#configuration)). Beispiel:

```C++
// project-specific definitions
#define CFG_eu868 1
//#define CFG_us915 1
//#define CFG_au915 1
//#define CFG_as923 1
// #define LMIC_COUNTRY_CODE LMIC_COUNTRY_CODE_JP	/* for as923-JP */
//#define CFG_kr920 1
//#define CFG_in866 1
#define CFG_sx1276_radio 1
//#define LMIC_USE_INTERRUPTS
```

Die Datei `secret-config.sample.h` muss kopiert und in `secret-config.h` umbenannt werden. Danach müssen die echten Werte für `APPEUI`, `DEVEUI` und `APPKEY` eingetragen werden. Der Werte finden sich in der Gerätedetailansicht in der [TTN Console](https://console.cloud.thethings.network/).

## Verkabelung

Pin 6 muss mit Pin io1 verbunden werden ([siehe Doku](https://learn.adafruit.com/the-things-network-for-feather?view=all#arduino-wiring)).

Für die Sensoren ist die Pinbelegung standardmäßig wie folgt:

- SMT50 Temperaturpin an **A0**
- SMT50 Feuchtigkeitspin an **A1**
- P-Channel MOSFET Gate-Pin an **A2**
- BME280 und VEML7700 an I2C

TODOD: Schaltplan erstellen.

## Stromsparen

Um eine möglichst lange Batterielaufzeit zu erreichen, ist es wichtig, dass die Komponenten möglichst effizient sind. Da sich die Messstation zu 99% im Leerlauf befindet, ist es besonders darauf zu achten, dass die Komponententen einen möglichst geringen Ruhestrom haben. Die größten Übeltäter sind hier Spannungsregler, die oft auf Breakoutboards zu finden sind.

**Wenn Breakoutboards verwendet werden, dann sollen darauf möglichst keine unnötigen Spannungsregler verbaut sein.**

Viele Breakoutboards haben einen Spannungsregler, der den Betrieb mit 3.3V und 5V erlaubt. Nativ arbeiten die meisten Boards aber bereits mit den 3.3V von unserem Feather, weswegen Spannungsregler oftmals nicht notwendig sind und nur unnötig Strom verbrauchen.

### BME280

Hier gibt es zwei Versionen von Breakoutboards: Eines für 3.3V **und** 5V und eines für **nur** 3.3V. Falls möglich, dann sollte letztere verwendet werden. Softwareseitig sollte der Sampling-Modus auf `forced` umgestellt werden, wodurch der Sensor nach jeder Messung direkt in den Schlafzustand übergeht.

### VEML7700

Das am besten verfügbare Breakoutboard von Adafruit besitzt einen Spannungsregler, der entlötet werden sollte ([siehe Bild](../Meta/VEML7700%20ohne%20Spannungsregler.jpg)). Außerdem sollte im Code der Power Saving Mode aktiviert werden.

### SMT50 mit P-Channel MOSFET schalten

Der Bodensensor SMT50 verbraucht im Leerlauf 2mA, was die Batterielaufzeit deutlich reduzieren würde, weswegen ein P-Channel MOSFET eingesetzt wird (z.B. [TSM500P02CX](https://www.taiwansemi.com/assets/uploads/datasheet/TSM500P02CX_B1811.pdf)), um den Sensor auszuschalten, wenn er nicht benötigt wird. Der Gate-Pin des MOSFETs wird dabei mit einem Pull-Up-Widerstand >=50kOhm auf 3.3V gezogen und schaltet dadurch den SMT50 standardmäßig aus.

### Adafruit Feather Mod

Das Adafruit Feather Board kommt standardmäßig mit einem [AP2112](https://www.diodes.com/assets/Datasheets/AP2112.pdf) Spannungsregler, der einen recht hohen Ruhestrom von 55µA aufweist. Mit einem effizienteren 1:1-Ersatzteil könnte der Ruhestrum von 55µA auf 5µA oder weniger reduziert werden. Siehe hierzu dieses [Video von Adafruit](https://youtu.be/OQNhpKNq0EY).

### Stromsparübersicht

| Zustand | Leerlaufstrom Änderung | Leerlaufstrom gesamt |
| - | - | - |
| Standard | 0mA | 2.6mA |
| VEML7700 ohne Spannungsregler | -0.6mA | 2mA |
| SMT50 mit MOSFET ausschalten | -1.9mA | 0.1mA |
| Adafruit Feather Spannungsregler austauschen | -0.05mA* | 0.05mA* |
\* geschätzt

## Stromversorgung

Ein 3.6V Lithium-Ionen-Akkus kann per JST PH Stecker an das Board zwecks Stromversorgung angeschlossen werden. Für den reibungslosen Betrieb im Freien sollte die Batterie auch für niedrige Temperaturen geeignet sein, weswegen wir den [Nitecore NL1829LTHP](https://flashlight.nitecore.com/product/nl1829lthp) Akku verwenden, welcher auch bei Minusgraden die nötige Leistung liefern kann.