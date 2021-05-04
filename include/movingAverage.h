#pragma once

// Stores a moving average of double values
class MovingAverage {
public:
    explicit MovingAverage(int samples = 8);
    void sample(double value);
    double average();

private:
    double* samples;
    int nSamples;
    int curSample{};
};
