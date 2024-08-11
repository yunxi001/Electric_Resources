/* SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_system.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_tinyuf2.h"

static char *TAG = "NVS: ui-events";

// 当点击设置按钮时调用此函数
void EventBtnSetupClick(lv_event_t *e)
{
    // 记录一条消息到控制台
    ESP_LOGI(TAG, "btn click!");
    // 配置 USB PHY，切换回 USB-Serial-Jtag

    // 卸载 tinyuf2 库
    esp_tinyuf2_uninstall();
    // 重新启动设备
    esp_restart();
}
