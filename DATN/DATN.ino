#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "Ultrasonic.h"
#include "MQ02.h"
#include "MLX90614.h"

#define BLYNK_TEMPLATE_ID "TMPL6LPDF7nIH"
#define BLYNK_TEMPLATE_NAME "WARNING"
#define BLYNK_AUTH_TOKEN "1BqZKZUc4WBQPTNfwpuWyr_z2d7hl9lO"
#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "hongphuong";
char pass[] = "abcde123";

#define Chedo               V0
#define Nongdokhoi          V1
#define Nguongcanhbaokhoi   V2
#define Thoigiancanhbao     V3
#define Dieukhienbep        V4

#define echo_pin 2
#define trigger_pin 15

#define mq02_pin 35

#define sda_pin 21
#define scl_pin 22

#define buzzer 19

#define button1 12
#define button2 13

#define relay 14

#define MAX_DISTANCE 30

bool detectPeople;
bool isTurningOn;

unsigned long startTimer = 0;
unsigned long warningTimer = 0;
bool startWarning = false;
bool smokeWarning = false;

unsigned long distanceTimer = 0;
bool distanceStart = false;

int nguongcanhbao = 15000;
int thoigiancanhbao = 1;

bool stateButton1 = HIGH;
bool relayMode = 0;           
bool relayState = 0;

bool stateButton2 = HIGH;
bool runMode = 1;      
bool canhbaoState = 1;  

int count = 0;

Ultrasonic Ultrasonic(trigger_pin, echo_pin);
MQ02 MQ02(mq02_pin);
MLX90614 MLX90614(sda_pin, scl_pin);

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    count += 1;
    delay(500);
    Serial.print(".");
    if(count >= 20) break;
  }
  if(WiFi.status() == WL_CONNECTED){
    Blynk.begin(auth, ssid, pass);
    Serial.println("\nWiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
}

void loop() {
  Blynk.run();

  checkButton1();
  checkButton2();

  float distance = Ultrasonic.distanceRead();
  float smokeLevel = MQ02.mq02ValueRead();
  float tempObject = MLX90614.tempObjectRead();
  float tempAmbient = MLX90614.tempAmbientRead();

  Serial.print("Khoang cach giua nguoi va bep: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Nong do khoi: ");
  Serial.print(smokeLevel);
  Serial.println(" ppm");

  Serial.print("Nguong canh bao khoi: ");
  Serial.print(nguongcanhbao);
  Serial.println(" ppm");

  Serial.print("Thoi gian canh bao: ");
  Serial.print(thoigiancanhbao);
  Serial.println(" phut");

  Serial.print("Nhiet do mat bep: ");
  Serial.print(tempObject);
  Serial.println(" do C");

  Serial.print("Nhiet do moi truong: ");
  Serial.print(tempAmbient);
  Serial.println(" do C");
  Serial.println("");

  // Gửi giá trị tới Blynk
  Blynk.virtualWrite(Nongdokhoi, smokeLevel);
  Blynk.virtualWrite(Thoigiancanhbao, thoigiancanhbao);
  Blynk.virtualWrite(Nguongcanhbaokhoi, nguongcanhbao);

  checkButton1();
  checkButton2();

  if(runMode == 1){
    unsigned long currentTime = millis();
    if (distance < MAX_DISTANCE) {
      if (!distanceStart) {
        distanceStart = true;
        distanceTimer = currentTime;
      } else if (currentTime - distanceTimer >= 2000) {
        detectPeople = true;
        Serial.println("Co nguoi o bep.");
        distanceStart = false;
      }
    } else {
      distanceStart = false;
      detectPeople = false;
      Serial.println("Khong co nguoi o bep !!!");
    }

    if((tempObject - tempAmbient) > 5){
      isTurningOn = true;
      Serial.println("Bep dang bat !!!");
    }else{
      isTurningOn = false;
      Serial.println("Bep dang khong bat !!!");
    } 

    checkButton1();
    checkButton2();

    if (isTurningOn && !detectPeople) {
      if (startTimer == 0) {
        startTimer = millis();
      }
      currentTime = millis();
      if (currentTime - startTimer >= (thoigiancanhbao*60000) && !startWarning) {
        digitalWrite(buzzer, HIGH);
        Serial.println("CANHBAO: Da khong phat hien nguoi trong " + String(thoigiancanhbao) +  " phut !");
        Blynk.logEvent("warning", String("Cảnh báo! Bếp vẫn bật mà không có người giám sát. Tự động tắt bếp sau 1 phút !!!"));
        startWarning = true;
        warningTimer = millis();
      }
      currentTime = millis();
      if (currentTime - warningTimer >= 30000 && startWarning) {
        digitalWrite(relay, LOW);
        Blynk.virtualWrite(Dieukhienbep, LOW);
        Serial.println("CANHBAO: Da khong phat hien nguoi trong " + String(thoigiancanhbao + 1) + " phut !");
        Blynk.logEvent("warning", String("Cảnh báo! Bếp vẫn bật mà không có người giám sát quá thời gian cho phép. Hệ thống tự động tắt bếp !!!"));
        Serial.println("Da tat bep !!!");
        digitalWrite(buzzer, LOW);
      }
    } else {
      startTimer = 0;
      startWarning = false;
      digitalWrite(buzzer, LOW);
    }

    checkButton1();
    checkButton2();

    if (smokeLevel > nguongcanhbao && !detectPeople) {
      digitalWrite(buzzer, HIGH);
      smokeWarning = true;
      Serial.println("CANHBAO: Nong do khoi vuot qua muc cho phep !!!");
      Blynk.logEvent("warning", String("Cảnh báo! Nồng độ khói = " + String(smokeLevel) + " ppm vượt quá ngưỡng cho phép!"));
    } else {
      if (!startWarning) {
        digitalWrite(buzzer, LOW);
      }
    }

    if(tempAmbient >= 50){
      digitalWrite(buzzer, HIGH);
      Serial.println("CANHBAO: Nhiet do vuot qua muc cho phep !!!");
      Blynk.logEvent("warning", String("Cảnh báo! Nhiệt độ = " + String(tempAmbient) + " độ C vượt quá ngưỡng cho phép!"));
    }else{
      if(!startWarning && !smokeWarning){
        digitalWrite(buzzer, LOW);
      }
    }


  } else{
    startTimer = 0;
    startWarning = false;
    smokeWarning = false;
    digitalWrite(buzzer, LOW);
  }
  checkButton1();
  checkButton2();
  delay(1000);
}

void checkButton1() {
  if (digitalRead(button1) == LOW) {
    if (stateButton1 == HIGH) {
      stateButton1 = LOW;
      relayMode = !relayMode;
      Blynk.virtualWrite(Dieukhienbep, relayMode);
      digitalWrite(relay, !digitalRead(relay));
      delay(200);
    }
  } else {
    stateButton1 = HIGH;
  }
}

void checkButton2() {
  if(digitalRead(button2) == LOW){
    if(stateButton2 == HIGH){
      stateButton2 = LOW;
      runMode = !runMode;
      Blynk.virtualWrite(Chedo,runMode);
      delay(200);
    }
  }else{
    stateButton2 = HIGH;
  }
}

BLYNK_WRITE(Chedo){
  runMode = param.asInt();
}

BLYNK_WRITE(Nguongcanhbaokhoi){
  nguongcanhbao = param.asInt();
}

BLYNK_WRITE(Thoigiancanhbao){
  thoigiancanhbao = param.asInt();
}

BLYNK_WRITE(Dieukhienbep){
  relayMode = param.asInt();
  digitalWrite(relay, relayMode);
}