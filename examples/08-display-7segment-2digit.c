// 2 位共阴数码管 8021AS
//       A
//      ---
//   F │   │ B
//      -G-
//   E │   │ C
//      ---     . DP
//       D
//
//      接地 G F A B
//      -----------------
//      10 9 8 7 6
//       1 2 3 4 5
//      -----------------
//       D DP E C 接地
//
// http://www.xlitx.com/datasheet/8021AS.pdf
// Type-I
//
// 如何判断自己的数码管引脚？
// 大部分都差不多，先找准型号，然后看 datasheet 里的接线图。如果有多个种类，第一步
// 先确定接地/电源线即可，一般接对了话会亮；如果不亮，说明接地/电源线不通。
//
// 算了，还需要 1kΩ 排阻，此例作废。

#include <stdio.h>
#include <Arduino.h>
#include <SevSeg.h>

#define LOG_BAUD 115200

SevSeg display;

void setup() {
    Serial.begin(LOG_BAUD);
    puts("setup");
    byte numPins[2] = {2, 3};
    byte segPins[8] = {10, 3, 8, 9, 19, 18, 0, 12};
    display.begin(COMMON_CATHODE, 2, numPins, segPins, 0, 0, 0, 0);
}

void loop() {
    display.setNumber(12, 1);
    delay(1000);
    display.setNumber(34, 0);
    delay(1000);
}
