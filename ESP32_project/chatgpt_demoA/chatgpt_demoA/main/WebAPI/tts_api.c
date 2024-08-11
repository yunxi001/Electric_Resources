/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <string.h>
#include"tts_api.h"
#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "app_audio.h"
#include "app_ui_ctrl.h"
#include "audio_player.h"
#include "esp_crt_bundle.h"
#include "inttypes.h"

// 定义宏VOICE_ID和VOLUME，分别赋值为CONFIG_VOICE_ID和CONFIG_VOLUME_LEVEL
#define VOICE_ID CONFIG_VOICE_ID
#define VOLUME CONFIG_VOLUME_LEVEL
FILE *fp = NULL;//
// 定义静态常量指针TAG，并赋值为"TTS-Api"
static const char *TAG = "TTS-Api";

/* Define a function to handle HTTP events during an HTTP request */

/**
 * @brief HTTP客户端事件处理函数
 * 
 * 此函数用于处理HTTP客户端在不同阶段产生的事件，根据事件类型执行相应的处理逻辑。
 * 
 * @param evt 包含事件详细信息的结构体指针。结构体中包含了事件ID和与事件相关的数据。
 * @return esp_err_t 返回ESP_OK表示事件处理成功，其他值表示处理过程中出现错误。
 */
static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        // 处理HTTP事件错误
        ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        // 当HTTP客户端成功连接到服务器时处理
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        // 当HTTP头发送完成后处理
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        // 当接收到HTTP头时处理，重置文件总长度
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
        file_total_len = 0;
        break;
    case HTTP_EVENT_ON_DATA:
        // 当接收到HTTP数据时处理，如果数据长度不超过最大文件大小，则将其存储到缓冲区
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=(%"PRIu32" + %d) [%d]", file_total_len, evt->data_len, MAX_FILE_SIZE);
        if ((file_total_len + evt->data_len) < MAX_FILE_SIZE) {
            memcpy(audio_rx_buffer + file_total_len, (char *)evt->data, evt->data_len);
            file_total_len += evt->data_len;
        }
        break;
    
    case HTTP_EVENT_ON_FINISH:
        // 当HTTP请求完成时处理，播放接收到的音频数据
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH:%" PRIu32 ", %" PRIu32 " K", file_total_len, file_total_len / 1024);
//        uint32_t dataLength = audio_rx_buffer;
//        char *speechptr = file_total_len;
        esp_err_t status = ESP_FAIL;
        fp = fmemopen((void *)audio_rx_buffer, file_total_len, "rb");
        if (fp)
        {
            status = audio_player_play(fp);
        }
        break;
        /*
        case HTTP_EVENT_ON_FINISH:
        // 当HTTP请求完成时处理，播放接收到的音频数据
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH:%"PRIu32", %"PRIu32" K", file_total_len, file_total_len / 1024);
        audio_player_play(audio_rx_buffer, file_total_len);
        break;
        // OpenAI音频语音回应
        speechresult = audioSpeech->speech(audioSpeech, response);
        if (NULL == speechresult)
        {
            ret = ESP_ERR_INVALID_RESPONSE;
            ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, 5 * LISTEN_SPEAK_PANEL_DELAY_MS);
            fp = fopen("/spiffs/tts_failed.mp3", "r");
            if (fp)
            {
                audio_player_play(fp);
            }
            ESP_GOTO_ON_ERROR(ret, err, TAG, "[audioSpeech]: invalid response");
        }

        uint32_t dataLength = speechresult->getLen(speechresult);
        char *speechptr = speechresult->getData(speechresult);
        esp_err_t status = ESP_FAIL;
        fp = fmemopen((void *)speechptr, dataLength, "rb");
        if (fp)
        {
            status = audio_player_play(fp);
        }
        */
        

    case HTTP_EVENT_DISCONNECTED:
        // 当HTTP客户端与服务器断开连接时处理
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        // 当服务器重定向请求时处理
        ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

/* Decode 2 Hex */

/**
 * 将十进制数转换为十六进制字符
 * @param c 十进制数，其值应在0到15之间
 * @return 返回对应的十六进制字符，如果输入不在有效范围内，则返回-1
 */
char dec2hex(short int c)
{
    // 判断十进制数是否在0到9之间
    if (0 <= c && c <= 9) {
        return c + '0'; // 直接将数字转换为对应的ASCII字符
    } else if (10 <= c && c <= 15) {
        // 判断十进制数是否在10到15之间，将其转换为对应的十六进制大写字母
        return c + 'A' - 10;
    } else {
        // 输入值不在有效范围内，返回-1
        return -1;
    }
}

/* Encode URL for playing sound */

/**
 * 对给定的URL进行编码。
 * 
 * 该函数遍历输入的URL字符串，将特殊字符以及非字母数字字符转换为百分号编码格式。
 * 特殊处理了换行符(\n)。
 * 
 * @param url 输入的URL字符串，不可为NULL。
 * @param encode_out 编码后的URL字符串将存储在此参数指向的缓冲区中，不可为NULL。
 * 注意：调用者需要确保encode_out有足够的空间存储编码后的字符串，包括终止空字符。
 */
void url_encode(const char *url, char *encode_out)
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;

    // 确保encode_out不为NULL
    assert(encode_out);

    for (i = 0; i < len; ++i) {
        char c = url[i];
        char n = url[i + 1];
        // 特殊处理换行符
        if (c == '\\' && n == 'n') {
            i += 1; // 跳过'\n'
            continue;
        } else if (('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '/' || c == '.') {
            // 字符在安全字符集内，直接复制
            encode_out[res_len++] = c;
        } else {
            // 对非安全字符进行编码
            int j = (short int)c;
            if (j < 0) {
                j += 256; // 确保字符编码为正数
            }
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1 * 16;
            // 将字符编码为'%xx'格式
            encode_out[res_len++] = '%';
            encode_out[res_len++] = dec2hex(i1);
            encode_out[res_len++] = dec2hex(i0);
        }
    }
    // 添加字符串终止符
    encode_out[res_len] = '\0';
}

/* Create Text to Speech request */

/**
 * 将文本转换为语音请求的函数。
 * 
 * @param message 需要转换为语音的文本消息。
 * @param code_format 音频编解码格式，可以是MP3或WAV。
 * @return 返回执行结果的错误代码。如果执行成功，返回ESP_OK。
 */
esp_err_t text_to_speech_request(const char *message, AUDIO_CODECS_FORMAT code_format)
{
    // 计算输入文本的长度
    size_t message_len = strlen(message);
    // 分配内存以存储编码后的消息
    char *encoded_message;
    char *codec_format_str;
    // 使用动态内存分配来存储编码后的消息
    encoded_message = heap_caps_malloc((3 * message_len + 1), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    // 对消息进行URL编码
    url_encode(message, encoded_message);

    // 根据代码格式选择相应的音频格式字符串
    if (AUDIO_CODECS_MP3 == code_format) {
        codec_format_str = "MP3";
    } else {
        codec_format_str = "WAV";
    }

    // 构造URL的长度
    int url_size = snprintf(NULL, 0, "https://dds.dui.ai/runtime/v1/synthesize?voiceId=%s&text=%s&speed=1&volume=%d&audiotype=%s", \
                            VOICE_ID, \
                            encoded_message, \
                            VOLUME, \
                            codec_format_str);
    // 分配内存以存储URL
    char *url = heap_caps_malloc((url_size + 1), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (url == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for URL");
        return ESP_ERR_NO_MEM; // 内存分配失败
    }
    // 根据变量和预定义字符串构造URL
    snprintf(url, url_size + 1, "https://dds.dui.ai/runtime/v1/synthesize?voiceId=%s&text=%s&speed=1&volume=%d&audiotype=%s", \
             VOICE_ID, \
             encoded_message, \
             VOLUME, \
             codec_format_str);

    // 配置HTTP客户端
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .event_handler = http_event_handler,
        .buffer_size = 128000,
        .buffer_size_tx = 4000,
        .timeout_ms = 40000,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    // 记录函数开始执行的时间，用于日志记录执行时长
    uint32_t starttime = esp_log_timestamp();
    ESP_LOGE(TAG, "[Start] create_TTS_request, timestamp:%"PRIu32, starttime);
    // 初始化HTTP客户端
    esp_http_client_handle_t client = esp_http_client_init(&config);
    // 执行HTTP GET请求
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    // 记录函数执行结束时间，并计算执行时长
    ESP_LOGE(TAG, "[End] create_TTS_request, + offset:%"PRIu32, esp_log_timestamp() - starttime);

    // 释放分配的内存资源
    heap_caps_free(url);
    heap_caps_free(encoded_message);
    // 清理HTTP客户端资源
    esp_http_client_cleanup(client);
    // 返回执行结果
    return err;
}
