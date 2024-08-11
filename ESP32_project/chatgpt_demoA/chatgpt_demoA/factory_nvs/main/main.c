// /* SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
//  *
//  * SPDX-License-Identifier: Apache-2.0
//  */

// 包含标准库
#include <stdio.h>
#include <string.h>
// 包含FreeRTOS库
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
// 包含日志库
#include "esp_log.h"
// 包含NVS库
#include "nvs.h"
#include "nvs_flash.h"
// 包含TinyUF2库
#include "esp_tinyuf2.h"
// 包含UI库
#include "ui.h"
// 包含BSP库
#include "bsp/esp-bsp.h"
// 包含OTA库
#include "esp_ota_ops.h"

// 定义NVS修改标志位
#define NVS_MODIFIED_BIT          BIT0
// SSID长度
#define SSID_SIZE 32
// 密码长度
#define PASSWORD_SIZE 64
// 密钥长度
#define KEY_SIZE 64
// URL长度
#define URL_SIZE 64

// 定义TAG
static const char *TAG = "ChatGPT_NVS";

// 定义事件组handle
static EventGroupHandle_t s_event_group;
// 定义NVS handle
static nvs_handle_t my_handle;
// 定义缓冲区长度
static size_t buf_len_long;
static void uf2_nvs_modified_cb()
{
    ESP_LOGI(TAG, "uf2 nvs modified");
    xEventGroupSetBits(s_event_group, NVS_MODIFIED_BIT);
}

/**
 * 主应用程序入口函数。
 * 该函数执行以下操作：
 * 1. 查找并设置应用程序的启动分区为工厂分区。
 * 2. 初始化 NVS 存储，读取或初始化 WiFi 参数、ChatGPT Key 和 Base URL。
 * 3. 安装 UF2 NVS 配置。
 * 4. 初始化 I2C 和显示设备。
 * 5. 持续检测 NVS 是否有修改，若有则读取新的配置值。
 */
void app_main(void)
{
    // 查找并设置启动分区为工厂分区
    const esp_partition_t *update_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    ESP_LOGI(TAG, "Switch to partition factory");
    esp_ota_set_boot_partition(update_partition);

    // UF2 NVS 的分区名称和命名空间
    const char *uf2_nvs_partition = "nvs";
    const char *uf2_nvs_namespace = "configuration";

    // 初始化用于 WiFi 参数、ChatGPT Key 和 Base URL 的缓冲区
//    char ssid[SSID_SIZE] = {0};
//    char password[PASSWORD_SIZE] = {0};
 //   char key[KEY_SIZE] = {0};
 //   char url[URL_SIZE] = {0};
    char ssid[SSID_SIZE] = {0};      //* 目标接入点的SSID。
    char password[PASSWORD_SIZE] = {0}; //* 目标接入点的密码。
    char key_chat_api[KEY_SIZE] = {0};  //* AI密钥。
    char key_chat_secret[KEY_SIZE] = {0}; //* AI密钥。
    char url_chat[URL_SIZE] = {0};        //* AI基础URL。
    char key_voice_api[KEY_SIZE] = {0};   //* 语音识别密钥。
    char key_voice_secret[KEY_SIZE] = {0}; //* 语音识别密钥。
    char url_voice_tts[URL_SIZE] = {0};    //* tts基础URL。
    char url_voice_stt[URL_SIZE] = {0};    //* stt基础URL。
    // 创建事件组
    s_event_group = xEventGroupCreate();

    // 初始化 NVS 存储
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // 打开 NVS 命名空间，读取或初始化 WiFi 参数、ChatGPT Key 和 Base URL
    err = nvs_open_from_partition(uf2_nvs_partition, uf2_nvs_namespace, NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
        // 读取和处理 ssid
        size_t buf_len_long = sizeof(ssid);
        // err = nvs_get_str(my_handle, "ssid", ssid, &buf_len_long);
        // if (err != ESP_OK || buf_len_long == 0) {
        //     ESP_ERROR_CHECK(nvs_set_str(my_handle, "ssid", CONFIG_ESP_WIFI_SSID));
        //     ESP_ERROR_CHECK(nvs_commit(my_handle));
        //     ESP_LOGI(TAG, "no ssid, give a init value to nvs");
        // }

        // // 读取和处理 password
        // buf_len_long = sizeof(password);
        // err = nvs_get_str(my_handle, "password", password, &buf_len_long);
        // if (err != ESP_OK || buf_len_long == 0) {
        //     ESP_ERROR_CHECK(nvs_set_str(my_handle, "password", CONFIG_ESP_WIFI_PASSWORD));
        //     ESP_ERROR_CHECK(nvs_commit(my_handle));
        //     ESP_LOGI(TAG, "no password, give a init value to nvs");
        // }

        // // 读取和处理 key_chat_api key
        // buf_len_long = sizeof(key_chat_api);
        // err = nvs_get_str(my_handle, "key_chat_api", key_chat_api, &buf_len_long);
        // if (err != ESP_OK || buf_len_long == 0)
        // {
        //     ESP_ERROR_CHECK(nvs_set_str(my_handle, "key_chat_api", CONFIG_OPENAI_API_KEY));
        //     ESP_ERROR_CHECK(nvs_commit(my_handle));
        //     ESP_LOGI(TAG, "no key_chat_api, give a init value to key_chat_api api");
        // }
        // // 读取和处理 key_chat_secret key
        // buf_len_long = sizeof(key_chat_secret);
        // err = nvs_get_str(my_handle, "key_chat_secret", key_chat_secret, &buf_len_long);
        // if (err != ESP_OK || buf_len_long == 0)
        // {
        //     ESP_ERROR_CHECK(nvs_set_str(my_handle, "key_chat_secret", CONFIG_OPENAI_API_KEY));
        //     ESP_ERROR_CHECK(nvs_commit(my_handle));
        //     ESP_LOGI(TAG, "no key_chat_secret, give a init value to key_chat_secret api");
        // }
        // // 读取和处理 key_voice_api key
        // buf_len_long = sizeof(key_voice_api);
        // err = nvs_get_str(my_handle, "voice_api", key_voice_api, &buf_len_long);
        // if (err != ESP_OK || buf_len_long == 0)
        // {
        //     ESP_ERROR_CHECK(nvs_set_str(my_handle, "voice_api", CONFIG_OPENAI_API_KEY));
        //     ESP_ERROR_CHECK(nvs_commit(my_handle));
        //     ESP_LOGI(TAG, "no key_voice_api, give a init value to key_voice_api api");
        // }
        // // 读取和处理 key_voice_secret key
        // buf_len_long = sizeof(key_voice_secret);
        // err = nvs_get_str(my_handle, "voice_secret", key_voice_secret, &buf_len_long);
        // if (err != ESP_OK || buf_len_long == 0)
        // {
        //     ESP_ERROR_CHECK(nvs_set_str(my_handle, "voice_secret", CONFIG_OPENAI_API_KEY));
        //     ESP_ERROR_CHECK(nvs_commit(my_handle));
        //     ESP_LOGI(TAG, "no key_voice_secret, give a init value to key_voice_secret api");
        // }

        // // 读取和处理 url_chat URL
        // buf_len_long = sizeof(url_chat);
        // err = nvs_get_str(my_handle, "url_chat", url_chat, &buf_len_long);
        // if (err != ESP_OK || buf_len_long == 0) {
        //     ESP_ERROR_CHECK(nvs_set_str(my_handle, "url_chat", CONFIG_OPENAI_URL));
        //     ESP_ERROR_CHECK(nvs_commit(my_handle));
        //     ESP_LOGI(TAG, "no base url_chat, give a init value to key");
        // }

        // // 读取和处理 url_voice_tts URL
        // buf_len_long = sizeof(url_voice_tts);
        // err = nvs_get_str(my_handle, "url_voice_tts", url_voice_tts, &buf_len_long);
        // if (err != ESP_OK || buf_len_long == 0)
        // {
        //     ESP_ERROR_CHECK(nvs_set_str(my_handle, "url_voice_tts", CONFIG_OPENAI_URL));
        //     ESP_ERROR_CHECK(nvs_commit(my_handle));
        //     ESP_LOGI(TAG, "no base url_voice_tts, give a init value to key");
        // }

        // // 读取和处理 url_voice_stt URL
        // buf_len_long = sizeof(url_voice_stt);
        // err = nvs_get_str(my_handle, "url_voice_stt", url_voice_stt, &buf_len_long);
        // if (err != ESP_OK || buf_len_long == 0)
        // {
        //     ESP_ERROR_CHECK(nvs_set_str(my_handle, "url_voice_stt", CONFIG_OPENAI_URL));
        //     ESP_ERROR_CHECK(nvs_commit(my_handle));
        //     ESP_LOGI(TAG, "no base url_voice_stt, give a init value to key");
        // }
        nvs_close(my_handle);
    } else {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }

    // 安装 UF2 NVS 配置
    tinyuf2_nvs_config_t nvs_config = DEFAULT_TINYUF2_NVS_CONFIG();
    nvs_config.part_name = uf2_nvs_partition;
    nvs_config.namespace_name = uf2_nvs_namespace;
    nvs_config.modified_cb = uf2_nvs_modified_cb;
    ESP_ERROR_CHECK(esp_tinyuf2_install(NULL, &nvs_config));

    // 初始化 I2C 和显示设备
    bsp_i2c_init();
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_H_RES * CONFIG_BSP_LCD_DRAW_BUF_HEIGHT,
        .double_buffer = 0,
        .flags = {
            .buff_dma = true,
        }
    };
    bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();
    ui_init();

    // 无限循环以检测 NVS 是否被修改
    while (1) {
        EventBits_t bits = xEventGroupWaitBits(s_event_group, NVS_MODIFIED_BIT,
                                               pdTRUE, pdFALSE, portMAX_DELAY);
        if (bits & NVS_MODIFIED_BIT) {
            err = nvs_open_from_partition(uf2_nvs_partition, uf2_nvs_namespace, NVS_READONLY, &my_handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to open NVS partition: %s", esp_err_to_name(err));
                return;
            }

            // // 读取并日志记录修改后的值
            // size_t buf_len_long = sizeof(ssid);
            // err = nvs_get_str(my_handle, "ssid", ssid, &buf_len_long);
            // if (err != ESP_OK) {
            //     ESP_LOGE(TAG, "Failed to read 'ssid' from NVS: %s", esp_err_to_name(err));
            //     nvs_close(my_handle);
            //     return;
            // }
            // ESP_LOGD(TAG, "SSID", ssid);

            // buf_len_long = sizeof(password);
            // err = nvs_get_str(my_handle, "password", password, &buf_len_long);
            // if (err != ESP_OK) {
            //     ESP_LOGE(TAG, "Failed to read 'password' from NVS: %s", esp_err_to_name(err));
            //     nvs_close(my_handle);
            //     return;
            // }
            // ESP_LOGD(TAG, "Password", password);

            // buf_len_long = sizeof(key_chat_api);
            // err = nvs_get_str(my_handle, "chat_api", key_chat_api, &buf_len_long);
            // if (err != ESP_OK) {
            //     ESP_LOGE(TAG, "Failed to read 'chat_api' from NVS: %s", esp_err_to_name(err));
            //     nvs_close(my_handle);
            //     return;
            // }
            // ESP_LOGD(TAG, "key_chat_api Key", key_chat_api);

            // buf_len_long = sizeof(key_chat_secret);
            // err = nvs_get_str(my_handle, "chat_secret", key_chat_secret, &buf_len_long);
            // if (err != ESP_OK)
            // {
            //     ESP_LOGE(TAG, "Failed to read 'chat_secret' from NVS: %s", esp_err_to_name(err));
            //     nvs_close(my_handle);
            //     return;
            // }
            // ESP_LOGD(TAG, "key_chat_secret Key", key_chat_secret);

            // buf_len_long = sizeof(key_voice_api);
            // err = nvs_get_str(my_handle, "voice_api", key_voice_api, &buf_len_long);
            // if (err != ESP_OK)
            // {
            //     ESP_LOGE(TAG, "Failed to read 'voice_api' from NVS: %s", esp_err_to_name(err));
            //     nvs_close(my_handle);
            //     return;
            // }
            // ESP_LOGD(TAG, "voice_api Key", key_voice_api);

            // buf_len_long = sizeof(key_voice_secret);
            // err = nvs_get_str(my_handle, "voice_secret", key_voice_secret, &buf_len_long);
            // if (err != ESP_OK)
            // {
            //     ESP_LOGE(TAG, "Failed to read 'voice_secret' from NVS: %s", esp_err_to_name(err));
            //     nvs_close(my_handle);
            //     return;
            // }
            // ESP_LOGD(TAG, "voice_secret Key", key_voice_secret);

            // buf_len_long = sizeof(url_chat);
            // err = nvs_get_str(my_handle, "url_chat", url_chat, &buf_len_long);
            // if (err != ESP_OK) {
            //     ESP_LOGE(TAG, "Failed to read 'url_chat' from NVS: %s", esp_err_to_name(err));
            //     nvs_close(my_handle);
            //     return;
            // }
            // ESP_LOGD(TAG, "url_chat url", url_chat);

            // buf_len_long = sizeof(url_voice_stt);
            // err = nvs_get_str(my_handle, "url_voice_stt", url_voice_stt, &buf_len_long);
            // if (err != ESP_OK)
            // {
            //     ESP_LOGE(TAG, "Failed to read 'url_voice_stt' from NVS: %s", esp_err_to_name(err));
            //     nvs_close(my_handle);
            //     return;
            // }
            // ESP_LOGD(TAG, "url_voice_stt url", url_voice_stt);

            // buf_len_long = sizeof(url_voice_tts);
            // err = nvs_get_str(my_handle, "url_voice_tts", url_voice_tts, &buf_len_long);
            // if (err != ESP_OK)
            // {
            //     ESP_LOGE(TAG, "Failed to read 'url_voice_tts' from NVS: %s", esp_err_to_name(err));
            //     nvs_close(my_handle);
            //     return;
            // }
            // ESP_LOGD(TAG, "url_voice_tts url", url_voice_tts);
            nvs_close(my_handle);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
/*
旧配置文件
dependencies:
  espressif/button:
    component_hash: e909337d12ca9e1148007c9e03bb0fdc43d1a7df8b2cf44a76458cc044001a96
    source:
      type: service
    version: 3.2.2
  espressif/cmake_utilities:
    component_hash: 351350613ceafba240b761b4ea991e0f231ac7a9f59a9ee901f751bddc0bb18f
    source:
      type: service
    version: 0.5.3
  espressif/esp-box:
    component_hash: 414c56448aa7ddc96f0ece236d5b1ad735e8e5ad20b4197df4b93afefc953f19
    source:
      type: service
    version: 3.1.0
  espressif/esp-box-3:
    component_hash: 8d877ee1abfd9d2d528219f3b48e682248b9fb3afce23a56cdbcdbf7c606e2a4
    source:
      type: service
    version: 1.2.0
  espressif/esp-box-lite:
    component_hash: e0cd22cb25ef48c41ead6c76d88d0cc0f6f2b1f5fc2106a3b820ec2df240b0bd
    source:
      type: service
    version: 2.1.0
  espressif/esp_codec_dev:
    component_hash: 014948481bda426cd46714f297fe1891711246c62bea288863a8cc8cf13ef1f0
    source:
      type: service
    version: 1.2.0
  espressif/esp_lcd_ili9341:
    component_hash: 31f1b793aa2110dd2ae071c21ccbff0a4eb20d9a4ee40b6294c0dc0ad9552c4e
    source:
      type: service
    version: 1.2.0
  espressif/esp_lcd_touch:
    component_hash: 779b4ba2464a3ae85681e4b860caa5fdc35801458c23f3039ee761bae7f442a4
    source:
      type: service
    version: 1.1.2
  espressif/esp_lcd_touch_gt911:
    component_hash: 664c544533b91497f29b41e045bb4374a0d4d002f9d8c67c7c1bf43fb4179e3f
    source:
      type: service
    version: 1.1.1
  espressif/esp_lcd_touch_tt21100:
    component_hash: 3670f6f3d68c120d680d70ffdf2dc10f6c2a5bea8c0e87521fb92ac2af1f94c5
    source:
      type: service
    version: 1.1.0
  espressif/esp_lvgl_port:
    component_hash: 531c0525b5a5d6c05c45a147baa0626209aea6b6c0e01a7af7e6bf151878d056
    source:
      type: service
    version: 2.2.2
  espressif/esp_tinyuf2:
    component_hash: 38a82159bebebcd541e16601117b3b293d75fcac9ff7b932ab2b2a44fb477255
    source:
      type: service
    version: 0.2.2
  espressif/icm42670:
    component_hash: 3327870e7a125e7278f318297726138278f0cda2898f2461c01af0423d4afcda
    source:
      type: service
    version: 1.0.0
  idf:
    component_hash: null
    source:
      type: idf
    version: 5.1.4
  leeebo/esp-inih:
    component_hash: a2ecd9643d4cf7b3592645579d6ecf0f5c4f07f8ca8f9bafeb1d0f89fd005f38
    source:
      type: service
    version: 0.0.2
  leeebo/tinyusb_src:
    component_hash: 8e9960651a97058f5decf7a8a0544fbd40d6a4d8a581a52112cf7c178a79fc2b
    source:
      type: service
    version: 0.16.0~5
  lvgl/lvgl:
    component_hash: 2a8f619b5c2b1c7ad116c33d43dae8989c76739068f406a7e8ba9221e1085f17
    source:
      type: service
    version: 9.1.0
manifest_hash: b3bd484a004b3f2ad6e7af41ada8c1f5757dc10112b3f0d48445f0cee6f9a61b
target: esp32s3
version: 1.0.0

*/