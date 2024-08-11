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
#include "app_ui_events.h"
#include "ui.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "stt_api.h"
static const char *TAG = "app_audio";  // 定义日志标签为 "app_audio"
uint8_t Sr_task_flag = 0;  // 语音识别标志，初始为0
extern bool ui_event_flag;
int8_t Volume_value = CONFIG_VOLUME_LEVEL;  // 音量值，初始为配置文件中的音量值

#if !CONFIG_BSP_BOARD_ESP32_S3_BOX_Lite
static bool mute_flag = true;  // 静音标志，默认为静音状态
#endif
bool record_flag = false;  // 录音标志，默认为未录音状态
uint32_t record_total_len = 0;  // 录音总长度，初始为0
uint32_t file_total_len = 0;  // 文件总长度，初始为0
static uint8_t *record_audio_buffer = NULL;  // 录音音频缓冲区，初始为空
uint8_t *audio_rx_buffer = NULL;  // 音频接收缓冲区，初始为空
audio_play_finish_cb_t audio_play_finish_cb = NULL;  // 音频播放完成回调函数，初始为空

extern sr_data_t *g_sr_data;  // 外部定义的语音识别数据结构体
extern esp_err_t start_openai(uint8_t *audio, int audio_len);  // 外部定义的启动OpenAI函数
extern int Cache_WriteBack_Addr(uint32_t addr, uint32_t size);  // 外部定义的缓存写回地址函数

void sr_command_task(uint8_t command_id);

extern bool sr_ai_flag;  
static const char *NVS_MEMU = "memu data";
bool memu_flag = false;
// 在您的代码中使用NVS
esp_err_t Write_memu(char *data ) {
    int8_t memodata_num = 0;
    // 获取当前时间
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm* timeinfo;
    timeinfo = localtime(&tv.tv_sec);
    char *time_str2 = heap_caps_malloc(50, MALLOC_CAP_DEFAULT);
    sprintf(time_str2, "%d年%d月%d日%d时%d分", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
    lv_textarea_set_text(ui_TextAreamemoname,time_str2);
    lv_textarea_set_text(ui_TextAreamemotext,data);
    lv_roller_set_options(ui_MemoText,time_str2,LV_ROLLER_MODE_NORMAL);
    nvs_handle handle;
    ESP_ERROR_CHECK( nvs_open( NVS_MEMU, NVS_READWRITE, &handle) );
    // if(nvs_get_i8 (handle, "memo_num", &memodata_num) == ESP_OK)
    // {
    //     lv_roller_set_options(ui_MemoText,time_str2,LV_ROLLER_MODE_NORMAL);
    //     if(memodata_num >= 20)
    //     memodata_num = -1;
    //     char time_nam[10];
    //     sprintf(time_nam, "text", memodata_num+1);
    //     char time_data[10];
    //     sprintf(time_data, "data", memodata_num+1);
    //     ESP_ERROR_CHECK( nvs_set_i8( handle, "memo_num", memodata_num+1) );
    //     ESP_ERROR_CHECK( nvs_set_str( handle, time_nam, time_str2) );
    //     ESP_ERROR_CHECK( nvs_set_str( handle, time_data, data) );
    // }
    // else
    // {
        lv_roller_set_options(ui_MemoText,time_str2,LV_ROLLER_MODE_NORMAL);
        ESP_ERROR_CHECK(nvs_set_i8(handle, "memo_num", 0));
        ESP_ERROR_CHECK( nvs_set_str( handle, "text", time_str2) );
        ESP_ERROR_CHECK( nvs_set_str( handle, "data", data) );
        ESP_LOGI(TAG, "DATA:%s", data);
    // }
       
    ESP_ERROR_CHECK( nvs_commit(handle) );
    nvs_close(handle);
    memu_flag = true;
    return ESP_OK;
}

/* 主函数 */
void mute_btn_handler(void *handle, void *arg)
{
#if !CONFIG_BSP_BOARD_ESP32_S3_BOX_Lite
    button_event_t event = (button_event_t)arg;  // 获取按钮事件

    if (BUTTON_PRESS_DOWN == event) {
        esp_rom_printf(DRAM_STR("Audio Mute On\r\n"));  // 打印静音开启信息
        mute_flag = true;  // 设置静音标志为真
    } else {
        esp_rom_printf(DRAM_STR("Audio Mute Off\r\n"));  // 打印静音关闭信息
        mute_flag = false;  // 设置静音标志为假
    }
#endif
}

static esp_err_t audio_mute_function(AUDIO_PLAYER_MUTE_SETTING setting)
{
    // 设置音频静音状态
    bsp_codec_mute_set(setting == AUDIO_PLAYER_MUTE ? true : false);
    // 取消静音时恢复音量
    if (setting == AUDIO_PLAYER_UNMUTE) {
        bsp_codec_volume_set(Volume_value, NULL);
    }
    return ESP_OK;
}

static esp_err_t audio_codec_set_fs(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t ch)
{
    esp_err_t ret = ESP_OK;
    // 设置音频采样率和位配置
    ret = bsp_codec_set_fs(rate, bits_cfg, ch);

    // 静音
    bsp_codec_mute_set(true);
    // 取消静音
    bsp_codec_mute_set(false);
    // 设置音量
    bsp_codec_volume_set(Volume_value, NULL);
    // 延迟50毫秒
    vTaskDelay(pdMS_TO_TICKS(50));

    return ret;
}
static void audio_player_cb(audio_player_cb_ctx_t *ctx)
{
    switch (ctx->audio_event) {
    case AUDIO_PLAYER_CALLBACK_EVENT_IDLE:
        ESP_LOGI(TAG, "播放器空闲"); // 打印播放器空闲状态
        bsp_codec_set_fs(16000, 16, 2); // 设置音频采样率、位深度和声道数
        if (audio_play_finish_cb) {
            audio_play_finish_cb(); // 调用播放完成回调函数
        }
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_COMPLETED_PLAYING_NEXT:
        ESP_LOGI(TAG, "播放下一首"); // 打印播放下一首状态
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_PLAYING:
        ESP_LOGI(TAG, "正在播放"); // 打印正在播放状态
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_PAUSE:
        ESP_LOGI(TAG, "播放暂停"); // 打印播放暂停状态
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_SHUTDOWN:
        ESP_LOGI(TAG, "播放器关闭"); // 打印播放器关闭状态
        break;
    default:
        break;
    }
}

void audio_record_init()
{
    /* 如果启用了记录到SD卡，则创建文件 */
#if DEBUG_SAVE_PCM
    // 分配内存给记录音频缓冲区
    record_audio_buffer = heap_caps_calloc(1, FILE_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(record_audio_buffer); // 确保内存分配成功
    printf("successfully created record_audio_buffer with a size: %zu\n", FILE_SIZE);

    // 分配内存给音频接收缓冲区
    audio_rx_buffer = heap_caps_calloc(1, MAX_FILE_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(audio_rx_buffer); // 确保内存分配成功
    printf("audio_rx_buffer with a size: %zu\n", MAX_FILE_SIZE);
#endif

    // 检查缓冲区是否成功分配
    if (record_audio_buffer == NULL || audio_rx_buffer == NULL) {
        printf("Error: Failed to allocate memory for buffers\n");
        return; // 返回或适当地处理错误条件
    }

    // 创建文件迭代器实例
    file_iterator_instance_t *file_iterator = file_iterator_new(BSP_SPIFFS_MOUNT_POINT);
    assert(file_iterator != NULL); // 确保文件迭代器创建成功

    // 配置音频播放器
    audio_player_config_t config = { 
        .mute_fn = audio_mute_function, // 静音函数
        .write_fn = bsp_i2s_write, // 写函数
        .clk_set_fn = audio_codec_set_fs, // 时钟设置函数
        .priority = 5 // 优先级
    };
    ESP_ERROR_CHECK(audio_player_new(config)); // 创建音频播放器实例
    audio_player_callback_register(audio_player_cb, NULL); // 注册音频播放器回调函数
}

void audio_record_save(int16_t *audio_buffer, int audio_chunksize)
{
#if DEBUG_SAVE_PCM
    // 如果记录标志被设置
    if (record_flag) {
        uint16_t *record_buff = (uint16_t *)(record_audio_buffer + sizeof(wav_header_t));
        record_buff += record_total_len;
        for (int i = 0; i < (audio_chunksize - 1); i++) {
            // 检查记录总长度是否超过最大文件大小
            if (record_total_len < (MAX_FILE_SIZE - sizeof(wav_header_t)) / 2) {
#if PCM_ONE_CHANNEL
                // 单声道记录
                record_buff[ i * 1 + 0] = audio_buffer[i * 3 + 0];
                record_total_len += 1;
#else
                // 立体声记录
                record_buff[ i * 2 + 0] = audio_buffer[i * 3 + 0];
                record_buff[ i * 2 + 1] = audio_buffer[i * 3 + 1];
                record_total_len += 2;
#endif
            }
        }
    }
#endif
}

// 注册音频播放完成回调函数
void audio_register_play_finish_cb(audio_play_finish_cb_t cb)
{
    audio_play_finish_cb = cb; // 将传入的回调函数赋值给全局变量 audio_play_finish_cb
}

// 静态函数，用于开始音频录制
static void audio_record_start()
{
#if DEBUG_SAVE_PCM
    ESP_LOGI(TAG, "### record Start"); // 打印日志，表示录音开始
    audio_player_stop(); // 停止音频播放

    record_flag = true; // 设置录音标志为真，表示正在录音
    record_total_len = 0; // 初始化录音总长度为0
    file_total_len = sizeof(wav_header_t); // 初始化文件总长度为wav头文件的大小
#endif
}

// 停止音频录制
static esp_err_t audio_record_stop()
{
    esp_err_t ret = ESP_OK;
#if DEBUG_SAVE_PCM
    // 设置录制标志为停止
    record_flag = false;
#if PCM_ONE_CHANNEL
    // 如果是一个通道，录制总长度不变
    record_total_len *= 1;
#else
    // 如果是两个通道，录制总长度乘以2
    record_total_len *= 2;
#endif
    // 更新文件总长度
    file_total_len += record_total_len;
    ESP_LOGI(TAG, "### record Stop, %" PRIu32 " %" PRIu32 "K", \
             record_total_len, \
             record_total_len / 1024);

    // 打开录制文件
    FILE *fp = fopen("/spiffs/echo_en_wake.wav", "r");
    ESP_GOTO_ON_FALSE(NULL != fp, ESP_FAIL, err, TAG, "Failed create record file");

    // 读取WAV头信息
    wav_header_t wav_head;
    int len = fread(&wav_head, 1, sizeof(wav_header_t), fp);
    ESP_GOTO_ON_FALSE(len > 0, ESP_FAIL, err, TAG, "Failed create record file");

    // 设置采样率
    wav_head.SampleRate = 16000;
#if PCM_ONE_CHANNEL
    // 设置通道数为1
    wav_head.NumChannels = 1;
#else
    // 设置通道数为2
    wav_head.NumChannels = 2;
#endif
    // 设置每个样本的位数
    wav_head.BitsPerSample = 16;
    // 设置数据块的大小
    wav_head.ChunkSize = file_total_len - 8;
    // 设置字节速率
    wav_head.ByteRate = wav_head.SampleRate * wav_head.BitsPerSample * wav_head.NumChannels / 8;
    // 设置子块2的ID为"data"
    wav_head.Subchunk2ID[0] = 'd';
    wav_head.Subchunk2ID[1] = 'a';
    wav_head.Subchunk2ID[2] = 't';
    wav_head.Subchunk2ID[3] = 'a';
    // 设置子块2的大小
    wav_head.Subchunk2Size = record_total_len;
    // 将WAV头信息复制到录音缓冲区
    memcpy((void *)record_audio_buffer, &wav_head, sizeof(wav_header_t));
    // 将录音缓冲区的数据写回
    Cache_WriteBack_Addr((uint32_t)record_audio_buffer, record_total_len);

#endif
err:
    // 关闭文件
    if (fp) {
        fclose(fp);
    }
    return ret;
}
esp_err_t audio_play_task(void *filepath)
{
    FILE *fp = NULL; // 文件指针
    struct stat file_stat; // 文件状态结构体
    esp_err_t ret = ESP_OK; // 返回状态

    const size_t chunk_size = 4096; // 数据块大小
    uint8_t *buffer = malloc(chunk_size); // 分配缓冲区
    ESP_GOTO_ON_FALSE(NULL != buffer, ESP_FAIL, EXIT, TAG, "缓冲区分配失败"); // 检查缓冲区分配

    ESP_GOTO_ON_FALSE(-1 != stat(filepath, &file_stat), ESP_FAIL, EXIT, TAG, "获取文件状态失败"); // 获取文件状态

    fp = fopen(filepath, "r"); // 打开文件
    ESP_GOTO_ON_FALSE(NULL != fp, ESP_FAIL, EXIT, TAG, "创建录音文件失败"); // 检查文件打开

    wav_header_t wav_head; // WAV文件头结构体
    int len = fread(&wav_head, 1, sizeof(wav_header_t), fp); // 读取WAV文件头
    ESP_GOTO_ON_FALSE(len > 0, ESP_FAIL, EXIT, TAG, "读取WAV文件头失败"); // 检查文件头读取

    if (NULL == strstr((char *)wav_head.Subchunk1ID, "fmt") &&
            NULL == strstr((char *)wav_head.Subchunk2ID, "data")) {
        ESP_LOGI(TAG, "PCM格式"); // 检查是否为PCM格式
        fseek(fp, 0, SEEK_SET); // 重置文件指针
        wav_head.SampleRate = 16000; // 设置采样率
        wav_head.NumChannels = 2; // 设置声道数
        wav_head.BitsPerSample = 16; // 设置采样位数
    }

    ESP_LOGI(TAG, "采样率= %" PRIi32 ", 声道数=%d, 采样位数=%d", wav_head.SampleRate, wav_head.NumChannels, wav_head.BitsPerSample); // 打印音频参数
    bsp_codec_set_fs(wav_head.SampleRate, wav_head.BitsPerSample, I2S_SLOT_MODE_STEREO); // 设置音频参数

    bsp_codec_mute_set(true); // 静音
    bsp_codec_mute_set(false); // 取消静音
    bsp_codec_volume_set(Volume_value, NULL); // 设置音量

    size_t cnt, total_cnt = 0; // 读取和写入的字节数
    do {
        /* 从文件中以块的形式读取数据到缓冲区 */
        len = fread(buffer, 1, chunk_size, fp);
        if (len <= 0) {
            break; // 读取结束
        } else if (len > 0) {
            bsp_i2s_write(buffer, len, &cnt, portMAX_DELAY); // 写入I2S
            total_cnt += cnt; // 累加写入字节数
        }
    } while (1);

EXIT:
    if (fp) {
        fclose(fp); // 关闭文件
    }
    if (buffer) {
        free(buffer); // 释放缓冲区
    }
    return ret; // 返回状态
}

/**
 * @brief 语音识别处理任务函数
 * @param pvParam 任务参数，此处未使用
 */
void sr_handler_task(void *pvParam)
{
#if !CONFIG_BSP_BOARD_ESP32_S3_BOX_Lite
    // 静态变量，用于存储静音状态
    static bool mute_state = false;
    // 获取静音按钮的GPIO电平状态
    mute_flag = gpio_get_level(BSP_BUTTON_MUTE_IO);
    // 打印当前静音状态
    printf("sr handle task, mute:%d\n", mute_flag);
#endif

    // 无限循环处理语音识别任务
    while (true) {
        // 如果需要删除任务并且事件组中有相关事件
        if (NEED_DELETE && xEventGroupGetBits(g_sr_data->event_group)) {
            // 设置事件组中的删除标志位
            xEventGroupSetBits(g_sr_data->event_group, HANDLE_DELETED);
            // 删除当前任务
            vTaskDelete(NULL);
        }

        // 初始化语音识别结果结构体
        sr_result_t result = {
            .wakenet_mode = WAKENET_NO_DETECT, // 未检测到唤醒词
            .state = ESP_MN_STATE_DETECTING,  // 检测状态
        };

        // 获取语音识别结果，设置超时时间为1秒
        app_sr_get_result(&result, pdMS_TO_TICKS(1 * 1000));

#if !CONFIG_BSP_BOARD_ESP32_S3_BOX_Lite
        // 如果静音状态发生变化
        if (mute_state != mute_flag) {
            mute_state = mute_flag;
            // 如果静音状态为关闭
            if (false == mute_state) {
                // 设置音频采样率为16000，位深为16，通道数为2
                bsp_codec_set_fs(16000, 16, 2);
            }
        }
#endif
        // 如果检测状态为超时
        if (ESP_MN_STATE_TIMEOUT == result.state) {
            ESP_LOGI(TAG, "ESP_MN_STATE_TIMEOUT"); // 打印超时信息
            audio_record_stop(); // 停止录音

            // if(Sr_task_flag == 0 && ui_event_flag == false)
            // {
            //     ui_event_flag = true;
            //     if(sr_ai_flag == true)
            //     {
            //         // 打开等待提示音文件
            //     FILE *fp = fopen("/spiffs/waitPlease.mp3", "r");
            //     if (fp) {
            //         audio_player_play(fp); // 播放等待提示音
            //     }
            //     }
                
            //     } 
             
            // 如果WiFi已连接
            if (WIFI_STATUS_CONNECTED_OK == wifi_connected_already()) {
                // 启动OpenAI处理录音数据
                if(Sr_task_flag == 1)
                {
                    FILE *fp = fopen("/spiffs/waitPlease.mp3", "r");
                        if (fp) {
                        audio_player_play(fp); // 播放等待提示音
                        }
                    Sr_task_flag = 0;
                    char *stt_result = baidu_stt((uint8_t *)record_audio_buffer, record_total_len);
                    // 检查百度ASR返回的结果是否为空，为空则显示错误信息并返回
                    if (stt_result == NULL)
                    {
                        lv_textarea_add_text(ui_TextAreamemoname,"没听清楚，请重新记录");
                    }
                    else
                    {
                        //序号
                        if(Write_memu(stt_result) != ESP_OK)
                        lv_textarea_add_text(ui_TextAreamemoname,"记录失败");
                    }
                    free(stt_result);
                }
                else
                {
                    if(ui_event_flag == false)
                    {
                        lv_label_set_text(ui_Label1, "请稍等~~~");
                        FILE *fp = fopen("/spiffs/waitPlease.mp3", "r");
                        if (fp) {
                        audio_player_play(fp); // 播放等待提示音
                        }

                        start_openai((uint8_t *)record_audio_buffer, record_total_len);
                        
                    }
                    // if(sr_ai_flag == true)
                    // {
                        
                    // }
                    // else
                    // {
                    //     audio_play_task("/spiffs/echo_cn_end.wav");
                    //     lv_label_set_text(ui_Label1, "ZZZ~~~");
                    // }
                }
            }
            continue; // 继续下一次循环
        } 
        

        // 如果检测到语音命令
        if (ESP_MN_STATE_DETECTED & result.state) {
            audio_play_task("/spiffs/echo_cn_ok.wav");
            audio_record_stop(); // 停止录音
            ESP_LOGI(TAG, "STOP:%d", result.command_id); // 打印停止信息
            // 播放确认提示音
            lv_label_set_text(ui_Label1, "好的~~~");
            sr_command_task(result.command_id); // 处理语音命令
            vTaskDelay( pdMS_TO_TICKS(50));
            if(Sr_task_flag == 1)
                {
                    audio_record_start(); // 重新开始录音
                }
            continue; // 继续下一次循环
        }

        // 如果检测到唤醒词
        if (WAKENET_DETECTED == result.wakenet_mode) {

        audio_play_task("/spiffs/echo_cn_wake.wav");
        lv_label_set_text(ui_Label1, "我在听~~~");
        ui_event_flag = false;

            audio_record_start(); // 开始录音
            // 显示UI提示正在聆听
//            ui_ctrl_guide_jump();
//            ui_ctrl_show_panel(UI_CTRL_PANEL_LISTEN, 0);
            
            // 播放唤醒提示音
            
            
            continue; // 继续下一次循环
        }
        // if (ESP_MN_STATE_DETECTED & result.state) {
        //     ESP_LOGI(TAG, "STOP:%d", result.command_id);
        //     audio_record_stop();
        //     audio_play_task("/spiffs/echo_cn_ok.wav");
        //     // 在start_openai开始时停止传输
        //     continue;
        // }
    }
    vTaskDelete(NULL); // 删除当前任务
}

