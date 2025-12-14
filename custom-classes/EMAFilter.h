//digital filter to reduce sensor noise

#ifndef EMAFILTER_H
#define EMAFILTER_H

template <typename T>
class EMAFilter {
  public:
    T level = T{};
    const float alpha;
    const float alpha2;
    const T threshold;

    EMAFilter(float smoothingFactor, T thresholdValue);
    EMAFilter(float alphaHigh, float alphaLow, T thresholdValue);
    void calculate(T input);
    void reset();
    bool aboveThreshold ();
};

#endif