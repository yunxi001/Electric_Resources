#include "stt_api.h"

#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "settings.h"
/*
 * 该文件定义了用于语音转文本的API，包括token和响应数据的处理
 * 文件路径：c:\Users\27392\Desktop\MY_Assistant-main\MY_Assistant-main\main\WebAPI\stt_api.c
 */

static char *TAG = "stt_api";  // 定义静态字符指针TAG，用于标识日志的标签

char token[] = "25.1fe95e7d1233fb3b2dffcd9ae0fb2803.315360000.2031832484.282335-73227049"; // 存储用于API身份验证的token

static char response_data[2048];  // 定义静态字符数组，用于存储响应数据
static int recived_len;  // 定义静态整型变量，表示接收到的数据长度
/**
 * @brief HTTP客户端的事件处理回调函数
 * 
 * 此函数用于处理HTTP客户端在连接、数据传输、请求完成或断开连接时触发的事件。
 * 根据事件类型，执行相应的日志记录和数据处理操作。
 * 
 * @param evt 指向esp_http_client_event_t结构体的指针，包含了事件的详细信息。
 * @return esp_err_t 返回ESP_OK表示成功处理了事件，无错误发生。
 */
static esp_err_t http_client_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_CONNECTED:
        // 连接到web服务器时的日志记录
        ESP_LOGI(TAG, "connected to web-server");
        recived_len = 0; // 重置接收长度计数器
        break;
    case HTTP_EVENT_ON_DATA:
        // 当接收到数据时处理
        if (evt->user_data)
        {
            // 将接收到的数据复制到用户数据缓冲区，并更新接收长度
            memcpy(evt->user_data + recived_len, evt->data, evt->data_len);
            recived_len += evt->data_len;
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        // 请求和响应完成时的日志记录
        ESP_LOGI(TAG, "finished a request and response!");
        recived_len = 0; // 重置接收长度计数器
        break;
    case HTTP_EVENT_DISCONNECTED:
        // 与web服务器断开连接时的日志记录
        ESP_LOGI(TAG, "disconnected to web-server");
        recived_len = 0; // 重置接收长度计数器
        break;
    case HTTP_EVENT_ERROR:
        // 发生错误时的日志记录
        ESP_LOGE(TAG, "error");
        recived_len = 0; // 重置接收长度计数器
        break;
    default:
        // 处理未知事件
        break;
    }

    return ESP_OK; // 表示事件已成功处理
}

/**
 * @brief 获取百度语音识别的访问令牌
 * 
 * 该函数通过发送HTTP POST请求到百度语音识别服务的授权端点，获取访问令牌。
 * 请求使用客户端凭据流（client_credentials flow）进行认证。
 * 
 * @return char* 返回从服务器获取的访问令牌字符串。如果获取失败，返回NULL。
 */
char *getAccessToken()
{
    char *access_token = NULL;

    // 初始化HTTP客户端配置，设置请求的URL和证书绑定
    esp_http_client_config_t config = {
        .url = "https://aip.baidubce.com/oauth/2.0/token",
        .event_handler = NULL,
        .crt_bundle_attach = esp_crt_bundle_attach};
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // 构建请求参数，包含grant_type、client_id和client_secret
    sys_param_t *sys_param = settings_get_parameter();
    char request_params[200];
    snprintf(request_params, sizeof(request_params),
             "grant_type=client_credentials&client_id=%s&client_secret=%s",
             sys_param->key_voice_api, sys_param->key_voice_secret);

    // 设置请求方法为POST，设置请求头的Content-Type，设置POST请求体
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
    esp_http_client_set_post_field(client, request_params, strlen(request_params));

    // 执行HTTP请求
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        // 获取响应内容长度，动态分配内存以存储响应内容
        int content_length = esp_http_client_get_content_length(client);
        char *response_buf = malloc(content_length + 1);
        int read_len = esp_http_client_read(client, response_buf, content_length);
        response_buf[read_len] = '\0';

        // 解析JSON响应，提取访问令牌
        cJSON *json = cJSON_Parse(response_buf);
        if (json != NULL)
        {
            cJSON *access_token_json = cJSON_GetObjectItem(json, "access_token");
            if (access_token_json != NULL)
            {
                access_token = strdup(access_token_json->valuestring);
            }
            cJSON_Delete(json);
        }

        free(response_buf); // 释放请求响应内容的内存
    }

    esp_http_client_cleanup(client); // 清理HTTP客户端资源
    return access_token;
}

/**
 * 使用百度语音识别API对音频数据进行识别。
 * 
 * @param audio_data 指向包含音频数据的缓冲区的指针。
 * @param audio_len 音频数据的长度。
 * @return 识别结果的字符串。成功时返回字符串，失败时返回NULL。
 */
char *baidu_asr(uint8_t *audio_data, int audio_len)
{
    char *asr_data = NULL; // 用于存储语音识别结果的字符串

    char url[256]; // 定义一个缓冲区来存储URL

    // 定义识别参数
    char dev_pid[] = "1537";   // 使用普通话识别
    char cuid[] = "yunxi001";  // 设置用户ID

    // 动态构建URL
    sprintf(url, "http://vop.baidu.com/server_api?dev_pid=%s&cuid=%s&token=%s", dev_pid, cuid, token);

    // 配置HTTP客户端
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_client_event_handler,
        .user_data = response_data};
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // 设置HTTP请求方法为POST，并设置请求体为音频数据
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_post_field(client, (const char *)audio_data, audio_len);
    // 设置请求头中的Content-Type
    esp_http_client_set_header(client, "Content-Type", "audio/wav;rate=16000");
    
    // 执行HTTP POST请求
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        // 解析HTTP响应中的JSON数据
        cJSON *json = cJSON_Parse(response_data);

        if (json != NULL)
        {
            // 获取"result"字段
            cJSON *result_json = cJSON_GetObjectItem(json, "result");
            if (result_json != NULL && cJSON_IsArray(result_json))
            {
                // 获取结果数组的第一个元素
                cJSON *result_array = cJSON_GetArrayItem(result_json, 0);
                if (result_array != NULL && cJSON_IsString(result_array))
                {
                    // 从JSON中提取识别结果字符串
                    asr_data = strdup(result_array->valuestring);
                }
            }
            // 释放JSON解析器分配的内存
            cJSON_Delete(json);
        }

        // 打印识别结果
        ESP_LOGE(TAG, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~result_data: %s\n", asr_data);
    }
    else
    {
        // 打印HTTP请求失败的信息
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }
    // 清理HTTP客户端资源
    esp_http_client_cleanup(client);

    return asr_data;
}
