#ifndef DF_H
#define DF_H
struct temperature_point
{
  double time;
  double temperature;
};
struct temperature_data
{
  temperature_point* data;
  unsigned long length;
  temperature_point& operator[](unsigned int index)
  {
    return data[index];
  }
}
#endif