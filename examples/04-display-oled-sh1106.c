// 1.3 寸 OLED 显示屏 SH1106
//
// 关键参数：3.3~5v，I²C 通讯，屏幕像素 128*64
// ---------------------------
// GND - 接地
// VCC - 电源 3.3v (推荐)
// SCL - 时钟线
// SDA - 数据线
// ---------------------------


#include <stdio.h>
#include <time.h>
#include <string.h>
#include <Arduino.h>
#include <Wire.h>
// https://github.com/olikraus/u8g2/wiki/u8g2setupc#sh1106-128x64_noname-1
// https://github.com/olikraus/u8g2/wiki/u8g2reference#reference
// https://github.com/larryli/u8g2_wqy
#include <U8g2lib.h>

#define LOG_BAUD 115200
#define GPIO_LED 13
#define I2C_SDA 4
#define I2C_SCL 5
#define OLED_ADDRESS 0x3c

static u8g2_t display;
uint8_t onI2CByteProc(u8x8_t *u8x8, uint8_t msg, uint8_t arg, void *ptrArg);
uint8_t onGPIODelayProc(u8x8_t *u8x8, uint8_t msg, uint8_t arg, void *ptrArg);
void checkAlive();
static time_t ts;
void buildDate(time_t *ts);

void setup() {
    buildDate(&ts);
    Serial.begin(LOG_BAUD);
    Wire.setPins(I2C_SDA, I2C_SCL);
    pinMode(GPIO_LED, OUTPUT);
    digitalWrite(GPIO_LED, LOW);
    puts("setup");
    u8g2_Setup_sh1106_i2c_128x64_noname_f(&display, U8G2_R0, onI2CByteProc, onGPIODelayProc);
    u8g2_InitDisplay(&display);
    u8g2_SetPowerSave(&display, 0);
    u8g2_ClearDisplay(&display);
}

void loop() {
    ts += 3;
    static char currentDate[17];
    struct tm *date = gmtime(&ts);
    sprintf(
        currentDate, "%d/%02d/%02d %02d:%02d",
        date->tm_year+1900, date->tm_mon+1, date->tm_mday,
        date->tm_hour, date->tm_min
    );
    printf("%s:%02d\n", currentDate, date->tm_sec);
    u8g2_ClearBuffer(&display);
    u8g2_SetFont(&display, u8g2_font_wqy16_t_chinese3);
    u8g2_SetDrawColor(&display, 1);
    u8g2_DrawUTF8(&display, 0, 14, "最多八八八八个字");
    u8g2_SetFontMode(&display, 1);
    u8g2_DrawFrame(&display, 0, 0, 128, 64);
    u8g2_DrawBox(&display, 20, 16, 16, 32);
    u8g2_DrawBox(&display, 38, 16, 16, 16);
    u8g2_SetDrawColor(&display, 0);
    u8g2_DrawStr(&display, 4, 30, "abcd"); // color=0
    u8g2_SetDrawColor(&display, 1);
    u8g2_DrawStr(&display, 4, 46, "abcd"); // color=1
    u8g2_SetDrawColor(&display, 2);
    u8g2_SetFontDirection(&display, 1);
    u8g2_DrawUTF8(&display, 38, 16, "中文"); // color=2
    u8g2_SetFontDirection(&display, 0);
    u8g2_SetDrawColor(&display, 1);
    u8g2_DrawRFrame(&display, 56, 17, 24, 16, 4);
    u8g2_DrawCircle(&display, 67, 40, 7, U8G2_DRAW_ALL);
    u8g2_DrawButtonUTF8(&display, 86, 30, U8G2_BTN_BW1, 16, 4, 0, "btn");
    u8g2_DrawButtonUTF8(&display, 86, 46, U8G2_BTN_INV, 16, 4, 0, "btn");
    u8g2_SetFont(&display, u8g2_font_open_iconic_weather_2x_t);
    u8g2_DrawGlyph(&display, 112, 33, 0x0045);
    u8g2_DrawGlyph(&display, 112, 50, 0x0043);
    // u8g2_DrawUTF8(&display, 16, 32, "第二行");
    // u8g2_DrawUTF8(&display, 32, 48, "第三行");
    u8g2_SetFont(&display, u8g2_font_wqy16_t_chinese3);
    u8g2_DrawStr(&display, 0, 62, currentDate);
    u8g2_SendBuffer(&display);
    // checkAlive();
    delay(3000);
}

void checkAlive() {
    Wire.beginTransmission(OLED_ADDRESS);
    int ret = Wire.endTransmission();
    if (ret == 0) {
        puts("SH1106 oled alive.");
    }
}

uint8_t onI2CByteProc(u8x8_t *u8x8, uint8_t msg, uint8_t arg, void *ptrArg) {
    switch (msg)
    {
    case U8X8_MSG_BYTE_INIT:
        Wire.begin();
        break;
    case U8X8_MSG_BYTE_SET_DC:
        break;
    case U8X8_MSG_BYTE_SEND:
        Wire.write((uint8_t *)ptrArg, (int)arg);
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        Wire.beginTransmission(OLED_ADDRESS);
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        Wire.endTransmission();
        break;
    default:
        return 0;
    }
    return 1;
}
uint8_t onGPIODelayProc(u8x8_t *u8x8, uint8_t msg, uint8_t arg, void *ptrArg) {
    switch (msg)
    {
    case U8X8_MSG_GPIO_I2C_DATA:
    case U8X8_MSG_GPIO_I2C_CLOCK:
        break;
    case U8X8_MSG_GPIO_MENU_SELECT:
    case U8X8_MSG_GPIO_MENU_PREV:
    case U8X8_MSG_GPIO_MENU_NEXT:
    case U8X8_MSG_GPIO_MENU_HOME:
        u8x8_SetGPIOResult(u8x8, 0);
        break;
    default:
        u8x8_SetGPIOResult(u8x8, 1);
        break;
    }
    return 1;
}

void buildDate(time_t *ts) {
    char monthNames[12][4] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    struct tm t = {0};
    int year = 0, mon = 0;
    char month[4];
    sscanf(__DATE__, "%s %d %d", month, &t.tm_mday, &year);
    sscanf(__TIME__, "%2d%*c%2d%*c%2d", &t.tm_hour, &t.tm_min, &t.tm_sec);
    while (strcmp(monthNames[mon], month) != 0 && mon < 12) {
        mon ++;
    }
    t.tm_mon = mon;
    t.tm_year = year - 1900;
    *ts =  mktime(&t);
}
