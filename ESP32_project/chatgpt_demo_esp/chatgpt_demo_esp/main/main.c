/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "app_ui_ctrl.h"
#include "app_sr.h"
#include "bsp/esp-bsp.h"
#include "bsp_board.h"
#include "app_audio.h"
#include "app_wifi.h"
#include "app_sntp.h"
#include "settings.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "esp_sntp.h"
//#include <esp_netif.h>!!!!!!!!!!!!!!!
#include "tts_api.h"
#include "stt_api.h"
#include "chat_api.h"
#include "ui.h"
#define SCROLL_START_DELAY_S (1.5)
static const char *TAG = "app_main";
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
    
    // char *chat = "你好，请问有什么可以帮助您？";
    // if(tts_response(chat) == ESP_OK)
    // {
    //     ESP_LOGI(TAG, "tts_OK\n");
    //     audio_play_task("/spiffs/tts.wav");
    // } 
 /*
    char * stt_result = baidu_stt(audio, audio_len);
        if(stt_result != NULL)
        {
            ESP_LOGI(TAG, "------------stt_result: %s\n", stt_result);
            char * chat_result = getCHATAnswer(stt_result); 
            if(chat_result != NULL)
            {
                ESP_LOGI(TAG, "------------chat_result: %s\n", chat_result);
            }
            else
            {
                ESP_LOGI(TAG, "chat_result is null\n");
            }
        }
        else
        {
            ESP_LOGI(TAG, "stt_result is null\n");
        }
    
 */     

    
    ESP_LOGE(TAG, "start\n");

    // 调用百度ASR进行语音转文字
    char *stt_result = baidu_stt(audio, audio_len);
    // 检查百度ASR返回的结果是否为空，为空则显示错误信息并返回
    if (stt_result == NULL)
    {
        ESP_LOGE(TAG, "Error: Baidu ASR returns empty result.");
        return ESP_FAIL;
    }
    // 打印百度ASR返回的结果
    ESP_LOGI(TAG, "------------tts_result: %s\n", stt_result);
    // // 检查是否收到了无效请求错误
    // if (strcmp(stt_result, "invalid_request_error") == 0)
    // {
    //     ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, "Sorry, I can't understand.");
    //     ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, 2000);
    //     return ESP_FAIL;
    // }
    // 调用函数获取ChatGPT的回答
    char *chat_result = getCHATAnswer(stt_result); 
    // 检查ChatGPT的回答是否为无效请求错误
    if (chat_result == NULL)
    {
        ESP_LOGE(TAG, "Error: ChatGPT returns null result.");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "------------chat_result: %s\n", chat_result);
    // // 更新UI显示
    // ui_ctrl_show_panel(UI_CTRL_PANEL_GET, 0);
    // ui_ctrl_label_show_text(UI_CTRL_LABEL_REPLY_QUESTION, stt_result);
    // ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, stt_result);
    // // 更新UI为准备好显示回答的状态
    // ui_ctrl_label_show_text(UI_CTRL_LABEL_REPLY_QUESTION, chat_result);
    // ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, chat_result);

    if(tts_respond(chat_result) == ESP_OK){
        ESP_LOGI(TAG, "tts_OK\n");
        lv_label_set_text(ui_Label1, "ZZZ~~~");
    }
    else{
        ESP_LOGE(TAG, "tts_chat_result error\n");
        return ESP_FAIL;
    }
    // // 准备回答的播放
    // ui_ctrl_label_show_text(UI_CTRL_LABEL_REPLY_CONTENT, chat_result);
    // ui_ctrl_show_panel(UI_CTRL_PANEL_REPLY, 0);
    // ESP_LOGE(TAG, "start tts\n");

//     // 调用文本转语音服务
// // 延迟一段时间后开始滚动显示回答内容
//         vTaskDelay(pdMS_TO_TICKS(SCROLL_START_DELAY_S * 1000));
//         ui_ctrl_reply_set_audio_start_flag(true);
    
    return ESP_OK;
}
/* play audio function */

static void audio_play_finish_cb(void)
{
    ESP_LOGI(TAG, "token is null");
//    if (ui_ctrl_reply_get_audio_start_flag()) {
//        ui_ctrl_reply_set_audio_end_flag(true);
//    }
}
// 主函数，程序入口
void app_main()
{
    // 初始化NVS
    // 初始化NVS闪存
    esp_err_t ret = nvs_flash_init();
    // 检查NVS初始化错误
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // 擦除NVS闪存并重新初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    // 检查NVS初始化结果
    ESP_ERROR_CHECK(ret);
    // 从NVS读取参数
//    ESP_ERROR_CHECK(settings_read_parameter_from_nvs());
    // 获取系统参数
    sys_param = settings_get_parameter();

    // 挂载SPIFFS文件系统
    bsp_spiffs_mount();
    // 初始化I2C总线
    bsp_i2c_init();

    // 配置显示参数
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_H_RES * CONFIG_BSP_LCD_DRAW_BUF_HEIGHT,
        .double_buffer = 0,
        .flags = {
            .buff_dma = true,
        }
    };
    
    // 使用配置启动显示
    bsp_display_start_with_config(&cfg);
    // 初始化板级支持包
    bsp_board_init();

    ESP_LOGI(TAG, "Display LVGL demo");
    // 打开显示背光
    bsp_display_backlight_on();
    // 启动网络应用
    app_network_start();

    app_sntp_init();//初始化SNTP
    // 初始化UI控制
    ui_ctrl_init();
    
    ESP_LOGI(TAG, "speech recognition start");
    // 启动语音识别
    app_sr_start(false);
    // 注册音频播放完成回调
   audio_register_play_finish_cb(audio_play_finish_cb);
 
    

    // 无限循环，定期打印内存使用情况
    while (true) {

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

/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
/*

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_check.h"
#include "nvs_flash.h"
#include "app_ui_ctrl.h"
#include "OpenAI.h"
#include "audio_player.h"
#include "app_sr.h"
#include "bsp/esp-bsp.h"
#include "bsp_board.h"
#include "app_audio.h"
#include "app_wifi.h"
#include "settings.h"

#define SCROLL_START_DELAY_S            (1.5)
#define LISTEN_SPEAK_PANEL_DELAY_MS     2000
#define SERVER_ERROR                    "server_error"
#define INVALID_REQUEST_ERROR           "invalid_request_error"
#define SORRY_CANNOT_UNDERSTAND         "Sorry, I can't understand."
#define API_KEY_NOT_VALID               "API Key is not valid"

static char *TAG = "app_main";
static sys_param_t *sys_param = NULL;

// program flow. This function is called in app_audio.c 
esp_err_t start_openai(uint8_t *audio, int audio_len)
{
    esp_err_t ret = ESP_OK;
    static OpenAI_t *openai = NULL;
    static OpenAI_AudioTranscription_t *audioTranscription = NULL;
    static OpenAI_ChatCompletion_t *chatCompletion = NULL;
    static OpenAI_AudioSpeech_t *audioSpeech = NULL;

    OpenAI_SpeechResponse_t *speechresult = NULL;
    OpenAI_StringResponse_t *result = NULL;
    FILE *fp = NULL;

    if (openai == NULL) {
        openai = OpenAICreate(sys_param->key);
        ESP_RETURN_ON_FALSE(NULL != openai, ESP_ERR_INVALID_ARG, TAG, "OpenAICreate faield");

        OpenAIChangeBaseURL(openai, sys_param->url);

        audioTranscription = openai->audioTranscriptionCreate(openai);
        chatCompletion = openai->chatCreate(openai);
        audioSpeech = openai->audioSpeechCreate(openai);

        audioTranscription->setResponseFormat(audioTranscription, OPENAI_AUDIO_RESPONSE_FORMAT_JSON);
        audioTranscription->setLanguage(audioTranscription, "en");
        audioTranscription->setTemperature(audioTranscription, 0.2);

        chatCompletion->setModel(chatCompletion, "gpt-3.5-turbo");
        chatCompletion->setSystem(chatCompletion, "user");
        chatCompletion->setMaxTokens(chatCompletion, CONFIG_MAX_TOKEN);
        chatCompletion->setTemperature(chatCompletion, 0.2);
        chatCompletion->setStop(chatCompletion, "\r");
        chatCompletion->setPresencePenalty(chatCompletion, 0);
        chatCompletion->setFrequencyPenalty(chatCompletion, 0);
        chatCompletion->setUser(chatCompletion, "OpenAI-ESP32");

        audioSpeech->setModel(audioSpeech, "tts-1");
        audioSpeech->setVoice(audioSpeech, "nova");
        audioSpeech->setResponseFormat(audioSpeech, OPENAI_AUDIO_OUTPUT_FORMAT_MP3);
        audioSpeech->setSpeed(audioSpeech, 1.0);
    }

    ui_ctrl_show_panel(UI_CTRL_PANEL_GET, 0);

    // OpenAI Audio Transcription
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

    // UI listen success
    ui_ctrl_label_show_text(UI_CTRL_LABEL_REPLY_QUESTION, text);
    ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, text);

    // OpenAI Chat Completion
    result = chatCompletion->message(chatCompletion, text, false);
    if (NULL == result) {
        ret = ESP_ERR_INVALID_RESPONSE;
        ESP_GOTO_ON_ERROR(ret, err, TAG, "[chatCompletion]: invalid response");
    }

    char *response = result->getData(result, 0);

    if (response != NULL && (strcmp(response, INVALID_REQUEST_ERROR) == 0 || strcmp(response, SERVER_ERROR) == 0)) {
        // UI listen fail
        ret = ESP_ERR_INVALID_RESPONSE;
        ui_ctrl_label_show_text(UI_CTRL_LABEL_LISTEN_SPEAK, SORRY_CANNOT_UNDERSTAND);
        ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, LISTEN_SPEAK_PANEL_DELAY_MS);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "[chatCompletion]: invalid response");
    }

    // UI listen success
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

    // OpenAI Speech Response
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
        // UI reply audio fail
        ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, 0);
    } else {
        // Wait a moment before starting to scroll the reply content
        vTaskDelay(pdMS_TO_TICKS(SCROLL_START_DELAY_S * 1000));
        ui_ctrl_reply_set_audio_start_flag(true);
    }

err:
    // Clearing resources
    if (speechresult) {
        speechresult->deleteResponse (speechresult);
    }

    if (result) {
        result->deleteResponse (result);
    }

    if (text) {
        free(text);
    }
    return ret;
}

// play audio function 

static void audio_play_finish_cb(void)
{
    ESP_LOGI(TAG, "replay audio end");
    if (ui_ctrl_reply_get_audio_start_flag()) {
        ui_ctrl_reply_set_audio_end_flag(true);
    }
}

void app_main()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(settings_read_parameter_from_nvs());
    sys_param = settings_get_parameter();

    bsp_spiffs_mount();
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
    bsp_board_init();

    ESP_LOGI(TAG, "Display LVGL demo");
    bsp_display_backlight_on();
    ui_ctrl_init();
    app_network_start();

    ESP_LOGI(TAG, "speech recognition start");
    app_sr_start(false);
    audio_register_play_finish_cb(audio_play_finish_cb);

    while (true) {

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
*/


/*

    // 创建鉴权URL函数
char *create_auth_url() {
    char date[64];
    char signature_origin[256];
    char signature[64];
    char authorization_origin[256];
    char authorization[256];
    char *url = heap_caps_malloc(512, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (url == NULL) {
        return NULL; // 内存分配失败
    }

    // 获取当前时间并格式化为RFC1123格式
    time_t now = time(NULL);
    struct tm *t = gmtime(&now);
    //Thu, 01 Aug 2019 01:53:21 GMT
    strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", t);
    //如果是空格，按照特定规则将其转换为 %20。如果是逗号，转换为 %2C。如果是冒号，转换为 %3A。
    //Thu %2C %20 01 %20 Aug %20 2019 %20 01 %3A 53 %3A 21 %20 GMT
    char *url_encoded_date = url_encode(date);
    
    // 生成签名字符串
    snprintf(signature_origin, sizeof(signature_origin), "host: ws-api.xfyun.cn\ndate: %s\nGET /v2/tts HTTP/1.1", date);
    // 使用HMAC-SHA256进行签名
    byte digest[256];
    Hmac hmac;
    // 初始化 HMAC 上下文
    wc_HmacSetKey(&hmac, WC_SHA256, (byte *)APISECRET, strlen(APISECRET));
    // 更新 HMAC 上下文
    wc_HmacUpdate(&hmac, (byte *)signature_origin, strlen(signature_origin));
    // 完成 HMAC 计算并获取结果
    wc_HmacFinal(&hmac, digest);
    
//    ESP_LOGI(TAG,"digest: %s", digest);
    // 对签名进行Base64编码
    size_t encoded_len = sizeof(signature);
    mbedtls_base64_encode(NULL, 0, &encoded_len, digest, sizeof(digest));
    mbedtls_base64_encode((unsigned char *)signature, encoded_len, &encoded_len, digest, sizeof(digest));

    ESP_LOGI(TAG, "Signature: %s", signature);
    // 生成Authorization头
    snprintf(authorization_origin, sizeof(authorization_origin), "api_key=\"%s\", algorithm=\"%s\", headers=\"%s\", signature=\"%s\"",
             APIKEY, "hmac-sha256", "host date request-line", signature);
    // 对Authorization头进行Base64编码
    encoded_len = sizeof(authorization);
    mbedtls_base64_encode(NULL, 0, &encoded_len, (unsigned char *)authorization_origin, strlen(authorization_origin));
    mbedtls_base64_encode((unsigned char *)authorization, encoded_len, &encoded_len, (unsigned char *)authorization_origin, strlen(authorization_origin));

    // 拼接最终的URL
    snprintf(url, 512, "wss://tts-api.xfyun.cn/v2/tts?authorization=%s&date=%s&host=ws-api.xfyun.cn",
             authorization, url_encoded_date);
    free(url_encoded_date);
    ESP_LOGI(TAG, "Authorization URL: %s", url);
    return url;
}
*/

/*
Cmake的命令未改过

set(MV_UF2_BIN_EXE ${COMPONENT_PATH}/generateuf2.py)
idf_build_get_property(build_dir BUILD_DIR)
set(nvs_src_file ${PROJECT_DIR}/factory_nvs/build/factory_nvs.bin)
set(nvs_dst_file ${build_dir}/uf2/factory_nvs.bin)

if(NOT EXISTS ${nvs_src_file})
    message(FATAL_ERROR "Please ensure that the 'factory_nvs' is built first, and make sure to keep the 'HMI Board Config' consistent.")
endif()

add_custom_target(nvs_uf2_bin ALL)
add_custom_command(
        TARGET nvs_uf2_bin
        COMMAND ${CMAKE_COMMAND} -E make_directory ${build_dir}/uf2
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${nvs_src_file} ${nvs_dst_file}
        COMMAND ${CMAKE_COMMAND} -E echo "Copying factory_nvs.bin to destination if it is different..."
        VERBATIM)

add_dependencies(flash nvs_uf2_bin)

partition_table_get_partition_info(size "--partition-name ota_0" "size")
partition_table_get_partition_info(offset "--partition-name ota_0" "offset")

if("${size}" AND "${offset}")
    esptool_py_flash_to_partition(flash "ota_0" "${nvs_dst_file}")
else()
    set(message "Failed to find ota_0 in partition table file"
    "Please add a line to the partition file.")
endif()*/