//nonblocking timer that tracks the time between intervals

#ifndef INTERVALTIMER_H
#define INTERVALTIMER_H

class IntervalTimer {
  private:
    unsigned long lastTime = 0;
    unsigned long interval;

  public:
    IntervalTimer(unsigned long intervalMs);
    bool isReady();
    void reset();
};

#endif