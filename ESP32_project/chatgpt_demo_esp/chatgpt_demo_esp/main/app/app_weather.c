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

#include "app_weather.h"

static const char *TAG = "http_get_weather";

/* 在堆上为天气数据结构分配内存 */
weather_data_t *http_weather_data_create() 
{
    // 为天气数据结构分配内存
    weather_data_t *weather_data = (weather_data_t *)heap_caps_malloc(sizeof(weather_data_t), MALLOC_CAP_SPIRAM);

    if (weather_data != NULL) {
        /*
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
        */
    } else {
        ESP_LOGE(TAG, "Allocate weather data failed");
        return NULL;
    }

    return weather_data;
}

/* 释放天气数据结构所使用的内存 */
esp_err_t http_weather_data_delete(weather_data_t *weather_data)
{
    if (weather_data != NULL) {
        //释放内存
        free(weather_data->city);
                free(weather_data->wind_dir);
                free(weather_data->wind_class);
//                free(weather_data->title);
                free(weather_data->text);
                free(weather_data->temp);
                free(weather_data->high);
                free(weather_data->low);
                free(weather_data->rh);
                free(weather_data);
/*
// 释放 week 字段所占用的内存
        if (weather_data->week != NULL) {
            free(weather_data->week);
        }

        // 释放 text 字段所占用的内存
        if (weather_data->text != NULL) {
            free(weather_data->text);
        }

*/
        // 释放 weather_data 结构体本身所占用的内存
        return ESP_OK;
    } else {
        // 记录错误日志，weather_data 为 NULL
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

        default:
            break;
            /*
            case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");  // 记录HTTP重定向事件
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;
            */
        
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
    if(cjson_root != NULL) {
        
        cJSON *status = cJSON_GetObjectItem(cjson_root, "status"); // 获取"status"对象
        if(status != NULL && status->valueint == 0)
        {
            cJSON *result = cJSON_GetObjectItem(cjson_root, "result"); // 获取"result"对象
            cJSON *location = cJSON_GetObjectItem(result, "location"); // 获取"location"对象
            cJSON *now = cJSON_GetObjectItem(result, "now"); // 获取"now"对象
            
            /* 从JSON响应中提取天气数据 */
            //为天气数据动态分配内存
            weather_data->city = (char *)heap_caps_malloc(strlen(cJSON_GetObjectItem(location, "name")->valuestring) + 1, MALLOC_CAP_SPIRAM);
            if(weather_data->city == NULL)
            {
                ESP_LOGE(TAG, "Allocate city failed");
            }
            weather_data->wind_dir = (char *)heap_caps_malloc(strlen(cJSON_GetObjectItem(now, "wind_dir")->valuestring) + 1, MALLOC_CAP_SPIRAM);
            if(weather_data->wind_dir == NULL)
            {
                ESP_LOGE(TAG, "Allocate wind_dir failed");
            }
            weather_data->wind_class = (char *)heap_caps_malloc(strlen(cJSON_GetObjectItem(now, "wind_class")->valuestring) + 1, MALLOC_CAP_SPIRAM);
            if(weather_data->wind_class == NULL)
            {
                ESP_LOGE(TAG, "Allocate wind_class failed");
            }
            weather_data->text = (char *)heap_caps_malloc(strlen(cJSON_GetObjectItem(now, "text")->valuestring) + 1, MALLOC_CAP_SPIRAM);
            if(weather_data->text == NULL)
            {
                ESP_LOGE(TAG, "Allocate text failed");
            }

            // 为整数类型的数据分配内存
            weather_data->temp = (int *)heap_caps_malloc(sizeof(int), MALLOC_CAP_SPIRAM);
            if(weather_data->temp == NULL)
            {
                ESP_LOGE(TAG, "Allocate temp failed");
            }
            weather_data->high = (int *)heap_caps_malloc(sizeof(int), MALLOC_CAP_SPIRAM);
            if(weather_data->high == NULL)
            {
                ESP_LOGE(TAG, "Allocate high failed");
            }
            weather_data->low = (int *)heap_caps_malloc(sizeof(int), MALLOC_CAP_SPIRAM);
            if(weather_data->low == NULL)
            {
                ESP_LOGE(TAG, "Allocate low failed");
            }
            weather_data->rh = (int *)heap_caps_malloc(sizeof(int), MALLOC_CAP_SPIRAM);
            if(weather_data->rh == NULL)
            {
                ESP_LOGE(TAG, "Allocate rh failed");
            }

            // 复制字符串和整数数据
            strcpy(weather_data->city, cJSON_GetObjectItem(location, "name")->valuestring);
            strcpy(weather_data->wind_dir, cJSON_GetObjectItem(now, "wind_dir")->valuestring);
            strcpy(weather_data->wind_class, cJSON_GetObjectItem(now, "wind_class")->valuestring);
            strcpy(weather_data->text, cJSON_GetObjectItem(now, "text")->valuestring);
            
            *(weather_data->temp) = cJSON_GetObjectItem(now, "temp")->valueint;
            *(weather_data->high) = cJSON_GetObjectItem(cJSON_GetObjectItem(result, "forecasts")->child, "high")->valueint; // 获取第一个预报的高温值
            *(weather_data->low) = cJSON_GetObjectItem(cJSON_GetObjectItem(result, "forecasts")->child, "low")->valueint;  // 获取第一个预报的低温值
            *(weather_data->rh) = cJSON_GetObjectItem(now, "rh")->valueint;

            ESP_LOGI(TAG, "weather json parse success");
        }
        else    
        {
            ESP_LOGI(TAG, "数据获取错误");
        }

        cJSON_Delete(cjson_root); // 释放JSON对象占用的内存
    }

    /* 记录获取的天气数据 */
    ESP_LOGI(TAG, "============================");
    ESP_LOGI(TAG, "天气: %s", weather_data->text); // 记录天气描述
    ESP_LOGI(TAG, "风向: %s", weather_data->wind_dir); // 记录风向
    ESP_LOGI(TAG, "风力: %s", weather_data->wind_class); // 记录风力
    ESP_LOGI(TAG, "城市: %s", weather_data->city); // 记录城市名称
    ESP_LOGI(TAG, "温度: %d", *(weather_data->temp)); // 记录温度
    ESP_LOGI(TAG, "最高温度: %d", *(weather_data->high)); // 记录最高温度
    ESP_LOGI(TAG, "最低温度: %d", *(weather_data->low)); // 记录最低温度
    ESP_LOGI(TAG, "湿度: %d", *(weather_data->rh)); // 记录湿度
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
    if (err != ESP_OK) {
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
