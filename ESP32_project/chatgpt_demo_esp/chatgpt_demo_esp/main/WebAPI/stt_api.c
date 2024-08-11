#include "stt_api.h"

#include <sys/param.h>
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "settings.h"

/*！！！！！！！！！！！使用百度语音识别极速版api，httpa,POST,raw方式！！！！！！！！！！！！！！*/
//百度服务端会将非pcm格式，转为pcm格式，因此使用wav、amr、m4a会有额外的转换耗时。

static const char *TAG = "stt_api";  // 定义静态字符指针TAG，用于标识日志的标签
char access_token_stt[256] = {0}; // 存储用于API身份验证的token,百度tts和stt共用！！！！！！！！！！！！！！
//static char response_data[4096];  // 定义静态字符数组，用于存储响应数据
//static int recived_len = 0;  // 定义静态整型变量，表示接收到的数据长度
#define MAX_RESPONSE_BUF 1024 * 5 // 定义最大HTTP响应缓冲区大小

/**
 * @brief HTTP客户端的事件处理回调函数，处理stt数据
 * 
 * 此函数用于处理HTTP客户端在连接、数据传输、请求完成或断开连接时触发的事件。
 * 根据事件类型，执行相应的日志记录和数据处理操作。
 * 
 * @param evt 指向esp_http_client_event_t结构体的指针，包含了事件的详细信息。
 * @return esp_err_t 返回ESP_OK表示成功处理了事件，无错误发生。
 */
static esp_err_t http_client_event_handler_stt(esp_http_client_event_t *evt)
{
//    static char *output_buffer;//只能接收一次数据，建议自己定义了数据接收区，删掉这个，后面的output_buffer都要删掉
    // 存储读取的字节数
    static int output_len;
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            // 处理HTTP请求错误事件
            ESP_LOGW(TAG, "HTTP_EVENT_ERROR");
            break;

/*case HTTP_EVENT_ON_CONNECTED:
            // 处理HTTP连接成功事件
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            // 处理HTTP头部已发送事件
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            // 处理HTTP头部接收事件
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;  
*/                
        case HTTP_EVENT_ON_DATA:
            // 处理HTTP数据接收事件
//           ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
//           ESP_LOGI(TAG , "HTTP_EVENT_ON_DATA, data=%s", (char *)evt->data);
            // 在新请求的情况下清理缓冲区
            if (output_len == 0 && evt->user_data) {
                memset(evt->user_data, 0, MAX_RESPONSE_BUF);
            }
//            if (!esp_http_client_is_chunked_response(evt->client)) / 检查分块编码
                int copy_len = 0;
                if (evt->user_data) {
                    copy_len = MIN(evt->data_len, (MAX_RESPONSE_BUF - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } 
/*               else {//else这部分定义了数据接收区，可不要
                    int content_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        output_buffer = (char *) calloc(content_len + 1, sizeof(char));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (content_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
*/ 
                output_len += copy_len;        
            break;
        case HTTP_EVENT_ON_FINISH:
            // 处理HTTP请求完成事件
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
                //// 解析HTTP响应中的JSON数据
                cJSON *json = cJSON_Parse(evt->user_data);
                if (json != NULL)
                {
                    //清空user_data，接下来把解析完的数据放到user_data中
                    memset(evt->user_data, 0, MAX_RESPONSE_BUF);
                    cJSON *error_json = cJSON_GetObjectItem(json, "error_msg");
                    if (error_json != NULL && cJSON_IsString(error_json))
                    {
                        ESP_LOGE(TAG, "Error message: %s", error_json->valuestring);
                    }
                    // 获取"result"字段
                    cJSON *result_json = cJSON_GetObjectItem(json, "result");
                    if (result_json != NULL && cJSON_IsArray(result_json))
                    {
                        cJSON *result_array = NULL;
                        int output_str_len = 0;
                        int cpoy_str_len = 0;
                        int arryLength = cJSON_GetArraySize(result_json);          // 获取数组长度
                        //把数组中的字符串拼接起来，按照顺序放到user_data中(注意先清空user_data)
                        for (int i = 0; i < arryLength; i++)
                        {
                            result_array = cJSON_GetArrayItem(result_json, i);        // 获取数组中JSON对象
                            if (result_array != NULL && cJSON_IsString(result_array))
                            {
                                cpoy_str_len = MIN(strlen(result_array->valuestring), (MAX_RESPONSE_BUF - output_str_len));
                                if (cpoy_str_len) {
                                    memcpy(evt->user_data + output_str_len, result_array->valuestring, cpoy_str_len);
                                    output_str_len += cpoy_str_len;
                                }
                            }                                                   
                        }
                        // 打印识别结果
//                        ESP_LOGI(TAG, "~result_data: %s\n", (char *)evt->user_data);
                    }
                    else
                    {
                        ESP_LOGE(TAG, "Failed to get result from response");
                    }
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to parse response as JSON");
                }           
                cJSON_Delete(json); // 释放JSON解析器分配的内存         
/*            if (output_buffer != NULL) {
                // 释放输出缓冲区
                free(output_buffer);
                output_buffer = NULL;
            }
*/            
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            // 处理HTTP断开连接事件
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
/*if (output_buffer != NULL) {
                // 释放输出缓冲区
                free(output_buffer);
                output_buffer = NULL;
            }
*/            
            output_len = 0;
            break;
/*          case HTTP_EVENT_REDIRECT:
            // 处理HTTP重定向事件
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;        
*/
        default:
            break;
    }
    return ESP_OK;
}


/**
 * @brief HTTP客户端的事件处理回调函数，处理ACCESS_TOKEN数据
 * 
 * 此函数用于处理HTTP客户端在连接、数据传输、请求完成或断开连接时触发的事件。
 * 根据事件类型，执行相应的日志记录和数据处理操作。
 * 
 * @param evt 指向esp_http_client_event_t结构体的指针，包含了事件的详细信息。
 * @return esp_err_t 返回ESP_OK表示成功处理了事件，无错误发生。
 */
esp_err_t http_client_event_handler_stt_token(esp_http_client_event_t *evt)
{
//    static char *output_buffer;//只能接收一次数据，建议自己定义了数据接收区，删掉这个，后面的output_buffer都要删掉
    // 存储读取的字节数
    static int output_len;
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            // 处理HTTP请求错误事件
            ESP_LOGW(TAG, "HTTP_EVENT_ERROR");
            break;
/*          case HTTP_EVENT_ON_CONNECTED:
            // 处理HTTP连接成功事件
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            // 处理HTTP头部已发送事件
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            // 处理HTTP头部接收事件
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;        
*/
        case HTTP_EVENT_ON_DATA:
            // 处理HTTP数据接收事件
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            // 在新请求的情况下清理缓冲区
            if (output_len == 0 && evt->user_data) {
                memset(evt->user_data, 0, MAX_RESPONSE_BUF);
            }
//            if (!esp_http_client_is_chunked_response(evt->client)) / 检查分块编码
                int copy_len = 0;
                if (evt->user_data) {
                    copy_len = MIN(evt->data_len, (MAX_RESPONSE_BUF - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } 
/*               else {//else这部分定义了数据接收区，可不要
                    int content_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        output_buffer = (char *) calloc(content_len + 1, sizeof(char));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (content_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
*/ 
                output_len += copy_len;
                       
            break;
        case HTTP_EVENT_ON_FINISH:
            // 处理HTTP请求完成事件
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            //接受完毕解析json
                cJSON *json = cJSON_Parse(evt->user_data);
                if (json != NULL)
                {
                    cJSON *access_token_json = cJSON_GetObjectItem(json, "access_token");
                        if (access_token_json != NULL)
                        {
                            strncpy(access_token_stt, access_token_json->valuestring, 256 - 1);
                            access_token_stt[256 - 1] = '\0'; // 确保字符串以 null 结尾
                            // 打印访问令牌
//                            ESP_LOGI(TAG, "Access token: %s", access_token_stt);
                        }
                        else
                        {
                            ESP_LOGE(TAG, "Failed to get access token from response");
                        }
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to parse response as JSON");
                }            
                cJSON_Delete(json); // 释放JSON解析器分配的内存  
/*            if (output_buffer != NULL) {
                // 释放输出缓冲区
                free(output_buffer);
                output_buffer = NULL;
            }
            */
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            // 处理HTTP断开连接事件
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
/*if (output_buffer != NULL) {
                // 释放输出缓冲区
                free(output_buffer);
                output_buffer = NULL;
            }
*/            
            output_len = 0;
            break;
/*          case HTTP_EVENT_REDIRECT:
            // 处理HTTP重定向事件
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;        
*/
        default:
            break;   
    }
    return ESP_OK;
}

/**
 * @brief 获取百度语音识别的AccessToken
 * 
 * 该函数通过发送HTTP POST请求到百度语音识别服务的授权端点，获取访问令牌。
 * 
 * @return  成功返回ESP_OK，如果获取失败，返回NULL。
 */
esp_err_t  getAccessToken_stt()
{
//   char *access_token = NULL;
        char *response_buf = heap_caps_malloc(MAX_RESPONSE_BUF, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (response_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for response buffer");
            return ESP_FAIL;
        }

    // 初始化HTTP客户端配置，设置请求的URL和证书绑定
    esp_http_client_config_t config = {
        .url = "https://aip.baidubce.com/oauth/2.0/token",
        .event_handler = http_client_event_handler_stt_token,
        .user_data = response_buf,
        .crt_bundle_attach = esp_crt_bundle_attach
        };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // 构建请求参数，包含grant_type、client_id和client_secret
    sys_param_t *sys_param = settings_get_parameter();
    char request_params[256];
    snprintf(request_params, sizeof(request_params),
             "grant_type=client_credentials&client_id=%s&client_secret=%s",
             sys_param->key_stt_api, sys_param->key_stt_secret);

    // 设置请求方法为POST，设置请求头的Content-Type，设置POST请求体
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
    esp_http_client_set_header(client, "Accept", "application/json");
    esp_http_client_set_post_field(client, request_params, strlen(request_params));

    // 执行HTTP请求
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) { // 检查HTTP POST请求是否成功
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err)); // 记录HTTP POST请求失败的原因
    }
    free(response_buf); // 释放HTTP响应缓冲区
    esp_http_client_cleanup(client); // 清理HTTP客户端资源
    if (strlen(access_token_stt) == 0) {
        return ESP_FAIL;
    }
    else {
        return ESP_OK;
    };
}


/**
 * 使用百度语音识别API对音频数据进行识别。//raw 方式请求，发送音频数据格式为wav
 * 
 * @param audio_data 指向包含音频数据的缓冲区的指针。
 * @param audio_len 音频数据的长度。
 * @return 识别结果的字符串的指针。成功时返回字符串的指针，失败时返回NULL。
 * @note    第一次出错可能是access_token_stt过期，重新运行即可！！！！！！
 */
char *baidu_stt(uint8_t *audio_data, int audio_len)
{
    // 检查Access Token是否为空，如果为空则获取新的Access Token
    if (strlen(access_token_stt) == 0) {
//        ESP_LOGI(TAG, "Access token is empty, fetching new token");
        if(getAccessToken_stt() != ESP_OK) 
        {
            ESP_LOGE(TAG, "Failed to get access token");
            return NULL;
        } 
    }

   
    char *response_buf = heap_caps_malloc(MAX_RESPONSE_BUF, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (response_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for response buffer");
        return NULL;
    }
    char url[512]; // 定义一个缓冲区来存储URL
    char dev_pid[] = "80001";   // 使用普通话识别
    char cuid[] = "yunxi001";  // 设置用户ID
    
    // 动态构建URL
    sprintf(url, "https://vop.baidu.com/pro_api?dev_pid=%s&cuid=%s&token=%s", dev_pid, cuid, access_token_stt);

    // 配置HTTP客户端
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_client_event_handler_stt,
        .user_data = response_buf,
        .crt_bundle_attach = esp_crt_bundle_attach 
        };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    // 设置HTTP请求方法为POST，并设置请求体为音频数据
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    // 设置请求头中的Content-Type
    esp_http_client_set_header(client, "Content-Type", "audio/wav;rate=16000");
    // 设置POST请求体
//    esp_http_client_set_post_field(client, (const char *)audio_data, audio_len);
    esp_http_client_set_post_field(client, (char *)audio_data, audio_len);

    // 设置HTTP请求的超时时间为5000毫秒
    esp_http_client_set_timeout_ms(client, 5000);

    // 执行HTTP请求
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) { // 检查HTTP POST请求是否成功
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err)); // 记录HTTP POST请求失败的原因
        //可能是access_token_stt过期，重新运行即可
        memset(access_token_stt, 0, sizeof(access_token_stt));//清空access_token_stt
    }
    //检查response_buf内数据的长度，将其分配空间储存,返回空间的指针到asr_data
    char *asr_data = NULL; // 用于存储语音识别结果的字符串
    if (response_buf != NULL) {
        asr_data =  strdup(response_buf); 
    }
    // 清理HTTP客户端资源
    free(response_buf);
    esp_http_client_cleanup(client);
    return asr_data;   
/*

// 打开音频文件，以二进制读取模式
FILE *audio_file = fopen("/spiffs/16k.wav", "rb");
if (!audio_file) {
    // 如果文件打开失败，记录错误日志并返回NULL
    ESP_LOGE(TAG, "Failed to open audio file");
    return NULL;
}

// 将文件指针移动到文件末尾，获取文件长度
fseek(audio_file, 0, SEEK_END);
long audio_len_file = ftell(audio_file);
// 将文件指针移动回文件开头
fseek(audio_file, 0, SEEK_SET);

// 分配内存以存储音频数据
uint8_t *audio_data_buf = malloc(audio_len_file);
if (!audio_data_buf) {
    // 如果内存分配失败，记录错误日志并关闭文件，返回NULL
    ESP_LOGE(TAG, "Failed to allocate memory for audio data");
    fclose(audio_file);
    return NULL;
}

// 读取音频数据到分配的内存中
fread(audio_data_buf, 1, audio_len_file, audio_file);
// 关闭音频文件
fclose(audio_file);

// 计算Base64编码后的数据长度
size_t encoded_len = 0;
mbedtls_base64_encode(NULL, 0, &encoded_len, audio_data_buf, audio_len_file);

// 分配内存以存储Base64编码后的数据
char *encoded_data = malloc(encoded_len + 1);
if (!encoded_data) {
    // 如果内存分配失败，记录错误日志并释放原始音频数据内存，返回NULL
    ESP_LOGE(TAG, "Failed to allocate memory for encoded data");
    free(audio_data_buf);
    return NULL;
}

// 进行Base64编码
mbedtls_base64_encode((unsigned char *)encoded_data, encoded_len + 1, &encoded_len, audio_data_buf, audio_len_file);
//ESP_LOGI(TAG, "Encoded data: %s", encoded_data);
ESP_LOGI(TAG, "Encoded data length: %ld", audio_len_file);
// 释放原始音频数据内存
free(audio_data_buf);

// 现在encoded_data中包含了Base64编码后的音频数据，encoded_len是编码后的数据长度
// 你可以在这里处理encoded_data和encoded_len

    char *asr_data = NULL; // 用于存储语音识别结果的字符串
    char *response_buf = heap_caps_malloc(MAX_RESPONSE_BUF, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (response_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for response buffer");
        return NULL;
    }

    static char cuid[] = "yunxi001";  // 设置用户ID
    // 检查Access Token是否为空，如果为空则获取新的Access Token
    if (strlen(access_token_stt) == 0) {
        ESP_LOGI(TAG, "Access token is empty, fetching new token");
        getAccessToken_stt();
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "format", "wav");
    cJSON_AddNumberToObject(root, "rate", 16000);
    cJSON_AddNumberToObject(root, "dev_pid", 80001);
    cJSON_AddNumberToObject(root, "channel", 1);
    cJSON_AddStringToObject(root, "token", access_token_stt);
    cJSON_AddStringToObject(root, "cuid", cuid);
    cJSON_AddNumberToObject(root, "len", audio_len_file);
    cJSON_AddStringToObject(root, "speech", encoded_data);
    char *request_data = cJSON_Print(root);
    cJSON_Delete(root);


    // 配置HTTP客户端
    esp_http_client_config_t config = {
        .url = "https://vop.baidu.com/pro_api",
        .event_handler = http_client_event_handler_stt,
        .user_data = response_buf,
        .crt_bundle_attach = esp_crt_bundle_attach };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // 设置HTTP请求方法为POST
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    // 设置请求头中的Content-Type
    esp_http_client_set_header(client, "Content-Type", "application/json");
//    esp_http_client_set_header(client, "Accept", "application/json");
    // 设置POST请求体
//    esp_http_client_set_post_field(client, (const char *)audio_data, audio_len);
    esp_http_client_set_post_field(client, (char *)request_data, strlen(request_data));

    // 设置HTTP请求的超时时间为5000毫秒
    esp_http_client_set_timeout_ms(client, 5000);

    // 执行HTTP请求
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) { // 检查HTTP POST请求是否成功
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err)); // 记录HTTP POST请求失败的原因
        //可能是access_token_stt过期，重新运行即可
        memset(access_token_stt, 0, sizeof(access_token_stt));//清空access_token_stt
    }
    // 清理HTTP客户端资源
    free(response_buf);
    esp_http_client_cleanup(client);
    return asr_data;

*/
}
