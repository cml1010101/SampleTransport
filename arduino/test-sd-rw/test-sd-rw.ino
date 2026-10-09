#include <SPI.h>
#include <SD.h>
constexpr int CHIP_SELECT_PIN = 10;
Sd2Card card;
SdVolume volume;
SdFile root;
void setup() {
  Serial.begin(9600);
  if (!card.init(SPI_HALF_SPEED, CHIP_SELECT_PIN))
  {
    Serial.println("Error: could not init card");
    return;
  }
  if (!volume.init(card))
  {
    Serial.println("Error: could not find volume on card");
    return;
  }
  if (!root.openRoot(&volume))
  {
    Serial.println("Error could not find root file");
    return;
  }
  SdFile hello;
  if (!hello.open(&root, "hello.txt", 0x1F))
  {
    Serial.println("Failure on open for write");
    return;
  }
  hello.write("Hello World!");
  hello.close();
  if (!hello.open(&root, "hello.txt", 0x1))
  {
    Serial.println("Failure on open for read");
    return;
  }
  char buf[13];
  hello.read(&buf[0], 12);
  Serial.println(buf);
  Serial.println("Success!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
