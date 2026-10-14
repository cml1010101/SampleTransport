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