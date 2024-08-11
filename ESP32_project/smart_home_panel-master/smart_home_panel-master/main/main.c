/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "bsp/esp32_s3_lcd_ev_board.h"
#include "lvgl.h"
#include "ui/ui.h"
#include "app_sntp.h"
#include "app_mqtt.h"
#include "bsp_board_extra.h"
#include "app_ui_control.h"
#include "app_play_music.h"
#include "app_sr.h"

static const char *TAG = "main";

// *INDENT-OFF*
// 初始化并显示LVGL图形界面
void app_lvgl_display(void)
{
    // 锁定显示设备
    bsp_display_lock(0);

    // 初始化UI
    ui_init();

    // 解锁显示设备
    bsp_display_unlock();
}

void app_main(void)
{
    // 初始化非易失性存储（NVS）
    ESP_ERROR_CHECK(nvs_flash_init());
    // 初始化网络接口
    ESP_ERROR_CHECK(esp_netif_init());
    // 创建默认的事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* 这个辅助函数根据menuconfig中的选择配置Wi-Fi或以太网。
     * 阅读examples/protocols/README.md中的“Establishing Wi-Fi or Ethernet Connection”部分
     * 了解更多关于这个函数的信息。
     */
    ESP_ERROR_CHECK(example_connect());

    // 初始化SNTP
    app_sntp_init();

    // 初始化音频
    bsp_extra_codec_init();
    ESP_ERROR_CHECK(app_sr_start());

    // 挂载SPIFFS文件系统
    bsp_spiffs_mount();

    // 初始化音频播放器
    bsp_extra_player_init(NULL);
    app_music_player_init();

    // 初始化显示和LVGL
    bsp_display_start();

    // 在显示器上添加并显示对象
    app_lvgl_display();

    // 启动MQTT应用
    mqtt_app_start();

    // 设置LVGL时钟
    ui_timer_init();

    // 打印启动后的空闲内存
    ESP_LOGI(TAG, "Free memory after start: %d bytes", heap_caps_get_total_size(MALLOC_CAP_INTERNAL));

    // 主循环
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
