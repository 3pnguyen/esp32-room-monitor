//simple, on-off, true-false, state-detection class. Used to send light and motion info through MQTT only when they change state

#ifndef DEBOUNCE_H
#define DEBOUNCE_H

template <typename T>
class Debounce {
  public:
    T firstValue;
    T secondValue;

    Debounce(T initialState);
    Debounce(T initialState, T secondState);
    bool hasChanged(T currentState);
    bool lastState(T state);
};

#endif