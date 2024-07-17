#include "MLX90614.h"

MLX90614::MLX90614(int SDA_Pin, int SCL_Pin){
  _SDA_Pin = SDA_Pin;
  _SCL_Pin = SCL_Pin;

  pinMode(_SDA_Pin, OUTPUT);
  pinMode(_SCL_Pin, OUTPUT);
}

void MLX90614::i2c_start(){
  digitalWrite(_SDA_Pin, HIGH);
  digitalWrite(_SCL_Pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(_SDA_Pin, LOW);
  delayMicroseconds(5);
  digitalWrite(_SCL_Pin, LOW);
  delayMicroseconds(5);
}

void MLX90614::i2c_stop(){
  digitalWrite(_SDA_Pin, LOW);
  digitalWrite(_SCL_Pin, LOW);
  delayMicroseconds(5);
  digitalWrite(_SCL_Pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(_SDA_Pin, HIGH);
  delayMicroseconds(5);
}

void MLX90614::i2c_write(uint8_t data) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(_SDA_Pin, (data & 0x80) ? HIGH : LOW);
    digitalWrite(_SCL_Pin, HIGH);
    delayMicroseconds(5);
    digitalWrite(_SCL_Pin, LOW);
    delayMicroseconds(5);
    data <<= 1;
  }
  pinMode(_SDA_Pin, INPUT);
  digitalWrite(_SCL_Pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(_SCL_Pin, LOW);
  delayMicroseconds(5);
  pinMode(_SDA_Pin, OUTPUT);
}

uint8_t MLX90614::i2c_read(bool ack) {
  uint8_t data = 0;
  pinMode(_SDA_Pin, INPUT);
  for (int i = 0; i < 8; i++) {
    data <<= 1;
    digitalWrite(_SCL_Pin, HIGH);
    delayMicroseconds(5);
    if (digitalRead(_SDA_Pin)) {
      data |= 0x01;
    }
    digitalWrite(_SCL_Pin, LOW);
    delayMicroseconds(5);
  }
  pinMode(_SDA_Pin, OUTPUT);
  digitalWrite(_SDA_Pin, ack ? LOW : HIGH);
  digitalWrite(_SCL_Pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(_SCL_Pin, LOW);
  delayMicroseconds(5);
  digitalWrite(_SDA_Pin, HIGH); // Release the line
  return data;
}

float MLX90614::tempObjectRead() {
  i2c_start();
  i2c_write(MLX90614_I2CADDR << 1); // Địa chỉ thiết bị và bit ghi
  i2c_write(MLX90614_TOBJ1); // Địa chỉ thanh ghi nhiệt độ đối tượng
  i2c_start();
  i2c_write((MLX90614_I2CADDR << 1) | 0x01); // Địa chỉ thiết bị và bit đọc
  uint8_t lsb = i2c_read(true);
  uint8_t msb = i2c_read(true);
  uint8_t pec = i2c_read(false);
  i2c_stop();
  
  float temp = ((msb << 8) | lsb) * 0.02 - 273.15;
  return temp;
}

float MLX90614::tempAmbientRead() {
  i2c_start();
  i2c_write(MLX90614_I2CADDR << 1); // Địa chỉ thiết bị và bit ghi
  i2c_write(MLX90614_TA); // Địa chỉ thanh ghi nhiệt độ môi trường
  i2c_start();
  i2c_write((MLX90614_I2CADDR << 1) | 0x01); // Địa chỉ thiết bị và bit đọc
  uint8_t lsb = i2c_read(true);
  uint8_t msb = i2c_read(true);
  uint8_t pec = i2c_read(false);
  i2c_stop();
  
  float temp = ((msb << 8) | lsb) * 0.02 - 273.15;
  return temp;
}