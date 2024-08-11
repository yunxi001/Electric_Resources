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

static const char *TAG = "settings";

//const char *uf2_nvs_partition = "nvs";
//const char *uf2_nvs_namespace = "configuration";
//static nvs_handle_t my_handle;
static sys_param_t g_sys_param = {0};


// 从NVS存储中读取参数
/*

esp_err_t settings_read_parameter_from_nvs(void)
{
    // 从分区中打开NVS存储，以只读模式访问指定的命名空间
    esp_err_t ret = nvs_open_from_partition(uf2_nvs_partition, uf2_nvs_namespace, NVS_READONLY, &my_handle);
    if (ESP_ERR_NVS_NOT_FOUND == ret) {
        // 如果凭证未找到，记录日志并跳转到错误处理标签
        ESP_LOGI(TAG, "Credentials not found");
        goto err;
    }

    ESP_GOTO_ON_FALSE(ESP_OK == ret, ret, err, TAG, "nvs open failed (0x%x)", ret);
    size_t len = 0;

    // Read SSID
    len = sizeof(g_sys_param.ssid);
    ret = nvs_get_str(my_handle, "ssid", g_sys_param.ssid, &len);
    if (ret != ESP_OK || len == 0) {
        ESP_LOGI(TAG, "No SSID found");
        goto err;
    }

    // Read password
    len = sizeof(g_sys_param.password);
    ret = nvs_get_str(my_handle, "password", g_sys_param.password, &len);
    if (ret != ESP_OK || len == 0) {
        ESP_LOGI(TAG, "No Password found");
        goto err;
    }

    // Read key
    len = sizeof(g_sys_param.key);
    ret = nvs_get_str(my_handle, "ChatGPT_key", g_sys_param.key, &len);
    if (ret != ESP_OK || len == 0) {
        ESP_LOGI(TAG, "No OpenAI key found");
        goto err;
    }

    // Read url
    len = sizeof(g_sys_param.url);
    ret = nvs_get_str(my_handle, "Base_url", g_sys_param.url, &len);
    if (ret != ESP_OK || len == 0) {
        ESP_LOGI(TAG, "No OpenAI Base url found");
        goto err;
    }

    nvs_close(my_handle);

    ESP_LOGI(TAG, "stored ssid:%s", g_sys_param.ssid);
    ESP_LOGI(TAG, "stored password:%s", g_sys_param.password);
    ESP_LOGI(TAG, "stored OpenAI:%s", g_sys_param.key);
    ESP_LOGI(TAG, "stored Base URL:%s", g_sys_param.url);
    return ESP_OK;

err:
    if (my_handle) {
        nvs_close(my_handle);
    }
    settings_factory_reset();
    return ret;
}
*/

sys_param_t *settings_get_parameter(void)
{
        ESP_LOGI(TAG, "Get system parameter");
    // 初始化系统参数结构体中的字段
    /*
    strcpy(g_sys_param.ssid, "DESKTOP-V2NVN7J 8971");                               // 设定SSID为"ESP32"
    strcpy(g_sys_param.password, "12345678");                                       // 设定密码为"20030930"
    strcpy(g_sys_param.key_chat_api, "sk-UMnNg19PCayc8JRdqp4O48OeYeFE9HuEjRZDETC1ldvfhhO1");       // 设定大模型API密钥
    strcpy(g_sys_param.key_chat_secret, "k1LxbRCC75tzHEq0zndf1Q40YPZ5h6q2");                       // 设定大模型secret密钥
    strcpy(g_sys_param.key_voice_api, "rv7NQ9NxAyJy6EDJNAgLkad2");                                 // 设定语音API密钥
    strcpy(g_sys_param.key_voice_secret, "lXwo203sHf18z8I7Du2vGK34TJmELTNh");                      // 设定语音secret密钥

    strcpy(g_sys_param.url_chat, "https://api.chatanywhere.tech");                  // 设定大模型API地址
    strcpy(g_sys_param.url_voice_tts, "http://vop.baidu.com/server_api");             // 设定语音转文字URL
    strcpy(g_sys_param.url_voice_stt, "https://aip.baidubce.com/rpc/2.0/aasr/v1/create"); // 设定文字转语音URL
    
    strcpy(g_sys_param.url, "https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/ernie-speed-128k");

    */
   //注意数组大小是否够用！！！
   
    strcpy(g_sys_param.ssid, "DESKTOP-V2NVN7J 8971");   
    strcpy(g_sys_param.password, "12345678");
    strcpy(g_sys_param.key_stt_api, "rv7NQ9NxAyJy6EDJNAgLkad2");
    strcpy(g_sys_param.key_stt_secret, "lXwo203sHf18z8I7Du2vGK34TJmELTNh");
    strcpy(g_sys_param.key_chat_api, "B8X2lLXVU8MUN9ikzSJV7nI6");
    strcpy(g_sys_param.key_chat_secret, "k1LxbRCC75tzHEq0zndf1Q40YPZ5h6q2");
    strcpy(g_sys_param.weather_url, "http://api.map.baidu.com/weather/v1/?district_id=410502&data_type=all&ak=CXv4Wkc8kyDl9ebmFOxRwgFemueym0Av"); 

    return &g_sys_param;
}
