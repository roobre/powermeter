#pragma once

#include <EmonLib.h>

struct sample {
    double value;
    unsigned long sample_time;
};

class CurrentSampler {
public:
    CurrentSampler();
    CurrentSampler(int pin, double ampsPerVolt);
    void begin(int pin, double ampsPerVolt);
    bool ready() const;
    struct sample sample();

    unsigned long sampleRate;
    unsigned long microSamples;

    static const unsigned long DEFAULT_SAMPLERATE = 3000;
    static const unsigned long DEFAULT_MICROSAMPLES = 1480;

private:
    EnergyMonitor emon;
    unsigned long lastSample;
};
