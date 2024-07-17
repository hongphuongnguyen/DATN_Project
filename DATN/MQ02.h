#ifndef MQ02_H
#define MQ02_H

#include <Arduino.h>

class MQ02 {
  public:
    // Constructor
    MQ02(int Analog_Pin);
    float mq02ValueRead();

  private:
    int _Analog_Pin;
};
#endif