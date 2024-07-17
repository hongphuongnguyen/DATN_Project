#include "MQ02.h"

MQ02::MQ02(int Analog_Pin){
  _Analog_Pin = Analog_Pin;

  pinMode(_Analog_Pin, INPUT);
}

float MQ02::mq02ValueRead() {
  int value = analogRead(_Analog_Pin);
  // float voltage = value * (5.0 / 4095.0); // Chuyển đổi giá trị đọc được thành điện áp
  // float Rs = ((5.0 * 10.0) / voltage) - 10.0; // Tính toán điện trở của cảm biến

  // // Tính toán tỷ lệ Rs/R0
  // float ratio = Rs / 10.0;

  // // Sử dụng đồ thị đặc tính để chuyển đổi Rs/R0 thành nồng độ khói (ppm)
  // // Giá trị m và b từ đồ thị đặc tính của cảm biến cho khói
  // float b = 3; // Thay đổi tùy theo đặc tính của cảm biến
  // float m = -1; // Thay đổi tùy theo đặc tính của cảm biến
  // float ppm = b*pow(ratio, m);

  int ppm;
  //Serial.println(value);
  if(value < 2200){
    ppm = map(value, 0, 2200, 1, 300);
  }
  else if(value > 2200 && value < 3000){
    ppm = map(value, 2200, 3000, 350, 5000);
  }
  else{
    ppm = map(value, 3000, 4094, 5000, 12000);
  }

  return ppm;
}
