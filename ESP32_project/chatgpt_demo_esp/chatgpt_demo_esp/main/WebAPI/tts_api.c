#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_http_client.h"
#include "mbedtls/base64.h"
#include "cJSON.h"
#include "esp_log.h"
#include "settings.h"
#include "tts_api.h"
#include "stt_api.h"
#include "esp_crt_bundle.h"
#include "app_audio.h"
#include "audio_player.h"
static const char *TAG = "TTS_APP";
FILE *fp = NULL;//
// #define MAX_RESPONSE_BUF 1024 * 5 // 定义最大HTTP响应缓冲区大小
extern char access_token_stt[256];
extern uint32_t file_total_len;
// 定义文件最大大小为1MB
#define MAX_FILE_SIZE       (1*1024*1024)
// 定义文件固定大小为256000B
#define FILE_SIZE (256000)
// static int file_flag = 0;
static int audio_play_finish_flag = 1;
static void audio_play_finish_cb(void)
{
    ESP_LOGI(TAG, "audio_play_finish_cb");
    audio_play_finish_flag = 1;
}
/**
 * @brief HTTP客户端的事件处理回调函数
 *
 * 此函数用于处理HTTP客户端在不同事件发生时的逻辑，例如在连接建立、接收数据、请求完成或断开连接时。
 * 它会根据事件类型来更新或处理相关的数据。
 *
 * @param evt 指向esp_http_client_event_t结构体的指针，包含了事件的详细信息，例如事件类型、数据等。
 * @return esp_err_t 返回ESP_OK表示成功处理了事件，其他错误码表示处理过程中出现了错误。
 */
static esp_err_t http_client_event_handler_tts(esp_http_client_event_t *evt)
{
//    static char *output_buffer;//只能接收一次数据，建议自己定义了数据接收区，删掉这个，后面的output_buffer都要删掉
    // 存储读取的字节数
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            // 处理HTTP请求错误事件
            ESP_LOGW(TAG, "HTTP_EVENT_ERROR");
            break;
          case HTTP_EVENT_ON_CONNECTED:
            // 处理HTTP连接成功事件
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            // 处理HTTP头部已发送事件
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
                file_total_len = 0;
            // 处理HTTP头部接收事件
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break; 
           
        case HTTP_EVENT_ON_DATA:
            // 处理HTTP数据接收事件
            // ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            // 在新请求的情况下清理缓冲区
            // if (output_len == 0 && evt->user_data) {
            //     memset(evt->user_data, 0, MAX_RESPONSE_BUF);
            // }
//            if (!esp_http_client_is_chunked_response(evt->client)) / 检查分块编码
//            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=(%"PRIu32" + %d) [%d]", file_total_len, evt->data_len, MAX_FILE_SIZE);
             if ((file_total_len + evt->data_len) < MAX_FILE_SIZE) {
            memcpy(audio_rx_buffer + file_total_len, (char *)evt->data, evt->data_len);
            file_total_len += evt->data_len;
              }
             break;

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
            break;
        case HTTP_EVENT_ON_FINISH:
            // 处理HTTP请求完成事件
            // 当HTTP请求完成时处理，播放接收到的音频数据
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH:%" PRIu32 ", %" PRIu32 " K", file_total_len, file_total_len / 1024);
//        uint32_t dataLength = audio_rx_buffer;
//        char *speechptr = file_total_len;
        esp_err_t status = ESP_FAIL;
                 while(audio_play_finish_flag == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(100));
                    
                }
                audio_play_finish_flag = 0;
        fp = fmemopen((void *)audio_rx_buffer, file_total_len, "rb");
        if (fp)
        {
            status = audio_player_play(fp);
        }
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

// //解析json数据，获取语音数据
// static esp_err_t  parse_json_data(char *json_data)
// {
//     cJSON *root = cJSON_Parse(json_data);
//     if (root == NULL) {
//         ESP_LOGE(TAG, "Error before: [%s]", cJSON_GetErrorPtr());
//         return ESP_FAIL;
//     }else {
//         //如果有err_no，则解析并打印错误信息
//         if( cJSON_GetObjectItem(root, "err_no") != NULL)
//         {
//             cJSON *err_no = cJSON_GetObjectItem(root, "err_no");
//             ESP_LOGE(TAG, "Error code: %d", err_no->valueint);
//             cJSON_Delete(root);
//             return ESP_FAIL;
//         }
//         else
//         {
//             cJSON *binary = cJSON_GetObjectItem(root, "binary");
//             //将二进制数据保存到文件中
//             //如果文件不为空，则保存到文件中
//             if(binary->valuestring!= NULL)
//             {
//                 // if(file_flag == 0)
//                 // {
//                 //     file_flag = 1;
//                     // while( audio_play_finish_flag == 0)
//                     // {
//                     //     vTaskDelay(pdMS_TO_TICKS(100));
//                     // }
//                     // audio_play_finish_flag = 0;
//                     FILE *fp = fopen("/spiffs/tts_respond.mp3", "wb");
//                     // while(fp != NULL)
//                     // {
//                     //     //循环等待直到文件为空
//                     //     vTaskDelay(pdMS_TO_TICKS(100));
//                     // }
//                     //先清空文件
//                     fseek(fp, 0, SEEK_SET);
//                     ftruncate(fileno(fp), 0);
//                     fwrite(binary->valuestring, 1, strlen(binary->valuestring), fp);
//                     fclose(fp);
//                 // }
//                 // else if(file_flag == 1)
//                 // {
//                 //     file_flag = 0;
//                 //     FILE *fp = fopen("/spiffs/tts_respond2.mp3", "wb");
//                 //     while(fp != NULL)
//                 //     {
//                 //         //循环等待直到文件为空
//                 //         vTaskDelay(pdMS_TO_TICKS(100));
//                 //     }
//                 //     fwrite(binary->valuestring, 1, strlen(binary->valuestring), fp);
//                 //     fclose(fp);
//                 // }
//             }
            
//         }
//         cJSON_Delete(root);
//     }
//     return ESP_OK;
// }


int URLEncode(const char* str, const int strSize, char* result, const int resultSize)
{
    int i;
    int j = 0;//for result index
    char ch;
 
    if ((str==NULL) || (result==NULL) || (strSize<=0) || (resultSize<=0)) {
        return 0;
    }
 
    for ( i=0; (i<strSize)&&(j<resultSize); ++i) {
        ch = str[i];
        if (((ch>='A') && (ch<'Z')) ||
            ((ch>='a') && (ch<'z')) ||
            ((ch>='0') && (ch<'9'))) {
            result[j++] = ch;
        } else if (ch == ' ') {
            result[j++] = '+';
        } else if (ch == '.' || ch == '-' || ch == '_' || ch == '*') {
            result[j++] = ch;
        } else {
            if (j+3 < resultSize) {
                sprintf(result+j, "%%%02X", (unsigned char)ch);
                j += 3;
            } else {
                return 0;
            }
        }
    }
    result[j] = '\0';
//    ESP_LOGI(TAG, "URLEncode: %s", result);
    return j;
}
// 字符串并将其编码为Base64格式的字符串 
char *get_file_base64_content(char *text)
{
     // 动态分配内存给 encoded_uri
    size_t encoded_uri_len = strlen(text) * 4 + 1; // 确保有足够的空间
    char *encoded_uri = (char *)heap_caps_malloc(encoded_uri_len + 1, MALLOC_CAP_SPIRAM);
    if (encoded_uri == NULL) {
        ESP_LOGI(TAG, "malloc encoded_uri failed");
        return NULL; // 内存分配失败
    }
    URLEncode(text, strlen(text), encoded_uri, encoded_uri_len);
    return encoded_uri;
}

//传入文本，返回语音数据
esp_err_t tts_task(char *pvParameters)
{
    if (strlen(access_token_stt) == 0) {
//        ESP_LOGI(TAG, "Access token is empty, fetching new token");
        if(getAccessToken_stt() != ESP_OK) 
        {
            ESP_LOGE(TAG, "Failed to get access token");
            return ESP_FAIL;
        } 
    } 
    // char *response_buf = heap_caps_malloc(MAX_RESPONSE_BUF, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    //     if (response_buf == NULL) {
    //         ESP_LOGE(TAG, "Failed to allocate memory for response buffer");
    //         return ESP_FAIL;
    //     }

    esp_http_client_config_t config = {
        .url = "https://tsn.baidu.com/text2audio",
        .event_handler = http_client_event_handler_tts,
//        .user_data = response_buf,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    char post_data[512];//返回为MP3格式
    char *tex = get_file_base64_content(pvParameters);
    if (tex == NULL) {
        // free(response_buf);
        ESP_LOGE(TAG, "Failed to get Base64 content");
        return ESP_FAIL;
    }
    snprintf(post_data, sizeof(post_data), "tex=%s&tok=%s&cuid=yunxi001&ctp=1&lan=zh&spd=5&pit=5&vol=5&per=0&aue=3",tex, access_token_stt);
    free(tex);
    esp_http_client_set_url(client, "https://tsn.baidu.com/text2audio");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");

    // 执行HTTP请求
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) { // 检查HTTP POST请求是否成功
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err)); // 记录HTTP POST请求失败的原因
        memset(access_token_stt, 0, sizeof(access_token_stt));//清空access_token_stt
    }
    // free(response_buf); // 释放HTTP响应缓冲区
    esp_http_client_cleanup(client); // 清理HTTP客户端资源
    
    return ESP_OK;
}
// 定义要检测的中文标点符号 
const char* delimiters[] = {"？", "！", "：","。","，"};

// 检查当前字符是否是指定的标点符号
int isDelimiter(const char* ch) {
    for (int i = 0; i < sizeof(delimiters) / sizeof(delimiters[0]); ++i) {
        if (strncmp(ch, delimiters[i], 3) == 0) { // 中文符号占3个字节
            return 1;
        }
    }
    return 0;
}

esp_err_t processString(char* input) {
    // 初始化一些必要的变量
    int start = 0; // 用于记录每个标点符号之前内容的起始位置
    int lastDelimiterIndex = -1; // 用于记录最后一个标点符号的位置
    int len = strlen(input); // 输入字符串的长度
    uint8_t istextFlag = 0; // 标记是否为中文字符
    // 遍历输入字符串的每一个字符
    for (int i = 0; i < len;) {
        // 检查是否为UTF-8多字节字符（中文）
        if ((unsigned char)input[i] >= 0xE4) { // 中文字符通常从0xE4开始
            if (isDelimiter(&input[i])) {
                // 输出当前标点符号之前的内容
                istextFlag = 1;
                char temp[i - start + 2];
                strncpy(temp, input + start, i - start); // 复制当前标点符号之前的内容
                temp[i - start] = '\0';

                ESP_LOGI(TAG, "Chinese symbol: %s", temp);
                    
                    if(tts_task(temp) != ESP_OK) 
                    {
                        ESP_LOGE(TAG, "Failed to get  tts data");
                        return ESP_FAIL;
                    }
                // 更新标点符号的起始位置为当前标点符号的下一个字符
                start = i + 3;
                // 更新最后一个标点符号的位置
                lastDelimiterIndex = i;
                i += 3; // 跳过当前标点符号
            } else {
                i += 3; // 跳过非标点符号的中文字符
            }
        } else {
            i += 1; // 跳过单字节字符（如英文、数字、常用符号）
        }
    }

    // 输出最后一个标点符号之后的内容（如果有的话）
    if (lastDelimiterIndex != -1 && lastDelimiterIndex + 3 < len) {
        int afterContentIndex = lastDelimiterIndex + 3;
        int isAfterContentEmpty = 1; // 标记最后一个标点符号之后的内容是否为空

        // 检查最后一个标点符号之后的内容
        for (int i = afterContentIndex; i < len; ++i) {
            if ((unsigned char)input[i] > 0x20) { // 忽略空格等控制字符
                isAfterContentEmpty = 0; // 找到非空内容
                break;
            }
        }

        if (!isAfterContentEmpty) { // 只有在最后一个符号后有内容时才输出
            char temp[len - afterContentIndex + 2];
                strncpy(temp, input + afterContentIndex, len - afterContentIndex); // 复制当前标点符号之前的内容
                temp[len - afterContentIndex] = '\0';
                ESP_LOGI(TAG, "Chinese symbol: %s", temp);
                while(audio_play_finish_flag == 0)
                {
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
                    if(tts_task(temp) != ESP_OK) 
                    {
                        ESP_LOGE(TAG, "Failed to get  tts data");
                        return ESP_FAIL;
                    }
                
        }
    }
    if(istextFlag == 0)
    {
        if(tts_task(input) != ESP_OK) 
        {
            ESP_LOGE(TAG, "Failed to get  tts data");
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

esp_err_t tts_respond(char *text)
{
    //判断输入的文本是否为空
    if (text == NULL || strlen(text) == 0) {
        ESP_LOGE(TAG, "Input text is empty");
        return ESP_FAIL;
    }
   audio_register_play_finish_cb(audio_play_finish_cb);
//    if(tts_task(text) == ESP_OK)
    if(processString(text) == ESP_OK)
    {
        return ESP_OK;
    }
    else
    {
        return ESP_FAIL;
    }
    //计算输入文本的字符数
    //每二十个字符合成一次语音，不足则最后一次发送剩下的字符，循环直到语音合成完毕
    //每次合成语音之后调用音频播放，下一次等待语音播放完毕，再播放下一段合成的语音
    //建立两个语音文件，第一次合成的语音保存在第一个文件，第二次合成的语音保存在第二个文件，
    //播放完第一个文件后，播放第二个文件，循环播放，直到播放完毕。
    //每次合成语音时，擦除未播放的语音文件，若没有未播放的语音文件，则等待语音播放完毕，将语音数据保存到文件中，
    //播放完毕后，删除两个文件。
    //所有的结束后，返回esp_ok
    // 调用语音合成接口
//    tts_task(text);
    // 解析返回的语音数据
//    parse_json_data(tts_data);
}
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>

// // 判断字符是否为汉字
// bool isChinese(char *str) {
//     unsigned char c = str[0];
//     return (c >= 0xE4 && c <= 0xE9);
// }

// // 判断字符是否为数字
// bool isDigit(char c) {
//     return (c >= '0' && c <= '9');
// }

// // 判断字符是否为特殊符号（这里只包括+和-）
// bool isSpecialSymbol(char c) {
//     return (c == '+' || c == '-');
// }

// // 统计并输出UTF-8字符串中的汉字、数字和特殊符号的数量，并以十个字符为一组输出
// void processString(char *str) {
//     int count = 0;  // 计数器
//     int chineseCount = 0, digitCount = 0, specialSymbolCount = 0;
//     char buffer[11];  // 临时存储十个字符

//     while (*str) {
//         if (isChinese(str)) {
//             chineseCount++;
//             count++;
//             strncpy(buffer + (count - 1), str, 3);
//             str += 3;  // 汉字在UTF-8中占三个字节
//         } else if (isDigit(*str)) {
//             digitCount++;
//             count++;
//             buffer[count - 1] = *str;
//             str++;
//         } else if (isSpecialSymbol(*str)) {
//             specialSymbolCount++;
//             count++;
//             buffer[count - 1] = *str;
//             str++;
//         } else {
//             str++;
//         }

//         if (count == 10) {
//             buffer[count] = '\0';
//             printf("十个字符：%s\n", buffer);
//             count = 0;
//         }
//     }

//     // 如果剩余字符不足十个
//     if (count > 0) {
//         buffer[count] = '\0';
//         printf("剩余字符：%s\n", buffer);
//     }

//     printf("汉字数量：%d\n", chineseCount);
//     printf("数字数量：%d\n", digitCount);
//     printf("特殊符号数量：%d\n", specialSymbolCount);
// }

// int main() {
//     char str[] = "你好123+-世界456！";
//     processString(str);
//     return 0;
// }










/*
#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "esp_tls.h"
#include "mbedtls/base64.h"
#include "settings.h"
#include <tts_api.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <mbedtls/base64.h>

static const char *TAG = "tts_api";

#include "esp_heap_caps.h"

// 替换 malloc 为 heap_caps_malloc
//定义字符串，保存接收到的数据
char *tts_data = NULL;
int tts_data_len = 0;

#define portMAX_DELAY_TTS 1000 // 最大延时时间ms
// 定义讯飞API的相关参数
#define APPID "c8f4f28c"
#define APIKEY "b890516b1539db5bd818605988eb6272"
#define APISECRET "NjIwNTBmYmU0YTZjMDNkMGM4MjRmZDg2"
//通过接口密钥基于hmac-sha256计算签名，向服务器端发送Websocket协议握手请求。详见下方 接口鉴权.
//握手成功后，客户端通过Websocket连接同时上传和接收数据。数据上传完毕，客户端需要上传一次数据结束标识。详见下方 接口数据传输与接收.
//接收到服务器端的结果全部返回标识后断开Websocket连接。

// WebSocket事件处理函数
static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    // 将事件数据转换为 esp_websocket_event_data_t 类型
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;

    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WebSocket connected");  // WebSocket连接成功
            break;
        case WEBSOCKET_EVENT_DATA:
            ESP_LOGI(TAG, "Received opcode=%d, message=%.*s", data->op_code, data->data_len, (char *)data->data_ptr);  // 接收到数据
            // 如果接收到的数据包含JSON结构，尝试解析
            cJSON *json = cJSON_Parse(data->data_ptr);
            if (json != NULL)
            {
                // 获取"code"字段，返回码，0表示成功，继续接收数据，其它表示异常，详情请参考错误码。
                cJSON *code_json = cJSON_GetObjectItem(json, "code");
                if (code_json != NULL && cJSON_IsNumber(code_json))
                {
                    int code = code_json->valueint;
                    if (code!= 0)
                    {
                        //打印code字段,表示错误信息.
                        ESP_LOGI(TAG, "code: %d", code);
                    }
                }
                else
                {
                    ESP_LOGI(TAG, "code field not found or not a number");

                    cJSON *data_json = cJSON_GetObjectItem(json, "data");
                    if (data_json != NULL && cJSON_IsArray(data_json))
                    {
                        // 获取"audio"字段,是音频数据的base64编码字符串，格式为aue指定的
                        cJSON *audio_json = cJSON_GetArrayItem(data_json, 0);
                        // 获取status字段，标志位，1表示未结束，2表示已结束
                        cJSON *status_json = cJSON_GetArrayItem(data_json, 2);
                        //判断status字段是否为1
                        if (status_json != NULL && cJSON_IsNumber(status_json) && status_json->valueint == 1)
                        {
                            //将每次接收到的数据按顺序保存在tts_data中
                            char *audio_data = audio_json->valuestring;
                            int audio_data_len = strlen(audio_data);
                            tts_data = realloc(tts_data, tts_data? tts_data_len + audio_data_len : audio_data_len);
                            memcpy(tts_data + tts_data_len, audio_data, audio_data_len);
                            tts_data_len += audio_data_len;
                            ESP_LOGI(TAG, "音频数据未结束，继续接收");
                        }
                        else
                        {
                            //打印status字段,表示音频数据是否已结束。
                            ESP_LOGI(TAG, "status: %d", status_json->valueint);
                        }
                        //判断status字段是否为2，表示已结束
                        if (status_json != NULL && cJSON_IsNumber(status_json) && status_json->valueint == 2)
                        {
                            //将接收到的数据保存在tts_data的最后
                            char *audio_data = audio_json->valuestring;
                            int audio_data_len = strlen(audio_data);
                            tts_data = realloc(tts_data, tts_data? tts_data_len + audio_data_len : audio_data_len);
                            memcpy(tts_data + tts_data_len, audio_data, audio_data_len);
                            tts_data_len += audio_data_len;
                            //将base64编码的音频数据解码并保存到文件中
                            char *audio_data_decode = malloc(tts_data_len);//base64数据占用内存和二进制占用内存不同
                            size_t audio_data_decode_len;
                            int ret = mbedtls_base64_decode((unsigned char *)audio_data_decode, tts_data_len, &audio_data_decode_len, (unsigned char *)tts_data, tts_data_len);
                            if (ret != 0) {
                                // 处理解码错误
                                ESP_LOGE(TAG, "Base64 解码失败");
                                free(audio_data_decode);
                                free(tts_data);
                                tts_data = NULL;
                                tts_data_len = 0;
                                return;
                            }
                            FILE *fp = fopen("/spiffs/tts.wav", "wb");
                            fwrite(audio_data_decode, 1, audio_data_decode_len, fp);
                            fclose(fp);
                            ESP_LOGI(TAG, "音频数据已结束，保存到文件");
                            // 最后记得播放音频文件
                            // 重置 tts_data
                            free(tts_data);
                            tts_data = NULL;
                            tts_data_len = 0;
                        }
                        else
                        {
                            //打印status字段,表示音频数据是否已结束。
                            ESP_LOGI(TAG, "ERROR: status is not 2, continue receiving");
                        }
                    }
                    else
                    {
                        ESP_LOGI(TAG, "data field not found or not an array");
                    }
                }           
                // 删除JSON对象
                cJSON_Delete(json);


        // 使用官方给的python示例程序，参数都正确的情况下，HMAC signature cannot be verified: fail to retrieve credential报错信息显示APIKey错误，但是仔细检查了确实没有错，示例程序出来参数都没有改
        // if (data->op_code == WS_TRANSPORT_OPCODES_BINARY) {
        //         ESP_LOGI(TAG, "Received binary data: %.*s", data->data_len, (char *)data->data_ptr);  // 接收到二进制数据
        //     } else {
        //         ESP_LOGI(TAG, "Received text data: %.*s", data->data_len, (char *)data->data_ptr);  // 接收到文本数据
        //     }
        //     break;

            }
            else
            {
                // 接收到的数据不是JSON结构
                //ESP_LOGI(TAG, "Received data is not JSON");
            }
            break;

        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "WebSocket disconnected");  // WebSocket断开连接
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(TAG, "WebSocket error");  // WebSocket错误
            break;
        default:
            break;  
    }
}
//wss://tts-api.xfyun.cn/v2/tts?authorization=aG1hYyB1c2VybmFtZT0iZGE0ZjMyOWUyZmQwMGQ1NjE4NjVjNjRkZjU3NDNiMjAiLCBhbGdvcml0aG09ImhtYWMtc2hhMjU2IiwgaGVhZGVycz0iaG9zdCBkYXRlIHJlcXVlc3QtbGluZSIsIHNpZ25hdHVyZT0ic1RtbzRobDBMdmRLWTRLRjltcGJKV0htRFFzNC8xZ2ZPdUgwZnBZbVdnbz0i
//&date=Thu%2C%2001%20Aug%202019%2001%3A53%3A21%20GMT&host=tts-api.xfyun.cn
// URL 编码函数
char *url_encode(const char *str) {
    char *pstr = (char *)str;
    char *buf = (char *)heap_caps_malloc(strlen(str) * 3 + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT); // 每个字符最多变成3个字符
    if (buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for URL encoding");
        return NULL; // 内存分配失败
    }
    char *pbuf = buf;

    while (*pstr) {
        if (isalnum((unsigned char)*pstr)) {
            *pbuf++ = *pstr;
        } else if (*pstr == ' ') {
            strcpy(pbuf, "%20");
            pbuf += 3;
        } else if (*pstr == ',') {
            strcpy(pbuf, "%2C");
            pbuf += 3;
        } else if (*pstr == ':') {
            strcpy(pbuf, "%3A");
            pbuf += 3;
        } else {
            sprintf(pbuf, "%%%02X", (unsigned char)*pstr);
            pbuf += 3;
        }
        pstr++;
    }
    *pbuf = '\0';
    return buf;
}

// 创建鉴权URL函数
char *create_auth_url() {
    // 为了避免多次调用ctime和gmtime，可以提前获取当前时间
    time_t now = time(NULL);
    struct tm *t = gmtime(&now);
    char date[64];
    //Thu, 01 Aug 2019 01:53:21 GMT
    //如果是空格，按照特定规则将其转换为 %20。如果是逗号，转换为 %2C。如果是冒号，转换为 %3A。
    //Thu %2C %20 01 %20 Aug %20 2019 %20 01 %3A 53 %3A 21 %20 GMT
    //Thu %2C %20 01 %20 Jan %20 1970 %20 00 %3A 13 %3A 15 %20 GMT
    strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", t);
    ESP_LOGI(TAG, "date: %s", date);
    // 生成签名字符串
    char signature_origin[256];
    snprintf(signature_origin, sizeof(signature_origin), "host: ws-api.xfyun.cn\ndate: %s\nGET /v2/tts HTTP/1.1", date);
    // 使用HMAC-SHA256进行签名
    byte digest[33];  // SHA256的结果长度为32字节，256位
    Hmac hmac;
    wc_HmacSetKey(&hmac, WC_SHA256, (byte *)APISECRET, strlen(APISECRET));
    wc_HmacUpdate(&hmac, (byte *)signature_origin, strlen(signature_origin));
    wc_HmacFinal(&hmac, digest);

//    char signature[50];  // 使用原来的长度即可
     // 对签名进行Base64编码
    size_t encoded_len = 0;
    mbedtls_base64_encode(NULL, 0, &encoded_len, digest, sizeof(digest));
    char *signature = (char *)heap_caps_malloc(encoded_len + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if(signature == NULL)
    {
        ESP_LOGI(TAG, "malloc signature failed");
        return NULL; // 内存分配失败
    }
    mbedtls_base64_encode((unsigned char *)signature, encoded_len, &encoded_len, digest, sizeof(digest));
    ESP_LOGI(TAG, "signature: %s", signature);
    // 生成Authorization头
    char authorization_origin[256] = {0};
//    char authorization[256];
    // 生成Authorization头
    snprintf(authorization_origin, sizeof(authorization_origin), "api_key=\"%s\", algorithm=\"%s\", headers=\"%s\", signature=\"%s\"",
             APIKEY, "hmac-sha256", "host date request-line", signature);
    free(signature); //释放签名内存        
    // 对Authorization头进行Base64编码
//    encoded_len = sizeof(authorization_origin);
    mbedtls_base64_encode(NULL, 0, &encoded_len, (unsigned char *)authorization_origin, strlen(authorization_origin));
    char *authorization = (char *)heap_caps_malloc(encoded_len + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if(authorization == NULL)
    {
        ESP_LOGI(TAG, "malloc authorization failed");
        return NULL; // 内存分配失败
    }
    mbedtls_base64_encode((unsigned char *)authorization, encoded_len, &encoded_len, (unsigned char *)authorization_origin, strlen(authorization_origin));

//对date进行url编码
    char *url_encoded_date = url_encode(date);
    if (url_encoded_date == NULL) {
        free(authorization);
        ESP_LOGE(TAG, "Failed to encode date for URL");
        return NULL; // URL编码失败
    }
    // 计算URL长度,为 "wss://tts-api.xfyun.cn/v2/tts?authorization=...&date=..." + 1
    size_t url_len = 73 + strlen(authorization) + strlen(url_encoded_date);
    char *url = (char *)heap_caps_malloc(url_len + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (url == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for URL");
        return NULL; // 内存分配失败
    }
    
    // 拼接最终的URL
    snprintf(url, url_len, "wss://tts-api.xfyun.cn/v2/tts?authorization=%s&date=%s&host=tts-api.xfyun.cn",
             authorization, url_encoded_date);

    free(authorization);//释放authorization内存
    free(url_encoded_date);//释放date内存
    ESP_LOGI(TAG, "Authorization URL: %s", url);

    return url;
}

// 启动WebSocket客户端函数
esp_err_t tts_response( char *TTS_TEXT) {
    // 创建鉴权URL
    char *url = create_auth_url();
    if (url == NULL) {
        ESP_LOGE(TAG, "Failed to create Authorization URL");
        return ESP_FAIL; // 创建鉴权URL失败
    }

    // 配置WebSocket客户端
    esp_websocket_client_config_t websocket_cfg = {
        .uri = url,
        .reconnect_timeout_ms = 50000,
        .network_timeout_ms = 50000,
//        .cert_pem = ,
    };
    free(url);//释放url内存
    // 初始化并启动WebSocket客户端
    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);
    esp_websocket_client_start(client);
    // 创建JSON请求数据
    cJSON *root = cJSON_CreateObject();
    cJSON *common = cJSON_CreateObject();
    cJSON *business = cJSON_CreateObject();
    cJSON *data = cJSON_CreateObject();

    cJSON_AddStringToObject(common, "app_id", APPID);
    cJSON_AddItemToObject(root, "common", common);

    cJSON_AddStringToObject(business, "aue", "raw");
    cJSON_AddStringToObject(business, "auf", "audio/L16;rate=16000");//默认16k采样率，可不传
    cJSON_AddStringToObject(business, "vcn", "xiaoyan");
    cJSON_AddStringToObject(business, "tte", "utf8");//数据编码格式
    cJSON_AddItemToObject(root, "business", business);

    char *encoded_text = NULL;
    size_t output_len = 0;
    mbedtls_base64_encode((unsigned char *)encoded_text, 0, &output_len, (unsigned char *)TTS_TEXT, strlen(TTS_TEXT));
    encoded_text = heap_caps_malloc(output_len + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    mbedtls_base64_encode((unsigned char *)encoded_text, output_len + 1, &output_len, (unsigned char *)TTS_TEXT, strlen(TTS_TEXT));
    cJSON_AddStringToObject(data, "status", "2");
    cJSON_AddStringToObject(data, "text", encoded_text);
    cJSON_AddItemToObject(root, "data", data);

    char *payload = cJSON_Print(root);

    free(encoded_text);
    cJSON_Delete(root);//删除JSON对象
    
    // 发送JSON数据到WebSocket服务器
    int err = esp_websocket_client_send_text(client, payload, strlen(payload), portMAX_DELAY_TTS);
    ESP_LOGI(TAG, "WebSocket send text: %d", err);
    if( err == -1)
    {
        ESP_LOGE(TAG, "WebSocket send text failed");
    } 
    free(payload);

    //判断数据是否接收完毕
//    while (tts_data != NULL) {
//        vTaskDelay(pdMS_TO_TICKS(50));//延时50ms
 //   }
// 等待WebSocket连接断开,清除WebSocket客户端资源
    esp_websocket_client_close(client, 1000);
    ESP_LOGI(TAG, "WebSocket client stopped");
    esp_websocket_client_destroy_on_exit(client);
    return ESP_OK;
}

*/





