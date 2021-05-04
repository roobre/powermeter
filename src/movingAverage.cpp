#include <movingAverage.h>

MovingAverage::MovingAverage(int nsamples) : nSamples(nsamples) {
    this->samples = new double[nsamples];

    for (int i = 0; i < nsamples; i++) {
        this->samples[i] = 0;
    }
}

void MovingAverage::sample(double value) {
    samples[curSample] = value;
    curSample = (curSample + 1) % nSamples;
}

double MovingAverage::average() {
    double avg = 0;
    for (int i = 0; i < nSamples; i++) {
        avg += samples[i] / nSamples;
    }
    return avg;
}
