// start df.h
#ifndef DF_H
#define DF_H
#include <SPI.h>
#include <SD.h>
struct temperature_point
{
  double time;
  double temperature;
};
struct temperature_data
{
  SdFile file;
  const char* path;
  SdFile& root;
  temperature_data(SdFile& root, const char* path) : root(root), path(path)
  {
    file.open(root, path, 0x1F);
  }
  temperature_point load_next()
  {
    temperature_point point;
    file.read(&point, sizeof(temperature_point));
    return point;
  }
  int size()
  {
    return file.fileSize() / sizeof(temperature_point);
  }
  void clearPos()
  {
    file.seekSet(0);
  }
  void append_new_data(temperature_point point)
  {
    file.seekEnd();
    file.write(&point, sizeof(temperature_point));
  }
  void clear()
  {
    file.remove();
    file.open(root, path, 0x1F);
  }
  void close()
  {
    file.close();
  }
};
#endif
// end df.h
constexpr int CHIP_SELECT_PIN = 10;
SdFile root;
Sd2Card card;
SdVolume volume;
int setup_sd()
{
  int ret;
  if (!(ret = card.init(SPI_HALF_SPEED, CHIP_SELECT_PIN)))
  {
    Serial.print("Error during card.init(): ");
    Serial.println(ret);
    return ret;
  }
  if (!(ret = volume.init(card)))
  {
    Serial.print("Error during volume.init(): ");
    Serial.println(ret);
    return ret;
  }
  if (!(ret = root.openRoot(&volume)))
  {
    Serial.print("Error during root.openRoot(): ");
    Serial.println(ret);
    return ret;
  }
  return 0;
}
double randd()
{
  return (double)random() / RANDOM_MAX;
}
double rand2d()
{
  return randd() * 2 - 1;
}
void setup() {
  Serial.begin(9600);
  if (setup_sd())
  {
    Serial.println("Could not set up SD card");
    return;
  }
  temperature_data data(root, "data.dat");
  data.clear();
  unsigned int N = 1000;
  for (int i = 0; i < N; i++)
  {
    data.append_new_data({i * 5 + rand2d() * 0.05, 4 + rand2d()});
  }
  data.clearPos();
  for (int i = 0; i < data.size(); i++)
  {
    auto dp = data.load_next();
    Serial.print(dp.time);
    Serial.print(": ");
    Serial.println(dp.temperature);
  }
  data.close();
}

void loop() {
  // put your main code here, to run repeatedly:
}
