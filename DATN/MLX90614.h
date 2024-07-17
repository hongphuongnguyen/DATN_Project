#ifndef MLX90614_H
#define MLX90614_H

#include <Arduino.h>

// Định nghĩa địa chỉ I2C của cảm biến
#define MLX90614_I2CADDR 0x5A
// Định nghĩa các thanh ghi của cảm biến
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07

class MLX90614 {
  public:
    // Constructor
    MLX90614(int SDA_Pin, int SCL_Pin);

    float tempObjectRead();
    float tempAmbientRead();

  private:
    int _SDA_Pin;
    int _SCL_Pin;

    void i2c_start();
    void i2c_stop();
    void i2c_write(uint8_t data);
    uint8_t i2c_read(bool ack);
};

#endif
