| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

# ESP32-S3 智能家居面板

本项目采用 [ESP32-S3-LCD-EV-Board](https://docs.espressif.com/projects/espressif-esp-dev-kits/zh_CN/latest/esp32s3/esp32-s3-lcd-ev-board/index.html) 作为核心控制面板，通过 LVGL 构建了智能家居的操作界面。集成 Home Assistant，实现了对多品牌智能家居产品的统一控制。此外，利用 ESP-SR 实现语音控制功能，并通过 [esp-audio-player](https://components.espressif.com/components/chmorgan/esp-audio-player/versions/1.0.7) 组件支持 MP3 音乐播放。

演示视频和实现过程：[用 ESP32 接入 Home Assistant【语音智能家居面板】](https://www.bilibili.com/video/BV1yZ421M7NR/)

# 示例使用说明

## **物料清单（BOM）**

1. [ESP32-S3-LCD-EV-Board](https://docs.espressif.com/projects/espressif-esp-dev-kits/zh_CN/latest/esp32s3/esp32-s3-lcd-ev-board/index.html#esp32-s3-lcd-ev-board)
2. XH2.54 接口的扬声器 

## **3D 打印智能家居面板外壳**
- 解压 `3D_Print` 文件夹中的压缩包，打印药盒外壳的 3D 模型

## **获取百度地图 API 密钥**

   - 前往 [百度开发者平台](http://lbsyun.baidu.com/) 点击 **创建应用** 获取开发者密钥

## 部署 Home Assistant OS

- 参考 [Home Assistant 官方教程](https://www.home-assistant.io/installation/) 进行部署，推荐 [在树莓派上进行部署](https://www.home-assistant.io/installation/raspberrypi)
- 在树莓派上部署可参考视频：[用 ESP32 接入 Home Assistant【语音智能家居面板】](https://www.bilibili.com/video/BV1yZ421M7NR/)

## **开发环境**
- 当前工程使用 [ESP-IDF release/v5.1](https://github.com/espressif/esp-idf/tree/release/v5.1) 版本

## **编译和烧录**
- 参考 [ESP-IDF VSCode 开发 【沉浸式教程】](https://www.bilibili.com/video/BV1X34y1M7L8/) 将代码烧录到开发板

# **配置项目**

## 必要配置

- 在 **Kconfig.projbuild** 中配置 `HTTP_URL_ADDRESS`（百度地图 API 服务地址）

    ```c
    HTTP_URL_ADDRESS = "https://api.map.baidu.com/weather/v1/?district_id=区县的行政区划编码&data_type=all&ak=百度开发者密钥";
    ```

- 在 **Kconfig.projbuild** 中配置 `BROKER_USERNAME` 和 `BROKER_PASSWORD`（Home Assistant 登录账号和密码）

- 在 **Kconfig.projbuild** 中配置 `EXAMPLE_WIFI_SSID` 和 `EXAMPLE_WIFI_PASSWORD`（Wi-Fi SSID 和密码）

## 非必要配置

- 如果无法使用以太网线将树莓派与电脑连接到同一局域网下，可以在 Home Assistant 启动前配置 Wi-Fi（不推荐，建议使用以太网线）。在 SD 卡创建文件夹和文件： `CONFG` -> `network` -> `my-network`。在 `my-network` 中配置 `SSID` 和 `密码`：


    ```shell
    [connection] 
    id=my-network 
    uuid=72111c67-4a5d-4d5c-925e-f8ee26efb3c3 
    type=802-11-wireless 
    
    [802-11-wireless] 
    mode=infrastructure 
    ssid="SSID" 
    # Uncomment below if your SSID is not broadcasted 
    #hidden=true 
    
    [802-11-wireless-security] 
    auth-alg=open 
    key-mgmt=wpa-psk 
    psk=密码
    
    [ipv4] 
    method=auto 
    
    [ipv6] 
    addr-gen-mode=stable-privacy 
    method=auto 
    ```

- 注意：如果 Home Assistant 已经完成了初始化配置则以上⽅式⽆法⽣效，确保文件内容是 `UNIX` 格式

# 技术支持和反馈

- 对于技术问题，请访问 [esp32.com](https://esp32.com/) 论坛
- 对于功能请求或错误报告，请创建 [GitHub issue](https://github.com/espressif/esp-idf/issues) 问题
