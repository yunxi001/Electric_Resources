/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "app_sr.h"
#include "esp_mn_speech_commands.h"
#include "esp_process_sdkconfig.h"
#include "esp_afe_sr_models.h"
#include "esp_mn_models.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "esp_afe_sr_iface.h"
#include "esp_mn_iface.h"
#include "model_path.h"
#include "bsp_board.h"
#include "app_audio.h"
#include "app_wifi.h"

// 定义TAG，用于日志输出
static const char *TAG = "app_sr";

// 定义afe_handle和models，用于音频前端和模型的操作
static esp_afe_sr_iface_t *afe_handle = NULL;
static srmodel_list_t *models = NULL;
// 定义manul_detect_flag，用于手动检测标志
static bool manul_detect_flag = false;

// 定义g_sr_data，用于存储音频前端操作的数据
sr_data_t *g_sr_data = NULL;

// 定义I2S_CHANNEL_NUM，用于设置I2S通道数
#define I2S_CHANNEL_NUM      2

// 定义record_flag和record_total_len，用于录音标志和录音总长度
extern bool record_flag;
extern uint32_t record_total_len;

// static void audio_feed_task(void *arg)
// 定义一个静态任务，名为audio_feed_task，参数为一个void类型的指针
static void audio_feed_task(void *arg)
{
    // 打印日志
    ESP_LOGI(TAG, "Feed Task");
    // 定义一个unsigned int类型的变量，用于存储读取的数据量
    size_t bytes_read = 0;
    // 定义一个esp_afe_sr_data_t类型的指针，用于存储传入的参数
    esp_afe_sr_data_t *afe_data = (esp_afe_sr_data_t *) arg;
    // 调用afe_handle中的get_feed_chunksize函数，获取音频块大小
    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
    // 定义一个int类型的变量，用于存储音频通道
    int feed_channel = 3;
    // 打印日志
    ESP_LOGI(TAG, "audio_chunksize=%d, feed_channel=%d", audio_chunksize, feed_channel);

    /* Allocate audio buffer and check for result */
    // 分配音频缓冲区，并检查结果
    int16_t *audio_buffer = heap_caps_malloc(audio_chunksize * sizeof(int16_t) * feed_channel, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(audio_buffer);
    // 将分配的缓冲区存储在全局变量g_sr_data中
    g_sr_data->afe_in_buffer = audio_buffer;

    while (true) {
        if (g_sr_data->event_group && xEventGroupGetBits(g_sr_data->event_group)) {
            // 获取事件组中的事件，并将FEED_DELETED标志设置到事件组中
            xEventGroupSetBits(g_sr_data->event_group, FEED_DELETED);
            // 销毁任务
            vTaskDelete(NULL);
        }

        /* Read audio data from I2S bus */
        // 从I2S总线读取音频数据
        bsp_i2s_read((char *)audio_buffer, audio_chunksize * I2S_CHANNEL_NUM * sizeof(int16_t), &bytes_read, portMAX_DELAY);

        /* Channel Adjust */
        // 通道调整
        for (int  i = audio_chunksize - 1; i >= 0; i--) {
            audio_buffer[i * 3 + 2] = 0;
            audio_buffer[i * 3 + 1] = audio_buffer[i * 2 + 1];
            audio_buffer[i * 3 + 0] = audio_buffer[i * 2 + 0];
        }

        /* Checking if WIFI is connected */
        // 检查WIFI是否已连接
        if (WIFI_STATUS_CONNECTED_OK == wifi_connected_already()) {

            /* Feed samples of an audio stream to the AFE_SR */
            // 将音频样本 feed 到AFE_SR中
            afe_handle->feed(afe_data, audio_buffer);
        }
        // 保存音频数据
        audio_record_save(audio_buffer, audio_chunksize);
    }
}
// 静态函数，用于处理音频检测任务
static void audio_detect_task(void *arg)
{
    ESP_LOGI(TAG, "Detection task");
    static afe_vad_state_t local_state;
    static uint8_t frame_keep = 0;

    bool detect_flag = false;
    esp_afe_sr_data_t *afe_data = arg;

    while (true) {
        // 如果需要删除并且检测到事件组的位，则设置删除标志并删除任务
        if (NEED_DELETE && xEventGroupGetBits(g_sr_data->event_group)) {
            xEventGroupSetBits(g_sr_data->event_group, DETECT_DELETED);
            vTaskDelete(g_sr_data->handle_task);
            vTaskDelete(NULL);
        }
        // 从 AFE 处理结果
        afe_fetch_result_t *res = afe_handle->fetch(afe_data);
        if (!res || res->ret_value == ESP_FAIL) {
            ESP_LOGW(TAG, "AFE Fetch Fail");
            continue;
        }
        // 如果唤醒状态是唤醒词检测，则发送结果到消息队列
        if (res->wakeup_state == WAKENET_DETECTED) {
            ESP_LOGI(TAG, LOG_BOLD(LOG_COLOR_GREEN) "wakeword detected");
            sr_result_t result = {
                .wakenet_mode = WAKENET_DETECTED,
                .state = ESP_MN_STATE_DETECTING,
                .command_id = 0,
            };
            xQueueSend(g_sr_data->result_que, &result, 0);
        } 
        // 如果唤醒状态是唤醒词通道已验证或者手动检测标志为真，则发送结果到消息队列
        else if (res->wakeup_state == WAKENET_CHANNEL_VERIFIED || manul_detect_flag) {
            ESP_LOGI(TAG,"AFE_FETCH_CHANNEL_VERIFIED");//记得删除
            detect_flag = true;
            if (manul_detect_flag) {
                manul_detect_flag = false;
                sr_result_t result = {
                    .wakenet_mode = WAKENET_DETECTED,
                    .state = ESP_MN_STATE_DETECTING,
                    .command_id = 0,
                };
                xQueueSend(g_sr_data->result_que, &result, 0);
            }
            frame_keep = 0;
            g_sr_data->afe_handle->disable_wakenet(afe_data);
            ESP_LOGI(TAG, LOG_BOLD(LOG_COLOR_GREEN) "AFE_FETCH_CHANNEL_VERIFIED, channel index: %d\n", res->trigger_channel_id);
        }

        if (true == detect_flag) {
            // 若本地状态与 VAD 状态不同，则重置 frame_keep；否则递增 frame_keep
            if (local_state != res->vad_state) {
                local_state = res->vad_state;
                frame_keep = 0;
            } else {
                frame_keep++;
            }

            // 如果 frame_keep 达到 100 并且 VAD 状态是静音，则发送结果到消息队列，重新启用唤醒网络，并重置检测标志
            if ((100 == frame_keep) && (AFE_VAD_SILENCE == res->vad_state)) {
                sr_result_t result = {
                    .wakenet_mode = WAKENET_NO_DETECT,
                    .state = ESP_MN_STATE_TIMEOUT,
                    .command_id = 0,
                };
                xQueueSend(g_sr_data->result_que, &result, 0);
                g_sr_data->afe_handle->enable_wakenet(afe_data);
                detect_flag = false;
                continue;
            }
        }
    }
    /* 任务永不返回 */
    vTaskDelete(NULL);
}
/*
 * 设置语言
 */
esp_err_t app_sr_set_language(sr_language_t new_lang)
{
    // 如果 g_sr_data 为空，则返回 ESP_ERR_INVALID_STATE
    ESP_RETURN_ON_FALSE(NULL != g_sr_data, ESP_ERR_INVALID_STATE, TAG, "SR is not running");

    // 如果新语言与当前语言相同，则记录警告并返回 ESP_OK
    if (new_lang == g_sr_data->lang) {
        ESP_LOGW(TAG, "nothing to do");
        return ESP_OK;
    } else {
        // 否则更新语言
        g_sr_data->lang = new_lang;
    }

    // 记录设置的语言日志
    ESP_LOGI(TAG, "Set language %s", SR_LANG_EN == g_sr_data->lang ? "EN" : "CN");

    // 如果模型数据存在，则销毁模型数据
    if (g_sr_data->model_data) {
        g_sr_data->multinet->destroy(g_sr_data->model_data);
    }

    // 使用 esp_srmodel_filter 加载 wakenet，并设置到 afe_handle
    char *wn_name = esp_srmodel_filter(models, ESP_WN_PREFIX, "");
    ESP_LOGI(TAG, "load wakenet:%s", wn_name);
    g_sr_data->afe_handle->set_wakenet(g_sr_data->afe_data, wn_name);

    // 返回 ESP_OK
    return ESP_OK;
}
/*
    启动语音识别功能
    record_en: 是否启用录音

    返回值:
    - ESP_OK: 成功
    - 其他: 失败
*/
esp_err_t app_sr_start(bool record_en)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(NULL == g_sr_data, ESP_ERR_INVALID_STATE, TAG, "SR already running");

    // 分配语音识别数据内存
    g_sr_data = heap_caps_calloc(1, sizeof(sr_data_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(NULL != g_sr_data, ESP_ERR_NO_MEM, TAG, "Failed create sr data");

    // 创建结果队列
    g_sr_data->result_que = xQueueCreate(3, sizeof(sr_result_t));
    ESP_GOTO_ON_FALSE(NULL != g_sr_data->result_que, ESP_ERR_NO_MEM, err, TAG, "Failed create result queue");

    // 创建事件组
    g_sr_data->event_group = xEventGroupCreate();
    ESP_GOTO_ON_FALSE(NULL != g_sr_data->event_group, ESP_ERR_NO_MEM, err, TAG, "Failed create event_group");

    BaseType_t ret_val;
    models = esp_srmodel_init("model");
    afe_handle = (esp_afe_sr_iface_t *)&ESP_AFE_SR_HANDLE;
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();

    // 配置唤醒词模型名和AEC初始化
    afe_config.wakenet_model_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL);
    afe_config.aec_init = false;
    ESP_LOGI(TAG, "load wakenet:%s", afe_config.wakenet_model_name);//记得删除
    // 从配置创建语音前端数据处理
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(&afe_config);
    g_sr_data->afe_handle = afe_handle;
    g_sr_data->afe_data = afe_data;

    g_sr_data->lang = SR_LANG_MAX;

    // 设置语音识别语言为英语
    ret = app_sr_set_language(SR_LANG_EN);
    ESP_GOTO_ON_FALSE(ESP_OK == ret, ESP_FAIL, err, TAG,  "Failed to set language");

    // 创建音频前端处理任务  
    ret_val = xTaskCreatePinnedToCore(&audio_feed_task, "Feed Task", 8 * 1024, (void *)afe_data, 5, &g_sr_data->feed_task, 0);
    ESP_GOTO_ON_FALSE(pdPASS == ret_val, ESP_FAIL, err, TAG,  "Failed create audio feed task");

    // 创建音频检测任务
    ret_val = xTaskCreatePinnedToCore(&audio_detect_task, "Detect Task", 10 * 1024, (void *)afe_data, 5, &g_sr_data->detect_task, 1);
    ESP_GOTO_ON_FALSE(pdPASS == ret_val, ESP_FAIL, err, TAG,  "Failed create audio detect task");

    // 创建语音识别处理任务
    ret_val = xTaskCreatePinnedToCore(&sr_handler_task, "SR Handler Task", 8 * 1024, NULL, 5, &g_sr_data->handle_task, 0);
    ESP_GOTO_ON_FALSE(pdPASS == ret_val, ESP_FAIL, err, TAG,  "Failed create audio handler task");

    audio_record_init();

    return ESP_OK;

err:
    // 启动失败时停止语音识别功能
    app_sr_stop();
    return ret;
}

esp_err_t app_sr_stop(void)
{
    // 判断SR是否正在运行
    ESP_RETURN_ON_FALSE(NULL != g_sr_data, ESP_ERR_INVALID_STATE, TAG, "SR is not running");
    // 设置事件组中的标志
    xEventGroupSetBits(g_sr_data->event_group, NEED_DELETE);
    // 等待事件组中的标志被设置
    xEventGroupWaitBits(g_sr_data->event_group, NEED_DELETE | FEED_DELETED | DETECT_DELETED | HANDLE_DELETED, 1, 1, portMAX_DELAY);

    // 删除结果队列
    if (g_sr_data->result_que) {
        vQueueDelete(g_sr_data->result_que);
        g_sr_data->result_que = NULL;
    }

    // 删除事件组
    if (g_sr_data->event_group) {
        vEventGroupDelete(g_sr_data->event_group);
        g_sr_data->event_group = NULL;
    }

    // 关闭文件
    if (g_sr_data->fp) {
        fclose(g_sr_data->fp);
        g_sr_data->fp = NULL;
    }

    // 销毁模型数据
    if (g_sr_data->model_data) {
        g_sr_data->multinet->destroy(g_sr_data->model_data);
    }

    // 销毁AFE数据
    if (g_sr_data->afe_data) {
        g_sr_data->afe_handle->destroy(g_sr_data->afe_data);
    }

    // 释放AFE输入缓冲区
    if (g_sr_data->afe_in_buffer) {
        heap_caps_free(g_sr_data->afe_in_buffer);
    }

    // 释放AFE输出缓冲区
    if (g_sr_data->afe_out_buffer) {
        heap_caps_free(g_sr_data->afe_out_buffer);
    }

    // 释放SR数据
    heap_caps_free(g_sr_data);
    g_sr_data = NULL;
    return ESP_OK;
}

esp_err_t app_sr_get_result(sr_result_t *result, TickType_t xTicksToWait)
{
    // 检查SR是否正在运行
    ESP_RETURN_ON_FALSE(NULL != g_sr_data, ESP_ERR_INVALID_STATE, TAG, "SR is not running");
    // 从SR的队列中接收结果
    xQueueReceive(g_sr_data->result_que, result, xTicksToWait);
    return ESP_OK;
}

esp_err_t app_sr_start_once(void)
{
    // 检查SR是否正在运行
    ESP_RETURN_ON_FALSE(NULL != g_sr_data, ESP_ERR_INVALID_STATE, TAG, "SR is not running");
    // 设置手动检测标志
    manul_detect_flag = true;
    return ESP_OK;
}
