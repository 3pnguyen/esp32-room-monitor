#include "custom-classes/Debounce.h"

template <class T>
Debounce<T>::Debounce(T initialState) {
  firstValue = initialState;
  secondValue = initialState;
}

template <class T>
Debounce<T>::Debounce(T initialState, T secondState) {
  firstValue = initialState;
  secondValue = secondState;
}

template <class T>
bool Debounce<T>::hasChanged(T currentState) {
  if (currentState == firstValue && currentState == secondValue) {
    return false;
  }

  secondValue = firstValue;
  firstValue = currentState;
  return true;
}

template <class T>
bool Debounce<T>::lastState(T state) {
  return (state == firstValue);
}

template class Debounce<int>;
template class Debounce<bool>;
template class Debounce<float>;