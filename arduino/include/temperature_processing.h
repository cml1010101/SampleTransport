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