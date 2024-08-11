/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

/* 
   在这里定义了一些头文件
*/
/*
#include <string.h>                    // 字符串操作函数
#include "freertos/FreeRTOS.h"        // FreeRTOS库，包含任务、事件组等函数
#include "freertos/task.h"            // 任务创建、挂起、清除等函数
#include "freertos/event_groups.h"    // 事件组相关函数
#include "esp_system.h"               // 系统底层函数，如硬件相关（如时钟、GPIO等）
#include "esp_log.h"                  // 日志输出函数
#include "esp_check.h"                // 错误检查函数
#include "nvs_flash.h"                // NVS flash操作函数
#include "app_ui_ctrl.h"             // 用户界面控制函数
#include "OpenAI.h"                   // 集成OpenAI的SDK
#include "audio_player.h"             // 音频播放函数
#include "app_sr.h"                   // 语音识别函数
#include "bsp/esp-bsp.h"              // 硬件相关代码，如显示、按键等
#include "bsp_board.h"                // 硬件相关代码，如网络连接等
#include "app_audio.h"                // 音频处理函数
#include "app_wifi.h"                 // WiFi连接函数
#include "settings.h"                 // 一些设置项，如API密钥等




//   定义了一些常量

#define SCROLL_START_DELAY_S            (1.5)
#define LISTEN_SPEAK_PANEL_DELAY_MS     2000
#define SERVER_ERROR                    "server_error"
#define INVALID_REQUEST_ERROR           "invalid_request_error"
#define SORRY_CANNOT_UNDERSTAND         "Sorry, I can't understand."
#define API_KEY_NOT_VALID               "API Key is not valid"


//   定义了一个静态字符指针变量，并初始化了它

static char *TAG = "app_main";
static sys_param_t *sys_param = NULL;

//   在app_audio.c中调用的程序流程。 

esp_err_t start_openai(uint8_t *audio, int audio_len)
{
    esp_err_t ret = ESP_OK;
    // 定义静态变量指针
    static OpenAI_t *openai = NULL;
    static OpenAI_AudioTranscription_t *audioTranscription = NULL;
    static OpenAI_ChatCompletion_t *chatCompletion = NULL;
    static OpenAI_AudioSpeech_t *audioSpeech = NULL;
    // 定义指针
    OpenAI_SpeechResponse_t *speechresult = NULL;
    OpenAI_StringResponse_t *result = NULL;
    FILE *fp = NULL;

    if (openai == NULL) {
        // 创建OpenAI对象
        openai = OpenAICreate(sys_param->key);
        ESP_RETURN_ON_FALSE(NULL != openai, ESP_ERR_INVALID_ARG, TAG, "OpenAICreate faield");

        OpenAIChangeBaseURL(openai, sys_param->url);

        // 创建OpenAI的音频转录对象
        audioTranscription = openai->audioTranscriptionCreate(openai);
        chatCompletion = openai->chatCreate(openai);
        audioSpeech = openai->audioSpeechCreate(openai);

        // 设置音频转录对象的参数
        audioTranscription->setResponseFormat(audioTranscription, OPENAI_AUDIO_RESPONSE_FORMAT_JSON);
        audioTranscription->setLanguage(audioTranscription, "en");
        audioTranscription->setTemperature(audioTranscription, 0.2);

        // 设置聊天补全对象的参数
        chatCompletion->setModel(chatCompletion, "gpt-3.5-turbo");
        chatCompletion->setSystem(chatCompletion, "user");
        chatCompletion->setMaxTokens(chatCompletion, CONFIG_MAX_TOKEN);
        chatCompletion->setTemperature(chatCompletion, 0.2);
        chatCompletion->setStop(chatCompletion, "\r");
        chatCompletion->setPresencePenalty(chatCompletion, 0);
        chatCompletion->setFrequencyPenalty(chatCompletion, 0);
        chatCompletion->setUser(chatCompletion, "OpenAI-ESP32");

        // 设置音频语音对象的参数
        // 设置音频模型的类型
        audioSpeech->setModel(audioSpeech, "tts-1");
        // 设置音频的语音类型
        audioSpeech->setVoice(audioSpeech, "nova");
        // 设置音频的输出格式
        audioSpeech->setResponseFormat(audioSpeech, OPENAI_AUDIO_OUTPUT_FORMAT_MP3);
        // 设置音频的播放速度
        audioSpeech->setSpeed(audioSpeech, 1.0);
    }

    // 显示面板
    ui_ctrl_show_panel(UI_CTRL_PANEL_GET, 0);

    // OpenAI音频转录
    char *text = audioTranscription->file(audioTranscription, (uint8_t *)audio, audio_len, OPENAI_AUDIO_INPUT_FORMAT_WAV);

    if (NULL == text) {
        ret = ESP_ERR_INVALID_RESPONSE;
        ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, INVALID_REQUEST_ERROR);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "[audioTranscription]: invalid url");
    }

    if (strstr(text, "\"code\": ")) {
        ret = ESP_ERR_INVALID_RESPONSE;
        ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, text);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "[audioTranscription]: invalid response");
    }

    if (strcmp(text, INVALID_REQUEST_ERROR) == 0 || strcmp(text, SERVER_ERROR) == 0) {
        ret = ESP_ERR_INVALID_RESPONSE;
        ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, SORRY_CANNOT_UNDERSTAND);
        ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, LISTEN_SPEAK_PANEL_DELAY_MS);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "[audioTranscription]: invalid response");
    }

    // UI显示监听成功
    ui_ctrl_label_show_text(UI_CTRL_LABEL_REPLY_QUESTION, text);
    ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, text);

    // openAI聊天补全
    result = chatCompletion->message(chatCompletion, text, false);
    if (NULL == result) {
        ret = ESP_ERR_INVALID_RESPONSE;
        ESP_GOTO_ON_ERROR(ret, err, TAG, "[chatCompletion]: invalid response");
    }

    char *response = result->getData(result, 0);

    if (response != NULL && (strcmp(response, INVALID_REQUEST_ERROR) == 0 || strcmp(response, SERVER_ERROR) == 0)) {
        // UI显示监听失败
        ret = ESP_ERR_INVALID_RESPONSE;
        ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, SORRY_CANNOT_UNDERSTAND);
        ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, LISTEN_SPEAK_PANEL_DELAY_MS);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "[chatCompletion]: invalid response");
    }

    // UI显示监听成功
    ui_ctrl_label_show_text(UI_CTRL_LABEL_REPLY_QUESTION, text);
    ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, response);

    if (strcmp(response, INVALID_REQUEST_ERROR) == 0) {
        ret = ESP_ERR_INVALID_RESPONSE;
        ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, SORRY_CANNOT_UNDERSTAND);
        ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, LISTEN_SPEAK_PANEL_DELAY_MS);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "[chatCompletion]: invalid response");
    }

    ui_ctrl_label_show_text(UI_CTRL_LABEL_REPLY_CONTENT, response);
    ui_ctrl_show_panel(UI_CTRL_PANEL_REPLY, 0);

    // OpenAI音频语音回应
    speechresult = audioSpeech->speech(audioSpeech, response);
    if (NULL == speechresult) {
        ret = ESP_ERR_INVALID_RESPONSE;
        ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, 5 * LISTEN_SPEAK_PANEL_DELAY_MS);
        fp = fopen("/spiffs/tts_failed.mp3", "r");
        if (fp) {
            audio_player_play(fp);
        }
        ESP_GOTO_ON_ERROR(ret, err, TAG, "[audioSpeech]: invalid response");
    }

    uint32_t dataLength = speechresult->getLen(speechresult);
    char *speechptr = speechresult->getData(speechresult);
    esp_err_t status = ESP_FAIL;
    fp = fmemopen((void *)speechptr, dataLength, "rb");
    if (fp) {
        status = audio_player_play(fp);
    }

    if (status != ESP_OK) {
        ESP_LOGE(TAG, "Error creating ChatGPT request: %s\n", esp_err_to_name(status));
        // UI回复音频失败
        ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, 0);
    } else {
        // 在开始滚动回复内容之前等一会儿
        vTaskDelay(pdMS_TO_TICKS(SCROLL_START_DELAY_S * 1000));
        ui_ctrl_reply_set_audio_start_flag(true);
    }

err:
    // 清理资源
    if (speechresult) {
        speechresult->deleteResponse(speechresult);
    }

    if (result) {
        result->deleteResponse(result);
    }

    if (text) {
        free(text);
    }
    return ret;
}
*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "app_ui_ctrl.h"
#include "tts_api.h"
#include "app_sr.h"
#include "bsp/esp-bsp.h"
#include "bsp_board.h"
#include "app_audio.h"
#include "app_wifi.h"
#include "settings.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "cJSON.h"

#include "stt_api.h"
#include "chat_api.h"

#define SCROLL_START_DELAY_S (1.5)
static char *TAG = "app_main";
static sys_param_t *sys_param = NULL;

/**
 * @brief 启动 OpenAI 语音识别并获取回答
 *
 * 该函数负责将音频数据发送到百度语音识别服务进行转换，然后基于转换后的文本获取 ChatGPT 的回答。
 * 接着，根据回答的内容更新 UI，并处理可能的错误情况。
 *
 * @param audio 指向包含音频数据的缓冲区的指针。
 * @param audio_len 音频数据的长度。
 * @return esp_err_t 返回操作的结果状态码。如果操作成功，返回 ESP_OK；如果失败，返回 ESP_FAIL。
 */
esp_err_t start_openai(uint8_t *audio, int audio_len)
{
    ESP_LOGE(TAG, "start\n");

    // 调用百度ASR进行语音转文字
    char *recognition_result = baidu_asr(audio, audio_len);
    ESP_LOGE(TAG, "------------text: %s\n", recognition_result);

    // 检查百度ASR返回的结果是否为空，为空则显示错误信息并返回
    if (strlen(recognition_result) == 0)
    {
        ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, "API Key is not valid");
        return ESP_FAIL;
    }

    // 打印用户说的话
    ESP_LOGE(TAG, "user: %s\n", recognition_result);

    // 检查是否收到了无效请求错误
    if (strcmp(recognition_result, "invalid_request_error") == 0)
    {
        ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, "Sorry, I can't understand.");
        ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, 2000);
        return ESP_FAIL;
    }

    // 调用函数获取ChatGPT的回答
    char *response = getGPTAnswer(recognition_result);
    ESP_LOGE(TAG, "------------response: %s\n", response);

    // 更新UI显示
    ui_ctrl_show_panel(UI_CTRL_PANEL_GET, 0);
    ui_ctrl_label_show_text(UI_CTRL_LABEL_REPLY_QUESTION, recognition_result);
    ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, recognition_result);

    // 检查ChatGPT的回答是否为无效请求错误
    if (response != NULL && (strcmp(response, "invalid_request_error") == 0))
    {
        ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, "Sorry, I can't understand.");
        ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, 2000);
        return ESP_FAIL;
    }

    // 更新UI为准备好显示回答的状态
    ui_ctrl_label_show_text(UI_CTRL_LABEL_REPLY_QUESTION, response);
    ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, response);

    // 再次检查回答是否为无效请求错误
    if (strcmp(response, "invalid_request_error") == 0)
    {
        ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, "Sorry, I can't understand.");
        ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, 2000);
        return ESP_FAIL;
    }

    // 准备回答的播放
    ui_ctrl_label_show_text(UI_CTRL_LABEL_REPLY_CONTENT, response);
    ui_ctrl_show_panel(UI_CTRL_PANEL_REPLY, 0);
    ESP_LOGE(TAG, "start tts\n");

    // 调用文本转语音服务
    esp_err_t status = text_to_speech_request(response, AUDIO_CODECS_MP3);

    // 根据文本转语音的结果更新UI
    if (status != ESP_OK)
    {
        ESP_LOGE(TAG, "Error creating ChatGPT request: %s\n", esp_err_to_name(status));
        ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, 0);
    }
    else
    {
        // 延迟一段时间后开始滚动显示回答内容
        vTaskDelay(pdMS_TO_TICKS(SCROLL_START_DELAY_S * 1000));
        ui_ctrl_reply_set_audio_start_flag(true);
    }

    return ESP_OK;
}

// play audio function 

//当音频播放完成时调用此函数
static void audio_play_finish_cb(void)
{
    //打印日志：音频播放结束
    ESP_LOGI(TAG, "replay audio end");
    //如果ui_ctrl_reply_get_audio_start_flag()为真，则设置ui_ctrl_reply_set_audio_end_flag(true);为真
    if (ui_ctrl_reply_get_audio_start_flag()) {
        ui_ctrl_reply_set_audio_end_flag(true);
    }
}

void app_main()
{
    // 初始化 NVS
    esp_err_t ret = nvs_flash_init();
    // 检查 NVS 初始化结果
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // 擦除 NVS 并重新初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    // 从 NVS 中读取参数
    ESP_ERROR_CHECK(settings_read_parameter_from_nvs());
    // 获取系统参数
    sys_param = settings_get_parameter();

    // 挂载 SPIFFS 文件系统
    bsp_spiffs_mount();
    // 初始化 I2C 总线
    bsp_i2c_init();

    // 配置显示屏参数
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_H_RES * CONFIG_BSP_LCD_DRAW_BUF_HEIGHT,
        .double_buffer = 0,
        .flags = {
            .buff_dma = true,
        }
    };
    // 启动显示屏
    bsp_display_start_with_config(&cfg);
    // 初始化板载外设
    bsp_board_init();

    ESP_LOGI(TAG, "Display LVGL demo");
    // 打开显示屏背光
    bsp_display_backlight_on();
    // 初始化用户界面控制
    ui_ctrl_init();
    // 启动网络功能
    app_network_start();

    ESP_LOGI(TAG, "speech recognition start");
    // 启动语音识别
    app_sr_start(false);
    // 注册音频播放结束回调
    audio_register_play_finish_cb(audio_play_finish_cb);

    while (true) {
        // 定期打印内存使用情况
        ESP_LOGD(TAG, "\tDescription\tInternal\tSPIRAM");
        ESP_LOGD(TAG, "Current Free Memory\t%d\t\t%d",
                 heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
                 heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
        ESP_LOGD(TAG, "Min. Ever Free Size\t%d\t\t%d",
                 heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
                 heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM));
        vTaskDelay(pdMS_TO_TICKS(5 * 1000));
    }
}
