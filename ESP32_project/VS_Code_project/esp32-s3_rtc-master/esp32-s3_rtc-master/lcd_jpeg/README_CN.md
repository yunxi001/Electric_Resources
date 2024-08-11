# JPEG 解码显示示例
- [English Version](./README.md)
- 例程难度：![alt text](../../../docs/_static/level_basic.png "初级")

## 例程简介

本例程演示调用 `esp_jpeg_dec` API 解码 JPEG 图片，并显示在 LCD 上。示例图片分辨率为 320 * 244。


## 环境配置

### 硬件要求

本例程支持的开发板在 `$ADF_PATH/examples/README_CN.md` 文档中 [例程与乐鑫音频开发板的兼容性表格](../../README_CN.md#例程与乐鑫音频开发板的兼容性) 中有标注，表格中标有绿色复选框的开发板均可运行本例程。请记住，如下面的 [配置](#配置) 一节所述，可以在 `menuconfig` 中选择开发板。

## 编译和下载


### IDF 默认分支

本例程支持 IDF release/v4.4 及以后的分支，例程默认使用 IDF release/v4.4 分支。

### IDF 分支

- 本例程使用 release/v4.4 分支，切换命令如下：

```
cd $IDF_PATH
git checkout master
git pull
git checkout release/v4.4
git submodule update --init --recursive
  ```

### 配置

本例程默认选择的开发板是 `ESP32-S3-Korvo-2 v3`。如果需要在其他的开发板上运行此例程，则需要在 menuconfig 中选择开发板的配置，例如选择 `ESP32-S2-Kaluga-1 Kit`。

```
menuconfig > Audio HAL > ESP32-S2-Kaluga-1 Kit
```

### 编译和下载

请先编译版本并烧录到开发板上，然后运行 monitor 工具来查看串口输出（替换 PORT 为端口名称）：

```
idf.py -p PORT flash monitor
```

退出调试界面使用 ``Ctrl-]``。

有关配置和使用 ESP-IDF 生成项目的完整步骤，请前往 [《ESP-IDF 编程指南》](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html)，并在页面左上角选择芯片和版本，查看对应的文档。

## 如何使用例程

### 功能和用法

例程运行时可看到红色字样的 `ESP32` 图片，其效果是波浪浮动并不停反转。


### 日志输出

以下为本例程的完整日志。

```c
I (25) boot: ESP-IDF v4.4-dev-3838-gf23dcd3555-dirty 2nd stage bootloader
I (25) boot: compile time 20:01:32
I (25) boot: chip revision: 0
I (29) boot.esp32s3: Boot SPI Speed : 80MHz
I (33) boot.esp32s3: SPI Mode       : DIO
I (38) boot.esp32s3: SPI Flash Size : 2MB
I (43) boot: Enabling RNG early entropy source...
W (48) bootloader_random: RNG for ESP32-S3 not currently supported
I (55) boot: Partition Table:
I (59) boot: ## Label            Usage          Type ST Offset   Length
I (66) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (73) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (81) boot:  2 factory          factory app      00 00 00010000 00100000
I (88) boot: End of partition table
I (93) esp_image: segment 0: paddr=00010020 vaddr=3c030020 size=1f518h (128280) map
I (124) esp_image: segment 1: paddr=0002f540 vaddr=3fc95700 size=00ad8h (  2776) load
I (125) esp_image: segment 2: paddr=00030020 vaddr=42000020 size=21bd8h (138200) map
I (155) esp_image: segment 3: paddr=00051c00 vaddr=3fc961d8 size=02070h (  8304) load
I (157) esp_image: segment 4: paddr=00053c78 vaddr=40374000 size=116fch ( 71420) load
I (176) esp_image: segment 5: paddr=0006537c vaddr=50000000 size=00010h (    16) load
I (183) boot: Loaded app from partition at offset 0x10000
I (183) boot: Disabling RNG early entropy source...
W (185) bootloader_random: RNG for ESP32-S3 not currently supported
I (203) opi psram: vendor id : 0x0d (AP)
I (203) opi psram: dev id    : 0x02 (generation 3)
I (203) opi psram: density   : 0x03 (64 Mbit)
I (207) opi psram: good-die  : 0x01 (Pass)
I (212) opi psram: Latency   : 0x01 (Fixed)
I (217) opi psram: VCC       : 0x01 (3V)
I (221) opi psram: SRF       : 0x01 (Fast Refresh)
I (227) opi psram: BurstType : 0x01 (Hybrid Wrap)
I (232) opi psram: BurstLen  : 0x01 (32 Byte)
I (237) opi psram: Readlatency  : 0x02 (10 cycles@Fixed)
I (243) opi psram: DriveStrength: 0x00 (1/1)
W (248) PSRAM: DO NOT USE FOR MASS PRODUCTION! Timing parameters will be updated in future IDF version.
I (259) spiram: Found 64MBit SPI RAM device
I (263) spiram: SPI RAM mode: sram 80m
I (267) spiram: PSRAM initialized, cache is in normal (1-core) mode.
I (275) cpu_start: Pro cpu up.
I (278) cpu_start: Starting app cpu, entry point is 0x40375490
0x40375490: call_start_cpu1 at /Users/maojianxin/workspace/esp-adf-internal-dev/esp-idf/components/esp_system/port/cpu_start.c:156

I (0) cpu_start: App cpu up.
I (698) spiram: SPI SRAM memory test OK
W (698) rtcinit: efuse read fail, set default blk2_version: 1, blk1_version:2

I (711) cpu_start: Pro cpu start user code
I (711) cpu_start: cpu freq: 240000000
I (711) cpu_start: Application information:
I (713) cpu_start: Project name:     lcd_tjpgd
I (719) cpu_start: App version:      v2.2-285-ge2b7cfc3-dirty
I (725) cpu_start: Compile time:     Dec 22 2021 22:53:58
I (731) cpu_start: ELF file SHA256:  8a905db51491e127...
I (737) cpu_start: ESP-IDF:          v4.4-dev-3838-gf23dcd3555-dirty
I (744) heap_init: Initializing. RAM available for dynamic allocation:
I (751) heap_init: At 3FC99268 len 00046D98 (283 KiB): D/IRAM
I (758) heap_init: At 3FCE0000 len 0000EE34 (59 KiB): STACK/DRAM
I (764) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (771) heap_init: At 600FE000 len 00002000 (8 KiB): RTCRAM
I (777) spiram: Adding pool of 8192K of external SPI memory to heap allocator
I (785) spi_flash: detected chip: gd
I (789) spi_flash: flash io: dio
W (793) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (806) sleep: Configure to isolate all GPIO pins in sleep state
I (813) sleep: Enable automatic switching of GPIO sleep configuration
I (820) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (840) spiram: Reserving pool of 32K of internal memory for DMA/internal allocations
I (870) gpio: GPIO[2]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (870) ImageDec: The image size is 156160 bytes, width:320, height:244
```

## 技术支持

请按照下面的链接获取技术支持：

- 技术支持参见 [esp32.com](https://esp32.com/viewforum.php?f=20) 论坛
- 故障和新功能需求，请创建 [GitHub issue](https://github.com/espressif/esp-adf/issues)

我们会尽快回复。
