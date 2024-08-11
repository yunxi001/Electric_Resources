/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_check.h"
#include "bsp/esp-bsp.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "settings.h"
#include "esp_ota_ops.h"
// 设置文件路径
// 文件名: d:\Electric_Resources\ESP32_project\chatgpt_demoA\main\settings\settings.c

// 定义日志标签
static const char *TAG = "settings";
// 定义nvs分区名称和命名空间
const char *uf2_nvs_partition = "nvs";
const char *uf2_nvs_namespace = "configuration";
// 定义nvs句柄和系统参数结构体
static nvs_handle_t my_handle;
static sys_param_t g_sys_param = {0};

// 恢复出厂设置函数
esp_err_t settings_factory_reset(void)
{
    // 查找OTA分区并设置为引导分区
    const esp_partition_t *update_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    ESP_LOGI(TAG, "Switch to partition UF2");
    esp_ota_set_boot_partition(update_partition);
    esp_restart();
    return ESP_OK;
}

// 从NVS中读取参数函数
esp_err_t settings_read_parameter_from_nvs(void)
{
    // 从指定的nvs分区和命名空间中打开nvs句柄
    esp_err_t ret = nvs_open_from_partition(uf2_nvs_partition, uf2_nvs_namespace, NVS_READONLY, &my_handle);
    // 处理nvs未找到的情况
    if (ESP_ERR_NVS_NOT_FOUND == ret) {
        ESP_LOGI(TAG, "Credentials not found");
        goto err;
    }

    // 检查nvs打开是否成功
    ESP_GOTO_ON_FALSE(ESP_OK == ret, ret, err, TAG, "nvs open failed (0x%x)", ret);
    size_t len = 0;

    // 读取SSID
    len = sizeof(g_sys_param.ssid);
    ret = nvs_get_str(my_handle, "ssid", g_sys_param.ssid, &len);
    // 处理未找到或长度为0的情况
    if (ret != ESP_OK || len == 0) {
        ESP_LOGI(TAG, "No SSID found");
        goto err;
    }

    // 读取密码
    len = sizeof(g_sys_param.password);
    ret = nvs_get_str(my_handle, "password", g_sys_param.password, &len);
    // 处理未找到或长度为0的情况
    if (ret != ESP_OK || len == 0) {
        ESP_LOGI(TAG, "No Password found");
        goto err;
    }

    // 读取密钥
    len = sizeof(g_sys_param.key_chat_api);
    ret = nvs_get_str(my_handle, "key_chat_api", g_sys_param.key_chat_api, &len);
    // 处理未找到或长度为0的情况
    if (ret != ESP_OK || len == 0) {
        ESP_LOGI(TAG, "没有找到key_chat_api");
        goto err;
    }
    // 读取密钥
    len = sizeof(g_sys_param.key_chat_secret);
    ret = nvs_get_str(my_handle, "key_chat_secret", g_sys_param.key_chat_secret, &len);
    // 处理未找到或长度为0的情况
    if (ret != ESP_OK || len == 0)
    {
        ESP_LOGI(TAG, "key_chat_secret");
        goto err;
    }
    // 读取密钥
    len = sizeof(g_sys_param.key_voice_api);
    ret = nvs_get_str(my_handle, "key_voice_api", g_sys_param.key_voice_api, &len);
    // 处理未找到或长度为0的情况
    if (ret != ESP_OK || len == 0)
    {
        ESP_LOGI(TAG, "key_voice_api");
        goto err;
    }
    // 读取密钥
    len = sizeof(g_sys_param.key_voice_secret);
    ret = nvs_get_str(my_handle, "key_voice_secret", g_sys_param.key_voice_secret, &len);
    // 处理未找到或长度为0的情况
    if (ret != ESP_OK || len == 0)
    {
        ESP_LOGI(TAG, "key_voice_secret");
        goto err;
    }

    // 读取URL
    len = sizeof(g_sys_param.url_chat);
    ret = nvs_get_str(my_handle, "url_chat", g_sys_param.url_chat, &len);
    // 处理未找到或长度为0的情况
    if (ret != ESP_OK || len == 0) {
        ESP_LOGI(TAG, "没有找到url_chat");
        goto err;
    }
    // 读取URL
    len = sizeof(g_sys_param.url_voice_tts);
    ret = nvs_get_str(my_handle, "url_voice_tts", g_sys_param.url_voice_tts, &len);
    // 处理未找到或长度为0的情况
    if (ret != ESP_OK || len == 0)
    {
        ESP_LOGI(TAG, "url_voice_tts");
        goto err;
    }
    // 读取URL
    len = sizeof(g_sys_param.url_voice_stt);
    ret = nvs_get_str(my_handle, "url_voice_stt", g_sys_param.url_voice_stt, &len);
    // 处理未找到或长度为0的情况
    if (ret != ESP_OK || len == 0)
    {
        ESP_LOGI(TAG, "url_voice_stt");
        goto err;
    }

    /*
    // 读取密钥
    len = sizeof(g_sys_param.key);
        ret = nvs_get_str(my_handle, "ChatGPT_key", g_sys_param.key, &len);
        // 处理未找到或长度为0的情况
        if (ret != ESP_OK || len == 0) {
            ESP_LOGI(TAG, "No OpenAI key found");
            goto err;
        }

        // 读取URL
        len = sizeof(g_sys_param.url);
        ret = nvs_get_str(my_handle, "Base_url", g_sys_param.url, &len);
        // 处理未找到或长度为0的情况
        if (ret != ESP_OK || len == 0) {
            ESP_LOGI(TAG, "No OpenAI Base url found");
            goto err;
        }
    */

    // 关闭nvs句柄
    nvs_close(my_handle);

    // 记录读取到的参数
    ESP_LOGI(TAG, "stored ssid:%s", g_sys_param.ssid);
    ESP_LOGI(TAG, "stored password:%s", g_sys_param.password);
    ESP_LOGI(TAG, "stored 大模型api:%s", g_sys_param.key_chat_api);
    ESP_LOGI(TAG, "stored 大模型secret:%s", g_sys_param.key_chat_secret);
    ESP_LOGI(TAG, "stored 语音api:%s", g_sys_param.key_voice_api);
    ESP_LOGI(TAG, "stored 语音secret:%s", g_sys_param.key_voice_secret);
    ESP_LOGI(TAG, "stored 大模型URL:%s", g_sys_param.url_chat);
    ESP_LOGI(TAG, "stored 语音转文字URL:%s", g_sys_param.url_voice_tts);
    ESP_LOGI(TAG, "stored 文字转语音URL:%s", g_sys_param.url_voice_stt);
    return ESP_OK;

err:
    // 如果存在nvs句柄，则关闭该句柄
    if (my_handle) {
        nvs_close(my_handle);
    }
    // 执行恢复出厂设置函数
    settings_factory_reset();
    return ret;
}
/*
sys_param_t *settings_get_parameter(void)
{
    return &g_sys_param;
}
*/

/**
 * 获取系统参数
 *
 * 该函数用于获取预设的系统参数，包括SSID、密码、API密钥和API地址。
 * 这些参数被硬编码在函数内，适用于简单应用场景，例如初始化设置。
 *
 * @return sys_param_t* - 返回系统参数结构体的指针。
 */
sys_param_t *settings_get_parameter(void)
{
    ESP_LOGI(TAG, "Get system parameter");
    // 初始化系统参数结构体中的字段
    strcpy(g_sys_param.ssid, "DESKTOP-V2NVN7J 8971");                               // 设定SSID为"ESP32"
    strcpy(g_sys_param.password, "12345678");                                       // 设定密码为"20030930"
    strcpy(g_sys_param.key_chat_api, "sk-UMnNg19PCayc8JRdqp4O48OeYeFE9HuEjRZDETC1ldvfhhO1");       // 设定大模型API密钥
    strcpy(g_sys_param.key_chat_secret, "k1LxbRCC75tzHEq0zndf1Q40YPZ5h6q2");                       // 设定大模型secret密钥
    strcpy(g_sys_param.key_voice_api, "rv7NQ9NxAyJy6EDJNAgLkad2");                                 // 设定语音API密钥
    strcpy(g_sys_param.key_voice_secret, "lXwo203sHf18z8I7Du2vGK34TJmELTNh");                      // 设定语音secret密钥

    strcpy(g_sys_param.url_chat, "https://api.chatanywhere.tech");                  // 设定大模型API地址
    strcpy(g_sys_param.url_voice_tts, "http://vop.baidu.com/server_api");             // 设定语音转文字URL
    strcpy(g_sys_param.url_voice_stt, "https://aip.baidubce.com/rpc/2.0/aasr/v1/create"); // 设定文字转语音URL

    // 记录读取到的参数
    return &g_sys_param;                                                            // 返回系统参数结构体的指针
}