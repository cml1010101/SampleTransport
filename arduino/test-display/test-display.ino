#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Keep your existing address and dimensions.
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Wire.setSDA(4);  // GP4, physical pin 6
  Wire.setSCL(5);  // GP5, physical pin 7

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello World");
}

void loop() {
  // The LCD retains the text.
}