#include <LiquidCrystal_I2C.h>

// df.h
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
  SdFile* root;
  temperature_data() = default;
  temperature_data(SdFile* root, const char* path) : root(root), path(path)
  {
    file.open(*root, path, 0x1F);
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
    file.open(*root, path, 0x1F);
  }
  void close()
  {
    file.close();
  }
  temperature_point load_first()
  {
    clearPos();
    return load_next();
  }
  temperature_point load_last()
  {
    file.seekSet(file.fileSize() - sizeof(temperature_point));
    temperature_point point;
    file.read(&point, sizeof(temperature_point));
    return point;
  }
  double time_range()
  {
    auto a = load_first();
    auto b = load_last();
    clearPos();
    return b.time - a.time;
  }
};
#endif

// temperature_processing.h

#ifndef TEMPERATURE_PROCESSING_H
#define TEMPERATURE_PROCESSING_H

double timeAtTemperature(temperature_data& data, double target, double range)
{
    data.clearPos();
    double totalSeconds = 0;

    temperature_point previous = data.load_next();

    for (unsigned long i = 1; i < data.size(); i++)
    {
        temperature_point current = data.load_next();

        if (previous.temperature >= target + range ||
            previous.temperature <= target - range)
        {
            double duration = current.time - previous.time;

            if (duration > 0)
            {
                totalSeconds += duration;
            }
        }

        previous = current;
    }

    return totalSeconds;
}

#endif

const int displayLength = 20;
const int displayHeight = 4;

LiquidCrystal_I2C lcd(0x27, 20, 4);  // I2C address, columns, rows
const int center = 4;
const double ranges[] = {0.5, 4, 10};
const int rangesLength = sizeof(ranges) / sizeof(ranges[0]);

// plus-minus character byte array
unsigned char plusMinusChar[8] = {
  B00100,
  B00100,
  B11111,
  B00100,
  B00100,
  B00000,
  B11111,
  B00000
};

typedef struct {
  int hours;
  int minutes;
  int seconds;
} time_hms;

time_hms convertSeconds(int totalSeconds); 

time_hms convertSeconds(int totalSeconds) {
    time_hms t;
    t.hours = totalSeconds / 3600;
    t.minutes = (totalSeconds % 3600) / 60;
    t.seconds = totalSeconds % 60;
    return t;
}

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

temperature_data data;

void print_data(temperature_point p)
{
  Serial.print(p.time);
  Serial.print(": ");
  Serial.println(p.temperature);
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, plusMinusChar);  
  if (setup_sd())
  {
    Serial.println("Uh oh sd not work");
    return;
  }
  data = temperature_data(&root, "data.dat");
  Serial.print("Size: ");
  Serial.println(data.size());
  if (data.size() > 0)
  {
    for (int i = 0; i < 5; i++) print_data(data.load_next());
  }
}

void loop() {
  printTempLines(ranges);
  printTimeLine();
}

void printTempLines(const double ranges[]) {
  for (int i = 0; i < rangesLength; i++) {
    lcd.setCursor(0, i);
    printTempLine(ranges[i]);
  }
}

void printTempLine(double range) {
  char line[displayLength + 1]; 
  
  // temp
  char rangeText[12];
  dtostrf(range, 0, 1, rangeText);  // 1 digit after the decimal point
  int length = snprintf(line, sizeof(line), "%d%c%s%cC", center, 1, rangeText, 0xDF);

  // whitespace
  int timeLen = 8;
  int timePos = displayLength - timeLen;
  for (int i = length; i < timePos; i++) {
    line[i] = ' ';
  }

  // time
  time_hms t = convertSeconds(timeAtTemperature(data, center, range));
  snprintf(line + timePos, timeLen + 1, "%02d:%02d:%02d", t.hours, t.minutes, t.seconds);

  lcd.print(line);
}

void printTimeLine() {
  char line[displayLength + 1];

  // text
  int length = snprintf(line, sizeof(line), "TOTAL");

  // whitespace
  int timeLen = 8;
  int timePos = displayLength - timeLen;
  for (int i = length; i < timePos; i++) {
    line[i] = ' ';
  }

  // time
  time_hms t = convertSeconds(data.time_range());
  snprintf(line + timePos, timeLen + 1, "%02d:%02d:%02d", t.hours, t.minutes, t.seconds);

  lcd.setCursor(0, 3);
  lcd.print(line);
}