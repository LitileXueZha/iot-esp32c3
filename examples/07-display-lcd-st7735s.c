// 1.8 寸全彩 lcd 显示屏 ST7735S
//
// 关键参数：3.3v，SPI 通讯，屏幕像素 128*160
//
//     GND   VCC        SCL    SDA   RES   DC           CS  BL
//    -----------------------------------------------------------
//     接地  3.3v 电源  时钟线  MOSI  重置  MISO(排除法)  CS  背光
//
// SPI 标准很松散，国内淘的设备找不到接线图的话，参考标准然后使用排除法。
// 简单了解下 4 线 SPI：
//    - SCLK 时钟线
//    - MISO 输入数据线
//    - MOSI 输出数据线
//    - CS 片选线。低电平指明开始传输数据
//
// 多个 SPI 设备时，往往时钟线和两条数据线是公用的，每个设备的 CS 线
// 需要接到不同引脚上，抽象理解是 I2C 协议中的设备地址。
//
// 有的液晶屏除了用于标准的 SPI 4 线外，还有其它引脚以供设置，例如：
//    - RES 复位。感觉像是多余，开发板上已经有复位键了，用于生产环境吧。
//    - BL 背光。指明是否关闭显示器
// 这些都是简单的 GPIO 引脚，直接使用数据信号即可。悬空亦可。
//
// 此设备说明：
//    - DC 数据\命令，应该是从 esp32c3 发送数据到显示屏
//    - SDA 数据线，也许是 esp32c3 接收数据，但是商品详情图写的 MOSI，
//      需要实际测试。
//
// 对比 I2C 协议，还是 I2C 更简洁，占用引脚更少，编程方式更佳，不过 SPI
// 不需要上拉电阻。
//
// 在 U8g2 库中，SPI 的数据线叫法是 MOSI/DC，事实上标准里的线名挺迷惑的，
// 还是更趋向于这种有区分度的命名。
//
// 合宙 esp32c3 把 SPI_MISO 接成了 RST，中间一个未命名的通用的 GPIO
// 接成了 DC，很迷惑。。。不过有用就行了。
//
// https://en.wikipedia.org/wiki/Serial_Peripheral_Interface
// https://docs.arduino.cc/learn/communication/spi

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <Arduino.h>
#include <SPI.h>
// 配置参考
// https://github.com/Bodmer/TFT_eSPI/blob/master/User_Setup.h
// #include <User_Setup.h>
// 为了避免各种奇怪的问题，还是得按照作者建议的方式，直接到源码库里修改配置，
// 虽然这种配置方式也很奇怪。。。
//
// #define USER_SETUP_LOADED
#define ST7735_DRIVER       // 记得注释掉 <User_Setup.h> 里默认的
#define TFT_WIDTH  128
#define TFT_HEIGHT 160
#define ST7735_GREENTAB2    // 屏幕不同，可能需要尝试不同配置，不白屏即可
#define TFT_SCLK 2
#define TFT_MOSI 3
#define TFT_MISO -1
#define TFT_DC   6
#define TFT_CS   7          // 只有一个设备的话，片选线可以悬空
#define TFT_RST  10         // 板载复位键无效，必须接到 GPIO 上
#define LOAD_GLCD
#define LOAD_FONT2
#define SMOOTH_FONT
#define SPI_FREQUENCY       40000000
// 使用字体
//  - 默认的 Font 1 为 8px，有点小。库里面所有的字体都只有 94 个字符，比
//    ASCII 编码还少，更别说支持中文。
//  - Font 2,4,6,7,8 在 <User_Setup.h> 里可以看到。
//  - GFX 字体（12pt = 16px）
//    https://github.com/Bodmer/TFT_eSPI/blob/master/Fonts/GFXFF/gfxfont.h
//
// GFX 字体系列不好用，非等宽，而且偏移，行高也有问题。
#undef LOAD_GFXFF
//
// 转一套兼容的字体太浪费时间，需要花时间研究，图标还是直接用图片吧。
// 中文可以转成点阵 bitmap。
//
// 平滑字体需要上传字体文件到开发板 SPI Flash 中，具体操作懒得研究了，所以
// 直接关闭平滑字体。
#undef SMOOTH_FONT
#include <TFT_eSPI.h>

#include "i-bitmap.h"

#define LOG_BAUD 115200
#define LED 12
#define RGB565(r,g,b) ((r>>3)<<11|(g>>2)<<5|b>>3)
// https://litilexuezha.github.io/primitives/colors
#define CANVAS RGB565(13,17,23)
#define FG RGB565(230,237,243)
#define FG2 RGB565(110, 118, 129)
#define BORDER RGB565(48,54,61)
#define ACCENT RGB565(47,129,247)
#define SUCCESS RGB565(63, 185, 80)
#define DANGER RGB565(248, 81, 73)
#define MAX_CHAR 94

TFT_eSPI tft = TFT_eSPI();
char printChars[MAX_CHAR];
void clearScreen();

void setup() {
  Serial.begin(LOG_BAUD);
  puts("setup");
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  tft.init();
  // puts("initialized");
  tft.setRotation(3);
  tft.setSwapBytes(1);
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.setTextPadding(0);
  tft.setBitmapColor(FG, CANVAS);
  // 支持显示的字符，编码范围 0x20~0x7e
  for (int i = 0; i < MAX_CHAR; i++) {
    printChars[i] = 0x20 + i;
  }
}

void loop() {
  puts("loop...");
  digitalWrite(LED, HIGH);
  tft.fillScreen(CANVAS);
  // tft.setAttribute(UTF8_SWITCH, 1);
  tft.setTextColor(FG);
  tft.drawString("Hello, world!", 0, 0);
  // tft.setFreeFont(&FreeMono9pt7b);
  // tft.drawString("Hello, FreeMono!", 0, 16);
  // tft.setFreeFont(&FreeSans12pt7b);
  // tft.drawString("Hello, FreeSans!", 0, 32);
  // tft.setFreeFont(&FreeSerif12pt7b);
  // tft.drawString("Hello, FreeSerif!", 0, 48);
  tft.drawFastHLine(0, 16-1, 160, BORDER);
  // tft.setCursor(0, 16);
  // tft.println(printChars);
  tft.drawRect(0, 16+2, 24, 16, ACCENT);
  tft.drawRoundRect(0, 32+4, 24, 16, 4, SUCCESS);
  tft.drawSmoothRoundRect(0, 48+6, 4, 4, 24, 16, DANGER, DANGER);
  tft.fillRectVGradient(24+4, 16+2, 24, 16, RGB565(189, 86, 29), RGB565(90, 30, 2));
  tft.fillSmoothCircle(24+4+12, 32+4+8, 8, SUCCESS, SUCCESS);
  tft.fillEllipse(24+4+12, 48+6+8, 12, 8, DANGER);
  tft.fillCircle(48+8+12, 16+2+8, 8, ACCENT);
  tft.drawSpot(48+8+6, 32+4+8, 2, ACCENT, ACCENT);
  tft.drawSpot(48+8+12, 32+4+8, 2, SUCCESS, SUCCESS);
  tft.drawSpot(48+8+18, 32+4+8, 2, DANGER, DANGER);
  tft.drawTriangle(48+8+12,48+6, 48+8+18,48+6+7, 48+8+6,48+6+16, DANGER);
  tft.pushImage(80, 32, 80, 64, liu_yifeng_80x64);
  tft.setTextColor(FG2);
  tft.drawCentreString("Liu YiFeng", 120, 96+2, 1);
  tft.drawBitmap(0, 80-2, hello_world_x, 16*5, 16, SUCCESS);
  tft.pushImage(0, 96, 16, 16, ic_sunny_16);
  tft.pushImage(16+2, 96, 16, 16, ic_rainy_16);
  tft.pushImage(32+4, 96, 16, 16, ic_cloudy_16);
  tft.drawBitmap(0, 112, ic_sunny_16_x, 16, 16, ACCENT);
  tft.drawBitmap(16+2, 112, ic_rainy_16_x, 16, 16, SUCCESS);
  tft.drawBitmap(32+4, 112, ic_cloudy_16_x, 16, 16, DANGER);
  // tft.drawCentreString("weather", 24, 112, 2);
  tft.drawBitmap(64-8, 112, celsius_x, 16, 16, SUCCESS);
  // tft.drawBitmap(0, 112, (uint8_t*)ic_sunny_16, 16, 16, FG, CANVAS);
  tft.setTextColor(ACCENT);
  tft.drawNumber(1234, 80, 112);
  tft.setTextWrap(1);
  tft.drawFloat(1.618, 3, 120, 112);
  tft.setTextWrap(0);
  digitalWrite(LED, LOW);
  delay(3500);
  clearScreen();
  tft.pushImage(0, 0, 160, 128, liu_yifeng_160x128);
  delay(1500);
  // clearScreen();
  // tft.invertDisplay(1);
  // tft.drawXBitmap(0, 0, liu_yifeng_160x128_x, 160, 128, FG);
  // delay(2500);
  // tft.invertDisplay(0);
}

void clearScreen() {
  tft.setRotation(2* random(2));
  tft.fillScreen(CANVAS);
  tft.setRotation(3);
}

void checkSPIPins() {
  SPIClass spi = SPIClass(HSPI);
  spi.begin(TFT_SCLK, TFT_RST, TFT_MOSI, TFT_CS);
  pinMode(TFT_CS, OUTPUT);
  delay(50);
  spi.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(TFT_CS, LOW);
  spi.transfer(0x01);
  digitalWrite(TFT_CS, HIGH);
  spi.endTransaction();
  puts("SPI end tranmission");
}
