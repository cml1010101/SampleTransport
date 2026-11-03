/*********
  Complete project details at https://randomnerdtutorials.com
  Based on the Dallas Temperature Library example
*********/

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pico with the Earle Philhower Arduino-Pico core:
// DS18B20 VDD -> 3V3(OUT), physical pin 36; GND -> GND, pin 8.
// DQ -> GP22, physical pin 29, with a 4.7k ohm pull-up to 3V3.
// Use GPIO numbers here, not physical header pin numbers.
// GP4/GP5 are reserved for the LCD's I2C bus in test-display.
constexpr uint8_t ONE_WIRE_BUS = 22;

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

void setup(void)
{
  Serial.begin(9600);
  // Give USB Serial a moment to connect, but allow standalone operation.
  const unsigned long serialStart = millis();
  while (!Serial && millis() - serialStart < 3000) {
    delay(10);
  }
  sensors.begin();
  Serial.print("Temperature sensors found: ");
  Serial.println(sensors.getDeviceCount());
}

void loop(void)
{
  sensors.requestTemperatures();
  const float temperatureC = sensors.getTempCByIndex(0);
  if (temperatureC == DEVICE_DISCONNECTED_C) {
    Serial.println("Temperature sensor disconnected. Check wiring and pull-up.");
    delay(1000);
    return;
  }

  Serial.print("Celsius temperature: ");

  Serial.print(temperatureC);
  Serial.print(" - Fahrenheit temperature: ");
  Serial.println(DallasTemperature::toFahrenheit(temperatureC));
  delay(1000);
}
