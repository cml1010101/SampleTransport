
#ifndef TEMPERATURE_PROCESSING_H
#define TEMPERATURE_PROCESSING_H

#include "df.h"

temperature_point load_next();

double timeAtTemperature(double target, double range, unsigned long numberOfReadings)
{
    double totalSeconds = 0;

    if (range < 0 || numberOfReadings < 2)
    {
        return 0;
    }

    temperature_point previous = load_next();

    for (unsigned long i = 1; i < numberOfReadings; i++)
    {
        temperature_point current = load_next();

        if (previous.temperature >= target - range &&
            previous.temperature <= target + range)
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