#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 4);  // I2C address, columns, rows
const int PAGE_DELAY = 5000;

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  display("Hello World");
  delay(1000);

  display("SampleSafe");
  delay(1000);
}

void display(char* text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
}