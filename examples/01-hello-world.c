// https://www.arduino.cc/reference/en/
#include <stdio.h>
#include <Arduino.h>

// Baud Rate 波特率，单位 bps，每秒传送二进制数 Bit Per Second。
// 又称调制速率，指的是将电子信号调制成有效数据的速率，可用于描述
// 信道上的传输速率。
//
// 由此可见，传输数据时，例如打印日志，需要选择对应的波特率。

// 出厂固件的日志波特率为 921600，不属于典型的波特率，很多串口监视器
// 默认都没有，可以手动设置日志波特率。
#define LOG_BAUD 9600

void setup() {
  // 控制台
  Serial.begin(LOG_BAUD);
}

void loop() {
  printf("Hello world, esp32c3!\n");
  delay(1000);
}
