#include <currentSampler.h>
#include <Arduino.h>

CurrentSampler::CurrentSampler() : emon() {
    lastSample = 0;
    sampleRate = DEFAULT_SAMPLERATE;
    microSamples = DEFAULT_MICROSAMPLES;
}

CurrentSampler::CurrentSampler(int pin, double ampsPerVolt) : CurrentSampler() {
    this->begin(pin, ampsPerVolt);
}

void CurrentSampler::begin(int pin, double ampsPerVolt) {
    emon.current(pin, ampsPerVolt);
}

struct sample CurrentSampler::sample() {
    lastSample = millis();
    double irms = emon.calcIrms(microSamples);

    struct sample s {
        .value = irms,
        .sample_time = millis() - lastSample,
    };

    return s;
}

bool CurrentSampler::ready() const {
    return millis() - lastSample > sampleRate;
}
