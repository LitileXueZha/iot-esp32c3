// 温湿度传感器 AHT-10
//
// 关键参数：3.3V，I²C 通讯
//    ----------------------------------
//        VIN     GND     SCL     SDA
//    -----↓-------↓-------↓-------↓----
//     电源 3.3V  接地  串行时钟  串行数据
//
// 工作范围：
//    - 温度 -40℃ ~ 85℃ ± 0.3   响应时间 8s
//    - 湿度 0 ~ 100 ± 2          响应时间 5-30s
//
// 长时间漂移 ≈5% 每年，需要复位一次。
// http://www.aosong.com/en/products-40.html
// http://www.aosong.com/userfiles/files/media/AHT10产品手册%20A3%2020201210.pdf
// https://github.com/enjoyneering/AHTxx

#include <stdio.h>
#include <stdint.h>
#include <Arduino.h>

// https://www.arduino.cc/reference/en/language/functions/communication/wire/
// https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2c.html
// I2C 模式下，时钟线和数据线都应该接一个上拉电阻，3.3v 下典型为 4.7kΩ
//
// Wire.endTransimission()
//    0: success.
//    1: data too long to fit in transmit buffer.
//    2: received NACK on transmit of address.
//    3: received NACK on transmit of data.
//    4: other error.
//    5: timeout
#include <Wire.h>

#define LOG_BAUD 115200
// 合宙 esp32c3 的 I2C 数据线和时钟线分别在 GPIO 4/5
#define I2C_SDA 4
#define I2C_SCL 5
#define AHT10_ADDRESS 0x38
#define AHT10_READ          0x71 // 读取传感器状态
#define AHT10_INIT          0xE1
#define AHT10_MEASURE_START 0xAC
#define AHT10_RESET         0xBA
#define AHT10_DELAY_BOOT 40
#define AHT10_DELAY_MEASURE 80
#define AHT10_DELAY_RESET 20
#define AHT10_READ_CAL(data)  ((data&0x04)>>2) // 校准状态 Bit[3]
#define AHT10_READ_MODE(data) ((data&0x60)>>4) // 工作模式 Bit[6:5]
#define AHT10_READ_BUSY(data) (data>>6)        // 空闲状态 Bit[7]
#define AHT10_RADIX 0x100000


void I2CScan();
void checkAlive();
void aht10_calibrate();
void aht10_measure(uint8_t);

void setup() {
  Serial.begin(LOG_BAUD);
  puts("setup");
  Wire.setPins(I2C_SDA, I2C_SCL);
  Wire.begin(); // Master Mode
  aht10_calibrate();
}

void loop() {
  // checkAlive();
  // I2CScan();
  aht10_measure(3);
  delay(5000);
}

void I2CScan() {
  char error, address;
  puts("Scanning for I2C devices...");
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      printf("I2C device address: %#x\n", address);
    } else if (error != 2) {
      printf("I2C error %d at address: %#x\n", error, address);
    }
  }
  puts("I2C scan done.");
}

void checkAlive() {
  uint8_t ret;
  Wire.beginTransmission(AHT10_ADDRESS);
  ret = Wire.endTransmission();
  if (ret == 0) {
    puts("Device alive.");
  }
}

void aht10_calibrate() {
  delay(AHT10_DELAY_BOOT);
  Wire.requestFrom(AHT10_ADDRESS, 1);
  char data = Wire.read();
  if (AHT10_READ_CAL(data) != 1) {
    Wire.beginTransmission(AHT10_ADDRESS);
    Wire.write(AHT10_INIT);
    Wire.write(0x08);
    Wire.write(0x00);
    char ret = Wire.endTransmission();
    if (ret == 0) {
      puts("AHT10 calibrate successful.");
    }
  }
}

void aht10_measure(uint8_t retries) {
  if (retries == -1) {
    puts("AHT10 always busy, reached max retries.");
    return;
  }
  Wire.beginTransmission(AHT10_ADDRESS);
  Wire.write(AHT10_MEASURE_START);
  Wire.write(0x33);
  Wire.write(0x00);
  char ret = Wire.endTransmission();
  if (ret != 0) {
    puts("AHT10 measure command failed.");
    return;
  }
  delay(AHT10_DELAY_MEASURE);
  Wire.requestFrom(AHT10_ADDRESS, 6);
  char data = Wire.read();
  if (AHT10_READ_BUSY(data) == 1) {
    aht10_measure(retries - 1);
    return;
  }
  char rawData[5];
  Wire.readBytes(rawData, 5);

  uint32_t temperature =
    (rawData[2] & 0x0f) << 16
    | rawData[3] << 8
    | rawData[4];
  uint32_t humidity =
    rawData[0] << 12
    | rawData[1] << 4
    | rawData[2] >> 4;
  if (humidity > AHT10_RADIX) humidity = AHT10_RADIX;
  uint8_t humd = (float)humidity / AHT10_RADIX *100;
  float temp = (float)temperature / AHT10_RADIX *200 - 50;
  printf("%.1f℃, %d%%RH\n", temp, humd);
}
