#include <Wire.h>

void setup() {
  Serial.begin(115200);
  // Allow time to open Serial Monitor without waiting forever.
  unsigned long start = millis();
  while (!Serial && millis() - start < 5000) {
    delay(10);
  }

  Wire.setSDA(4);  // GP4, physical pin 6
  Wire.setSCL(5);  // GP5, physical pin 7
  Wire.begin();
  Wire.setClock(100000);
}

void loop() {
  Serial.println("Scanning I2C...");
  unsigned int found = 0;

  for (uint8_t address = 0x08; address <= 0x77; ++address) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Device found at 0x");
      if (address < 0x10) Serial.print('0');
      Serial.println(address, HEX);
      ++found;
    }
  }

  if (found == 0) Serial.println("No devices found. Check power and wiring.");
  Serial.println();
  delay(3000);
}
