// 声音传感器 KY-038
//
// 关键参数：5v，AO+DO 输出
//    --------------------------------
//        A0      G      +       D0
//    ----↓-------↓------↓-------↓----
//     模拟信号  接地  电源 5v  数字信号
//
// https://kirig.ph/wp-content/uploads/2020/08/KY-038-Joy-IT.pdf
// https://sensorkit.joy-it.net/en/sensors/ky-038
//
// 这是个垃圾，浪费几块钱了。灵敏度非常差，需要非常近距离的声源才有明显的差异。
// 还不如用手机装个 app 麦克风检测，输出的模拟信号根本无法用于编程分贝噪音。

#include <stdio.h>
#include <Arduino.h>

#define LOG_BAUD 115200
#define GPIO_AO 0
#define GPIO_DO 1
#define LED_BI1 12
#define LED_BI2 13

void setup() {
    Serial.begin(LOG_BAUD);
    puts("setup");
    pinMode(LED_BI1, OUTPUT);
    pinMode(LED_BI2, OUTPUT);
    digitalWrite(LED_BI1, LOW);
    digitalWrite(LED_BI2, LOW);
    pinMode(GPIO_DO, INPUT);
    analogReadResolution(10);
}

void loop() {
    int sig = analogRead(GPIO_AO);
    float noise = 5 - sig *5.0/1023; // 5v, 10 bit
    int highLoud = digitalRead(GPIO_DO);
    printf("AO: %fv, %d DO: %d\n", noise, 1023-sig, highLoud);
    delay(200);
}
