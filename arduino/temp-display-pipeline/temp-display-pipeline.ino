#include <Arduino.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

// Raspberry Pi Pico with the Earle Philhower Arduino-Pico core.
// Use GPIO numbers, not physical header pin numbers.
constexpr uint8_t ONE_WIRE_BUS = 22;  // GP22, physical pin 29 on the Pico
constexpr uint8_t LCD_SDA_PIN = 4;    // GP4, physical pin 6
constexpr uint8_t LCD_SCL_PIN = 5;    // GP5, physical pin 7
constexpr uint8_t LED = 25;

constexpr unsigned long UPDATE_INTERVAL_MS = 1000UL;
constexpr uint8_t DISPLAY_LENGTH = 20;
constexpr uint8_t DISPLAY_HEIGHT = 4;
constexpr double TARGET_C = 25.0;
const double RANGES_C[] = {0.5, 4.0, 10.0};
constexpr uint8_t RANGE_COUNT = sizeof(RANGES_C) / sizeof(RANGES_C[0]);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, DISPLAY_LENGTH, DISPLAY_HEIGHT);

uint8_t plusMinusChar[8] = {
  0b00100, 0b00100, 0b11111, 0b00100, 0b00100, 0b00000, 0b11111, 0b00000
};

unsigned long lastUpdateStartedMs = 0;
unsigned long lastSampleMs = 0;
uint64_t totalMs = 0;
uint64_t outsideRangeMs[RANGE_COUNT] = {};
float previousTemperatureC = 0.0;
bool previousSampleValid = false;
bool haveSample = false;

// Fixed-width durations keep all four rows aligned. Saturate after 99:59:59.
void printDuration(uint64_t durationMs) {
  uint64_t seconds = durationMs / 1000ULL;
  if (seconds > 359999UL) {
    seconds = 359999UL;
  }
  char text[9];
  snprintf(text, sizeof(text), "%02u:%02u:%02u",
           static_cast<unsigned int>(seconds / 3600UL),
           static_cast<unsigned int>((seconds % 3600UL) / 60UL),
           static_cast<unsigned int>(seconds % 60UL));
  lcd.print(text);
}

void printDisplay(float temperatureC, bool valid) {
  for (uint8_t i = 0; i < RANGE_COUNT; ++i) {
    lcd.setCursor(0, i);
    lcd.print(TARGET_C, 0);
    lcd.write(static_cast<uint8_t>(1));
    lcd.print(RANGES_C[i], 1);
    lcd.write(static_cast<uint8_t>(0xDF));
    lcd.print(F("C     "));
    lcd.setCursor(DISPLAY_LENGTH - 8, i);
    printDuration(outsideRangeMs[i]);
  }

  // Live temperature on the left; total elapsed time on the right.
  lcd.setCursor(0, 3);
  lcd.print(F("            "));
  lcd.setCursor(0, 3);
  if (valid) {
    lcd.print(F("TOTAL"));
  } else {
    lcd.print(F("SENSOR ERROR"));
  }
  lcd.setCursor(DISPLAY_LENGTH - 8, 3);
  printDuration(totalMs);
}

void initializeTemperatureSensor() {
  sensors.begin();
  sensors.setResolution(12);
  sensors.setWaitForConversion(true);
  Serial.print(F("Temperature sensors found on GP22: "));
  Serial.println(sensors.getDeviceCount());
}

void updateTemperature() {
  // begin() caches the device count. Rescan after an error so a sensor that
  // was absent at startup can be found again without restarting the Pico.
  if (haveSample && !previousSampleValid) {
    initializeTemperatureSensor();
  }
  sensors.requestTemperatures();
  const float temperatureC = sensors.getTempCByIndex(0);
  const bool valid = temperatureC != DEVICE_DISCONNECTED_C &&
                     isfinite(temperatureC) &&
                     temperatureC >= -55.0f && temperatureC <= 125.0f;
  const unsigned long sampleMs = millis();

  if (haveSample) {
    // Unsigned subtraction also handles the millis() rollover.
    const unsigned long elapsedMs = sampleMs - lastSampleMs;
    totalMs += elapsedMs;
    // Discard intervals touching an invalid reading instead of guessing.
    if (previousSampleValid && valid) {
      for (uint8_t i = 0; i < RANGE_COUNT; ++i) {
        // Preserve the SD pipeline's inclusive, outside-range comparison.
        if (previousTemperatureC <= TARGET_C - RANGES_C[i] ||
            previousTemperatureC >= TARGET_C + RANGES_C[i]) {
          outsideRangeMs[i] += elapsedMs;
        }
      }
    }
  }

  lastSampleMs = sampleMs;
  previousTemperatureC = temperatureC;
  previousSampleValid = valid;
  haveSample = true;
  printDisplay(temperatureC, valid);

  if (valid) {
    Serial.print(F("Temperature (C): "));
    Serial.println(temperatureC, 1);
  } else {
    Serial.print(F("Temperature read failed (C): "));
    Serial.println(temperatureC);
    Serial.print(F("OneWire presence on GP22: "));
    Serial.println(oneWire.reset() ? F("YES") : F("NO"));
    Serial.println(F("Check sensor power, GP22 data, ground, and 4.7k pull-up."));
    Serial.println(F("Rescanning at the next 5-second update."));
  }
}

void setup() {
  Serial.begin(9600);
  Wire.setSDA(LCD_SDA_PIN);
  Wire.setSCL(LCD_SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, plusMinusChar);
  lcd.clear();
  initializeTemperatureSensor();
  pinMode(LED, OUTPUT);

  // Show the first reading immediately, then start one conversion every 5 s.
  lastUpdateStartedMs = millis();
  updateTemperature();
}
int toggle = 1;
void loop() {
  const unsigned long now = millis();
  if (now - lastUpdateStartedMs >= UPDATE_INTERVAL_MS) {
    lastUpdateStartedMs = now;
    updateTemperature();
    digitalWrite(LED, toggle);
    toggle = !toggle;
  }
}
