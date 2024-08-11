/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/param.h>
#include "esp_tls.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include "http_get_weather.h"

static const char *TAG = "http_get_weather";
/* 在堆上为天气数据结构分配内存 */
weather_data_t *http_weather_data_create() 
{
    // 为天气数据结构分配内存
    weather_data_t *weather_data = (weather_data_t *)heap_caps_malloc(sizeof(weather_data_t), MALLOC_CAP_SPIRAM);

    if (weather_data != NULL) {
        // 为星期字符串分配内存
        weather_data -> week        = (char *)heap_caps_malloc(WEATHER_STRING_SIZE * sizeof(char), MALLOC_CAP_SPIRAM);
        // 为天气描述字符串分配内存
        weather_data -> text        = (char *)heap_caps_malloc(WEATHER_STRING_SIZE * sizeof(char), MALLOC_CAP_SPIRAM);

        // 检查内存分配是否成功
        if (weather_data->week == NULL || weather_data->text == NULL ) {
            ESP_LOGE(TAG, "Allocate weather data failed");
            // 释放已分配的内存
            free(weather_data->week);
            free(weather_data->text);
            free(weather_data);
            return NULL;
        }
    } else {
        ESP_LOGE(TAG, "Allocate weather data failed");
        return NULL;
    }

    return weather_data;
}


/* Deallocates memory used by a weather data structure */
esp_err_t http_weather_data_delete(weather_data_t *weather_data)
{
    if (weather_data != NULL) {
        if (weather_data->week != NULL) {
            free(weather_data->week);
        }

        if (weather_data->text != NULL) {
            free(weather_data->text);
        }

        free(weather_data);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Weather data is NULL");
        return ESP_FAIL;
    }

    return ESP_OK;
}

/* Event handler for HTTP client events */
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // 用于存储从事件处理程序中获取的HTTP请求响应的缓冲区
    static int output_len;       // 存储已读取的字节数
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");  // 记录HTTP事件错误
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");  // 记录HTTP连接成功事件
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");  // 记录HTTP头部已发送事件
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);  // 记录HTTP头部接收事件
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);  // 记录HTTP数据接收事件
            /*
             * 检查分块编码，因为本示例中使用的URL返回二进制数据。
             * 然而，事件处理程序也可以用于处理分块编码的情况。
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // 如果配置了user_data缓冲区，将响应数据复制到缓冲区中
                int copy_len = 0;
                if (evt->user_data) {
                    copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } else {
                    const int buffer_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(buffer_len);
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");  // 记录输出缓冲区内存分配失败
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (buffer_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
                output_len += copy_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");  // 记录HTTP请求完成事件
            if (output_buffer != NULL) {
                // 响应数据累积在output_buffer中。取消注释以下行以打印累积的响应数据
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");  // 记录HTTP断开连接事件
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);  // 记录最后的ESP错误代码
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);  // 记录最后的mbedTLS失败代码
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");  // 记录HTTP重定向事件
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;
    }
    return ESP_OK;
}


static void http_phrase_json(const char *local_response_buffer, weather_data_t *weather_data)
{
    // 检查输入指针是否为空
    if (local_response_buffer == NULL || weather_data == NULL) {
        ESP_LOGE(TAG, "NULL pointer input"); // 记录错误日志
        return;
    }

    /* 解析JSON响应 */
    cJSON *cjson_root = cJSON_Parse(local_response_buffer); // 解析JSON字符串

    // 获取JSON对象中的各个字段
    cJSON *result           = cJSON_GetObjectItem(cjson_root,"result"); // 获取"result"对象
    cJSON *now              = cJSON_GetObjectItem(result,"now"); // 获取"now"对象

    cJSON *forecasts        = cJSON_GetObjectItem(result,"forecasts"); // 获取"forecasts"数组
    cJSON *today_forecast   = cJSON_GetArrayItem(forecasts, 0); // 获取"forecasts"数组中的第一个元素

    /* 从JSON响应中提取天气数据 */
    strncpy(weather_data->text, cJSON_GetObjectItem(now, "text")->valuestring, WEATHER_STRING_SIZE - 1); // 复制天气描述
    strncpy(weather_data->week, cJSON_GetObjectItem(today_forecast, "week")->valuestring, WEATHER_STRING_SIZE - 1); // 复制星期信息

    cJSON_Delete(cjson_root); // 释放JSON对象占用的内存

    /* 记录获取的天气数据 */
    ESP_LOGI(TAG, "============================");
    ESP_LOGI(TAG, "天气: %s", weather_data->text); // 记录天气描述
    ESP_LOGI(TAG, "星期: %s", weather_data->week); // 记录星期信息
    ESP_LOGI(TAG, "============================");
}


/* 执行HTTP GET请求以获取天气数据 */
esp_err_t http_get_with_url(const char* http_url, weather_data_t *weather_data)
{
    // 检查输入参数是否为空
    if (http_url == NULL || weather_data == NULL) {
        ESP_LOGE(TAG, "输入参数为空");
        return ESP_FAIL;
    }

    // 创建本地响应缓冲区以存储HTTP响应
    char *local_response_buffer = (char *)calloc(MAX_HTTP_OUTPUT_BUFFER, sizeof(char));

    // 检查本地响应缓冲区是否分配成功
    if (local_response_buffer == NULL) {
        ESP_LOGE(TAG, "本地响应缓冲区分配失败");
        return ESP_FAIL;
    }

    // 配置HTTP客户端，设置URL和事件处理程序
    esp_http_client_config_t config = {
        .url                    = http_url,
        .event_handler          = _http_event_handler,
        .user_data              = local_response_buffer,        // 传递本地缓冲区的地址以获取响应
        .disable_auto_redirect  = true,
    };

    // 初始化HTTP客户端
    esp_http_client_handle_t http_client = esp_http_client_init(&config);
    if (http_client == NULL) {
        ESP_LOGE(TAG, "HTTP客户端初始化失败");
        goto err;
    }

    // 执行HTTP GET请求
    esp_err_t err = esp_http_client_perform(http_client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET状态码 = %d, 内容长度 = %"PRIu64,
                esp_http_client_get_status_code(http_client),
                esp_http_client_get_content_length(http_client));
    } else {
        ESP_LOGE(TAG, "HTTP GET请求失败: %s", esp_err_to_name(err));
        esp_http_client_cleanup(http_client);
        goto err;
    }

    // 解析从API返回的JSON数据
    http_phrase_json(local_response_buffer, weather_data);

    // 释放资源
    esp_http_client_cleanup(http_client);
    free(local_response_buffer);

    return ESP_OK;

err:
    free(local_response_buffer);
    return ESP_FAIL;
}
