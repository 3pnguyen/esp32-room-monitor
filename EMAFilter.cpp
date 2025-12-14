#include "custom-classes/EMAFilter.h"

template <class T>
EMAFilter<T>::EMAFilter(float smoothingFactor, T thresholdValue) : alpha(smoothingFactor), alpha2(smoothingFactor), threshold(thresholdValue) {}

template <class T>
EMAFilter<T>::EMAFilter(float alphaHigh, float alphaLow, T thresholdValue) : alpha(alphaHigh), alpha2(alphaLow), threshold(thresholdValue) {}

template <class T>
void EMAFilter<T>::calculate(T input) {
  if (alpha == alpha2) {
    level = (input * alpha) + (level * (1 - alpha));
  } else {
    float selectedAlpha = (input > level) ? alpha : alpha2;
    level = (input * selectedAlpha) + (level * (1 - selectedAlpha));
  }
}

template <class T>
void EMAFilter<T>::reset() {
  level = T{};
}

template <class T>
bool EMAFilter<T>::aboveThreshold () {
  return level > threshold;
}

template class EMAFilter<int>;
template class EMAFilter<bool>;
template class EMAFilter<float>;