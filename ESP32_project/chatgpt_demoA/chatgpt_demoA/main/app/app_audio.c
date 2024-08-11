/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_timer.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "app_sr.h"
#include "app_audio.h"
#include "bsp_board.h"
#include "bsp/esp-bsp.h"
#include "audio_player.h"
#include "file_iterator.h"
#include "app_ui_ctrl.h"
#include "app_wifi.h"
/* 
   app_audio模块
   该模块主要用于处理音频相关的功能
*/

static const char *TAG = "app_audio";  // 定义日志标签

#if !CONFIG_BSP_BOARD_ESP32_S3_BOX_Lite
static bool mute_flag = true;  // 定义静音标志变量并初始化为true
#endif
bool record_flag = false;  // 定义录音标志变量并初始化为false
uint32_t record_total_len = 0;  // 定义录音总长度变量
uint32_t file_total_len = 0;  // 定义文件总长度变量
static uint8_t *record_audio_buffer = NULL;  // 定义录音音频缓冲区指针并初始化为空
uint8_t *audio_rx_buffer = NULL;  // 定义音频接收缓冲区指针并初始化为空
audio_play_finish_cb_t audio_play_finish_cb = NULL;  // 定义音频播放完成回调函数指针并初始化为空

extern sr_data_t *g_sr_data;  // 导入语音识别数据结构指针
extern esp_err_t start_openai(uint8_t *audio, int audio_len);  // 导入启动openai函数声明
extern int Cache_WriteBack_Addr(uint32_t addr, uint32_t size);  // 导入缓存写回地址函数声明

/* 主要函数 */
void mute_btn_handler(void *handle, void *arg)
{
#if !CONFIG_BSP_BOARD_ESP32_S3_BOX_Lite
    button_event_t event = (button_event_t)arg;

    if (BUTTON_PRESS_DOWN == event) {
        esp_rom_printf(DRAM_STR("Audio Mute On\r\n"));  // 打印日志，音频静音开启
        mute_flag = true;  // 设置静音标志为true
    } else {
        esp_rom_printf(DRAM_STR("Audio Mute Off\r\n"));  // 打印日志，音频静音关闭
        mute_flag = false;  // 设置静音标志为false
    }
#endif
}// 静态方法：控制音频静音/取消静音
static esp_err_t audio_mute_function(AUDIO_PLAYER_MUTE_SETTING setting)
{
    bsp_codec_mute_set(setting == AUDIO_PLAYER_MUTE ? true : false);
    // 在取消静音时恢复语音音量
    if (setting == AUDIO_PLAYER_UNMUTE) {
        bsp_codec_volume_set(CONFIG_VOLUME_LEVEL, NULL);
    }
    return ESP_OK;
}

// 静态方法：设置音频编解码器的采样率、位数配置和通道模式
// 设置音频编解码器的采样率、位配置和通道模式
static esp_err_t audio_codec_set_fs(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t ch)
{
    esp_err_t ret = ESP_OK;
    ret = bsp_codec_set_fs(rate, bits_cfg, ch);

    // 设置编解码器静音
    bsp_codec_mute_set(true);
    // 取消编解码器静音
    bsp_codec_mute_set(false);
    // 设置编解码器音量
    bsp_codec_volume_set(CONFIG_VOLUME_LEVEL, NULL);
    // 延迟50毫秒
    vTaskDelay(pdMS_TO_TICKS(50));

    return ret;
}

// 回调函数：处理音频播放器的回调事件
static void audio_player_cb(audio_player_cb_ctx_t *ctx)
{
    switch (ctx->audio_event) {
    case AUDIO_PLAYER_CALLBACK_EVENT_IDLE:
        ESP_LOGI(TAG, "Player IDLE");
        bsp_codec_set_fs(16000, 16, 2);
        if (audio_play_finish_cb) {
            audio_play_finish_cb();
        }
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_COMPLETED_PLAYING_NEXT:
        ESP_LOGI(TAG, "Player NEXT");
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_PLAYING:
        ESP_LOGI(TAG, "Player PLAYING");
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_PAUSE:
        ESP_LOGI(TAG, "Player PAUSE");
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_SHUTDOWN:
        ESP_LOGI(TAG, "Player SHUTDOWN");
        break;
    default:
        break;
    }
}void audio_record_init()
{
    /* 初始化音频记录模块 */

    /* 如果启用将记录保存到SD卡，则创建文件 */
#if DEBUG_SAVE_PCM
    // 分配内存用于记录音频数据
    record_audio_buffer = heap_caps_calloc(1, FILE_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(record_audio_buffer);
    printf("successfully created record_audio_buffer with a size: %zu\n", FILE_SIZE);
    // 分配内存用于接收音频数据
    audio_rx_buffer = heap_caps_calloc(1, MAX_FILE_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(audio_rx_buffer);
    printf("audio_rx_buffer with a size: %zu\n", MAX_FILE_SIZE);
#endif

    if (record_audio_buffer == NULL || audio_rx_buffer == NULL) {
        printf("Error: Failed to allocate memory for buffers\n");
        return; // 返回或适当处理错误条件
    }

    /* 创建文件迭代器实例 */
    file_iterator_instance_t *file_iterator = file_iterator_new(BSP_SPIFFS_MOUNT_POINT);
    assert(file_iterator != NULL);

    /* 配置音频播放器并注册回调函数 */
    audio_player_config_t config = { .mute_fn = audio_mute_function,
                                     .write_fn = bsp_i2s_write,
                                     .clk_set_fn = audio_codec_set_fs,
                                     .priority = 5
                                   };
    ESP_ERROR_CHECK(audio_player_new(config));
    audio_player_callback_register(audio_player_cb, NULL);
}

void audio_record_save(int16_t *audio_buffer, int audio_chunksize)
{
    /* 保存音频数据到缓冲区 */

#if DEBUG_SAVE_PCM
    if (record_flag) {
        uint16_t *record_buff = (uint16_t *)(record_audio_buffer + sizeof(wav_header_t));
        record_buff += record_total_len;
        for (int i = 0; i < (audio_chunksize - 1); i++) {
            if (record_total_len < (MAX_FILE_SIZE - sizeof(wav_header_t)) / 2) {
#if PCM_ONE_CHANNEL
                record_buff[ i * 1 + 0] = audio_buffer[i * 3 + 0];
                record_total_len += 1;
#else
                record_buff[ i * 2 + 0] = audio_buffer[i * 3 + 0];
                record_buff[ i * 2 + 1] = audio_buffer[i * 3 + 1];
                record_total_len += 2;
#endif
            }
        }
    }
#endif
}/*
 * 注册音频播放完成回调函数
 * 参数 cb: 音频播放完成回调函数
 */
void audio_register_play_finish_cb(audio_play_finish_cb_t cb)
{
    audio_play_finish_cb = cb;
}

/*
 * 开始音频录制
 */
static void audio_record_start()
{
#if DEBUG_SAVE_PCM
    ESP_LOGI(TAG, "### record Start");
    audio_player_stop();

    record_flag = true;
    record_total_len = 0;
    file_total_len = sizeof(wav_header_t);
#endif
}

/*
 * 停止音频录制
 */
static esp_err_t audio_record_stop()
{
    esp_err_t ret = ESP_OK;
#if DEBUG_SAVE_PCM
    record_flag = false;
#if PCM_ONE_CHANNEL
    record_total_len *= 1;
#else
    record_total_len *= 2;
#endif
    file_total_len += record_total_len;
    ESP_LOGI(TAG, "### record Stop, %" PRIu32 " %" PRIu32 "K", \
             record_total_len, \
             record_total_len / 1024);

    FILE *fp = fopen("/spiffs/echo_en_wake.wav", "r");
    ESP_GOTO_ON_FALSE(NULL != fp, ESP_FAIL, err, TAG, "Failed create record file");

    // 读取wav文件头信息
    wav_header_t wav_head;
    int len = fread(&wav_head, 1, sizeof(wav_header_t), fp);
    ESP_GOTO_ON_FALSE(len > 0, ESP_FAIL, err, TAG, "Failed create record file");

    // 修改wav头信息
    wav_head.SampleRate = 16000;
#if PCM_ONE_CHANNEL
    wav_head.NumChannels = 1;
#else
    wav_head.NumChannels = 2;
#endif
    wav_head.BitsPerSample = 16;
    wav_head.ChunkSize = file_total_len - 8;
    wav_head.ByteRate = wav_head.SampleRate * wav_head.BitsPerSample * wav_head.NumChannels / 8;
    wav_head.Subchunk2ID[0] = 'd';
    wav_head.Subchunk2ID[1] = 'a';
    wav_head.Subchunk2ID[2] = 't';
    wav_head.Subchunk2ID[3] = 'a';
    wav_head.Subchunk2Size = record_total_len;
    memcpy((void *)record_audio_buffer, &wav_head, sizeof(wav_header_t));
    Cache_WriteBack_Addr((uint32_t)record_audio_buffer, record_total_len);
#endif

err:
    if (fp) {
        fclose(fp);
    }
    return ret;
}/*
函数名称：audio_play_task
参数：filepath 文件路径
描述：从指定路径读取音频文件并通过I2S接口播放
返回值：esp_err_t类型，表示函数执行状态
*/
esp_err_t audio_play_task(void *filepath)
{
    FILE *fp = NULL;
    struct stat file_stat;
    esp_err_t ret = ESP_OK;

    const size_t chunk_size = 4096;
    uint8_t *buffer = malloc(chunk_size);  // 申请内存用于存储读取的音频数据
    ESP_GOTO_ON_FALSE(NULL != buffer, ESP_FAIL, EXIT, TAG, "buffer malloc failed");  // 内存申请失败时跳转至EXIT

    ESP_GOTO_ON_FALSE(-1 != stat(filepath, &file_stat), ESP_FAIL, EXIT, TAG, "Failed to stat file");  // 获取文件状态失败时跳转至EXIT

    fp = fopen(filepath, "r");  // 以只读方式打开文件
    ESP_GOTO_ON_FALSE(NULL != fp, ESP_FAIL, EXIT, TAG, "Failed create record file");  // 文件打开失败时跳转至EXIT

    wav_header_t wav_head;
    int len = fread(&wav_head, 1, sizeof(wav_header_t), fp);  // 读取音频文件的wav头信息
    ESP_GOTO_ON_FALSE(len > 0, ESP_FAIL, EXIT, TAG, "Read wav header failed");  // 读取失败时跳转至EXIT

    if (NULL == strstr((char *)wav_head.Subchunk1ID, "fmt") &&
            NULL == strstr((char *)wav_head.Subchunk2ID, "data")) {
        ESP_LOGI(TAG, "PCM format");
        // 如果不是WAV格式，则设置默认的样本率、通道数和采样位数
        fseek(fp, 0, SEEK_SET);
        wav_head.SampleRate = 16000;
        wav_head.NumChannels = 2;
        wav_head.BitsPerSample = 16;
    }

    ESP_LOGI(TAG, "frame_rate= %" PRIi32 ", ch=%d, width=%d", wav_head.SampleRate, wav_head.NumChannels, wav_head.BitsPerSample);
    bsp_codec_set_fs(wav_head.SampleRate, wav_head.BitsPerSample, I2S_SLOT_MODE_STEREO);  // 设置解码器的采样率和采样位数

    bsp_codec_mute_set(true);  // 静音
    bsp_codec_mute_set(false);  // 取消静音
    bsp_codec_volume_set(CONFIG_VOLUME_LEVEL, NULL);  // 设置音量

    size_t cnt, total_cnt = 0;
    do {
        /* 将文件以固定大小的块读入缓冲区并通过I2S接口传输 */
        len = fread(buffer, 1, chunk_size, fp);
        if (len <= 0) {
            break;
        } else if (len > 0) {
            bsp_i2s_write(buffer, len, &cnt, portMAX_DELAY);  // 将数据通过I2S接口传输
            total_cnt += cnt;
        }
    } while (1);

EXIT:
    if (fp) {
        fclose(fp);  // 关闭文件
    }
    if (buffer) {
        free(buffer);  // 释放内存
    }
    return ret;  // 返回函数执行状态
}
/*
 * 语音识别处理任务
 */
void sr_handler_task(void *pvParam)
{
#if !CONFIG_BSP_BOARD_ESP32_S3_BOX_Lite
    // 定义一个变量来存储静音状态
    static bool mute_state = false;
    // 获取静音IO的值
    mute_flag = gpio_get_level(BSP_BUTTON_MUTE_IO);
    printf("sr handle task, mute:%d\n", mute_flag);
#endif

    while (true) {
        if (NEED_DELETE && xEventGroupGetBits(g_sr_data->event_group)) {
            xEventGroupSetBits(g_sr_data->event_group, HANDLE_DELETED);
            vTaskDelete(NULL);
        }

        // 获取语音识别结果
        sr_result_t result = {
            .wakenet_mode = WAKENET_NO_DETECT,
            .state = ESP_MN_STATE_DETECTING,
        };
        app_sr_get_result(&result, pdMS_TO_TICKS(1 * 1000));

#if !CONFIG_BSP_BOARD_ESP32_S3_BOX_Lite
        // 检查静音状态是否发生变化，如果发生变化，则设置相应的音频采样频率
        if (mute_state != mute_flag) {
            mute_state = mute_flag;
            if (false == mute_state) {
                bsp_codec_set_fs(16000, 16, 2); 
            }
        }
#endif
        if (ESP_MN_STATE_TIMEOUT == result.state) {
            ESP_LOGI(TAG, "ESP_MN_STATE_TIMEOUT");
            audio_record_stop();
            FILE *fp = fopen("/spiffs/waitPlease.mp3", "r");
            if (fp) {
                audio_player_play(fp);
            }
            if (WIFI_STATUS_CONNECTED_OK == wifi_connected_already()) {
                start_openai((uint8_t *)record_audio_buffer, record_total_len);
            }
            continue;
        }

        if (WAKENET_DETECTED == result.wakenet_mode) {
            audio_record_start();

            // UI显示正在倾听
            ui_ctrl_guide_jump();
            ui_ctrl_show_panel(UI_CTRL_PANEL_LISTEN, 0);

            audio_play_task("/spiffs/echo_en_wake.wav");
            continue;
        }

        if (ESP_MN_STATE_DETECTED & result.state) {
            ESP_LOGI(TAG, "STOP:%d", result.command_id);
            audio_record_stop();
            audio_play_task("/spiffs/echo_en_ok.wav");
            // 在start_openai开始时停止传输
            continue;
        }
    }
    vTaskDelete(NULL);
}