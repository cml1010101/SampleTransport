# SampleTransport
This is a project for EGR 101 at Duke University. The goal of this project is to track temperature in sample transport conditions in Uganda.

## Live temperature display

Open `arduino/temp-display-pipeline/temp-display-pipeline.ino` in the Arduino IDE.
Select **Raspberry Pi Pico** from Earle Philhower's **Raspberry Pi Pico/RP2040**
board package (`rp2040:rp2040:rpipico`). This sketch uses the Arduino-Pico core,
including its configurable I2C pins.

This combines the DS18B20 reader from `test-temp` and the 20x4 I2C LCD from
`test-display` with the range timers from `sd-display-pipeline`. It needs no SD
card or SD library. Install **OneWire**, **DallasTemperature**, and
**LiquidCrystal_I2C** (the version with `lcd.init()`).

The first reading appears at startup; a new reading and display refresh follow
every **5 seconds**. Each temperature conversion takes up to about 750 ms.
Serial output uses 9600 baud.

Wiring defaults:

| Connection | Raspberry Pi Pico |
| --- | --- |
| DS18B20 data | GP22 (physical pin 29), matching `test-temp` |
| DS18B20 VDD | 3V3(OUT) (physical pin 36) |
| DS18B20 GND | GND (for example physical pin 38) |
| LCD SDA | GP4 (physical pin 6) |
| LCD SCL | GP5 (physical pin 7) |

Use a 4.7 kOhm pull-up from sensor data to 3V3(OUT) and a common ground.
The sketch expects a powered DS18B20, rather than parasite power.
Keep Pico I2C signals at 3.3 V using a level shifter
if the LCD backpack pulls them up to 5 V. The LCD address is `0x27`.

The first three rows show cumulative time **outside** 4 +/- 0.5, 4, and 10 degrees
Celsius, respectively, including the boundaries, as in the SD pipeline. Each
interval uses its preceding temperature reading. The last row shows the live
Celsius temperature and total elapsed time since the first reading. A sensor
failure displays `SENSOR ERROR`; intervals touching a failed reading are excluded
from the range timers. After a failed reading, the sketch rescans for the sensor
at the next five-second update, including when it was absent at startup.
Serial diagnostics report the raw failed reading, OneWire presence on GP22,
and the sensor count after each scan. A `NO` presence result means no presence
pulse was detected; `YES` with failed readings can indicate corrupted data or
unstable sensor power. Neither message alone identifies the faulty component.
Durations display as `HH:MM:SS` and stop displaying increases at `99:59:59`.
All timers live in RAM and reset when the board restarts.
