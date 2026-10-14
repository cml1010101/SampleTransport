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
  temperature_data(SdFile& root, const char* path)
  {
    file.open(root, path, 0x1F)
  }
  temperature_point load_next()
  {
    temperature_point point;
    file.read(&point, sizeof(temperature_point));
    return point;
  }
  int size()
  {
    return file.size() / sizeof(temperature_point);
  }
  void clearPos()
  {
    file.seekSet(0);
  }
  void append_new_data(const temperature_point& point)
  {
    file.seekEnd();
    file.write(&point, sizeof(temperature_point));
  }
}
#endif
// end df.h