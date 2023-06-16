// 控制 LED 灯闪烁

#include <stdio.h>
#include <Arduino.h>

// 板载 2 颗 LED，占用 GPIO 12/13 引脚。
// https://wiki.luatos.com/chips/esp32c3/board.html#led
#define LED_BI1 12
#define LED_BI2 13

void setup() {
  Serial.begin(9600);
  // 基本指令，控制引脚模式。
  pinMode(LED_BI1, OUTPUT);
  pinMode(LED_BI2, OUTPUT);
}

void loop() {
  // 电子信号在传输时主要分为 2 种：
  //   - digital 数字信号
  //   - analog 模拟信号
  //
  // 数字信号，高精度，一般只有比特位 0 和 1，分别成为低电平、高电平，
  // 高电平有效或输出时说明需要以数字信号设置或读取为 1。
  //
  // 模拟信号，连续变化的电信号，能够表示一个范围的数据，精度降低。
  // 一些温度、湿度传感器可供读取模拟信号，实际上是电压数值，需要手动
  // 编程转化为可读数据。
  digitalWrite(LED_BI1, HIGH);
  delay(500);
  digitalWrite(LED_BI1, LOW);
  printf("LED built-in 1 blinked.\n");
  delay(1000);
  digitalWrite(LED_BI2, HIGH);
  delay(500);
  digitalWrite(LED_BI2, LOW);
  printf("LED built-in 2 blinked.\n");
  delay(1000);
}
