# Arduino - 合宙 esp32c3 开发系列

## 引

## 设备示例

3. 温湿度传感器 AHT-10，I2C 通讯
4. OLED 液晶屏 128*64，I2C 通讯，SH1106 驱动
5. 温度传感器 DS18B20，单总线通讯
6. ~~声音传感器 KY-038，AO+DO 输出~~
7. 彩色 LCD 液晶屏 128*160，SPI 通讯，ST7735S 驱动
8. ~~2 位共阴数码管，10 PIN~~

## n 多的坑

### 不要买带排针的设备

重要的事情说 3 遍，**不要买带排针的**。合宙 esp32c3 就是这种，带个排针看似很“美好”，其实是个天坑。为什么？带排针，说明引脚没焊接，没焊接大概率引脚会接触不良，就算插上面包板也一样。不要问我怎么知道的，，，排查了一整天头都快炸了。

记得买**已焊接**的设备。除非你会手动焊。

### vscode arduino 下载包太慢，没走代理，设置了环境变量也没用

找到扩展里自带 arduino-cli 的位置，初始化设置并添加代理。

```shell
$ arduino-cli.exe config init
$ arduino-cli.exe config set network.proxy <proxy_url>
```

vscode 这个 arduino 扩展比较简陋，没有对 arduino-cli 配置文件的支持，不过可以手动使用 arduino-cli 做一些操作，和扩展自己执行是等效的。扩展只是加了些命令行参数而已。

### KY-038 没用，检测不了分贝

### 数码管需要接的引脚很多，合宙板子没多少 IO 接口，不推荐购买。不如买 I2C 通讯的显示屏，可以串更多的设备

### LCD 屏真的辣眼睛，尤其在小屏幕上，有钱建议购买 OLED

### 注意事项

多看几遍合宙板子文档里的注意事项。

## 调试

### 前提条件

- 经典款。带串口芯片 CH343，安装驱动后可烧录
  
  不支持内置 JTAG 调试（坑）。这样情况下需要外接 GPIO 18/19 引脚到 USB 以供调试。

- 简约款/新款。无串口芯片

  支持内置 JTAG，但是需要进入下载模式烧录。

## 参考文档

- [Arduino 库参考](https://www.arduino.cc/reference/en/)
- [乐鑫文档 - esp32](https://docs.espressif.com/projects/arduino-esp32/en/latest/index.html)
- [合宙文档 - esp32c3 开发板](https://wiki.luatos.com/chips/esp32c3/board.html)
- [合宙文档2](https://doc.openluat.com/)
