// 温度传感器 DS18B20
//
// 关键参数：3v~5.5v，单总线通讯
//      --------------------
//      ﹙ GND   DQ    VDD﹚
//      ﹙  ↓    ↓      ↓ ﹚
//        接地 数据线 电源 3.3v
//
// 工作范围：-55℃ ~ 125℃
//          -10℃ ~ 70℃ ± 0.4
//
// OneWire 需要数据线上拉电阻。
//
// https://www.arduino.cc/reference/en/
// https://github.com/PaulStoffregen/OneWire
// https://github.com/milesburton/Arduino-Temperature-Control-Library
// https://www.umw-ic.com/static/pdf/9040af7892e8300bacfc7bdfd93e8ef7.pdf
//
// 切记，别直接上 5v 电到电源线，不然冒烟了别怪我没提醒你。还有，真冒烟了千万别
// 用手去摸，真的很烫 😂
//
// 建议电源线接地，数据线连上就有电，用所谓的寄生模式。

#include <stdio.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#define LOG_BAUD 115200
#define GPIO_ONEWIRE 6
#define GPIO_LED 12

void scanDevices();
// c++ 语法
OneWire ds(GPIO_ONEWIRE);
DallasTemperature sensors(&ds);

void setup() {
    Serial.begin(LOG_BAUD);
    puts("setup");
    pinMode(GPIO_LED, OUTPUT);
    ds.reset();
    sensors.begin();
    digitalWrite(GPIO_LED, LOW);
    printf("Found %d devices.\n", sensors.getDeviceCount());
}

void loop() {
    // scanDevices();
    sensors.requestTemperatures();
    float temp1 = sensors.getTempCByIndex(0);
    printf("Temperature(0) is %.1f℃\n", temp1);
    printf("Temperature(1) is %f℃\n", sensors.getTempCByIndex(1));
    delay(2000);
}

void scanDevices() {
    byte address[8], ret;

    ret = ds.search(address);
    if (ret == 0) {
        ds.reset_search();
        puts("No more OneWire devices.");
        delay(250);
        return;
    }
    printf("R=");
    for (int i = 0; i < 8; i++) {
        printf("%02X ", address[i]);
    }
    if (OneWire::crc8(address, 7) != address[7]) {
        puts("\tCRC check failed.");
        return;
    }
    putchar('\n');
    delay(50);
}
