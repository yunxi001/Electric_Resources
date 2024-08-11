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
#include "ui.h"
static model_iface_data_t       *model_data     = NULL; // 模型接口数据指针
static const esp_mn_iface_t     *multinet       = NULL; // 多网络接口指针
//bsp_codec_volume_set(0); // 设置音量为0
static const char *TAG = "app_sr"; // 日志标签

static uint8_t task_count = 0; // 帧数计数器
static esp_afe_sr_iface_t *afe_handle = NULL; // AFE（音频前端）处理句柄
static srmodel_list_t *models = NULL; // 模型列表
static bool manul_detect_flag = false; // 手动检测标志

sr_data_t *g_sr_data = NULL; // 语音识别数据全局指针

#define I2S_CHANNEL_NUM      2 // I2S通道数量

extern bool record_flag; // 录音标志
extern uint32_t record_total_len; // 录音总长度
bool sr_ai_flag = true; // 退出标志

// 定义语音命令数组，包含智能家居控制指令
const char *cmd_phoneme[15] = {
    "da kai zhi neng dui hua", // 打开智能对话 
    "da kai shi jian", // 打开时间 
    "da kai ri li", // 打开日历 
    "da kai yin yue", // 打开音乐  
    "bo fang yin yue", // 播放音乐 
    "ting zhi bo fang", // 停止播放 
    "zeng da sheng yin", // 增大声音 
    "jian xiao sheng yin", // 减小声音 
    "da kai tian qi", // 打开天气 
    "da kai bei wang lu", // 打开备忘录 
    "ji bei wang lu", // 记备忘录 
    "da kai wang luo",// 打开网络 
    "lian jie wang luo",//连接网络 
    "xia yi sho",//build
    "shang yi sho"//run
    // "da kai kong qi jing hua qi", // 打开空气净化器
    // "guan bi kong qi jing hua qi", // 关闭空气净化器
    // "da kai tai deng", // 打开台灯
    // "guan bi tai deng", // 关闭台灯
    // "tai deng tiao liang", // 台灯调亮
    // "tai deng tiao an", // 台灯调暗
    // "da kai deng dai", // 打开灯带
    // "guan bi deng dai", // 关闭灯带
    // "bo fang yin yue", // 播放音乐
    // "ting zhi bo fang", // 停止播放
    // "da kai shi jian", // 打开时间
    // "da kai ri li" // 打开日历
};

// 音频输入任务函数，处理音频数据的读取和处理
static void audio_feed_task(void *arg)
{
    // 记录日志，表示任务开始
    ESP_LOGI(TAG, "Feed Task");
    size_t bytes_read = 0;
    esp_afe_sr_data_t *afe_data = (esp_afe_sr_data_t *) arg;
    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data);
    int feed_channel = 3;
    ESP_LOGI(TAG, "audio_chunksize=%d, feed_channel=%d", audio_chunksize, feed_channel);

    // 分配音频缓冲区并检查结果
    int16_t *audio_buffer = heap_caps_malloc(audio_chunksize * sizeof(int16_t) * feed_channel, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(audio_buffer);
    g_sr_data->afe_in_buffer = audio_buffer;

    while (true) {
        if (g_sr_data->event_group && xEventGroupGetBits(g_sr_data->event_group)) {
            xEventGroupSetBits(g_sr_data->event_group, FEED_DELETED);
            vTaskDelete(NULL);
        }

        // 从I2S总线读取音频数据
        bsp_i2s_read((char *)audio_buffer, audio_chunksize * I2S_CHANNEL_NUM * sizeof(int16_t), &bytes_read, portMAX_DELAY);

        // 调整通道
        for (int  i = audio_chunksize - 1; i >= 0; i--) {
            audio_buffer[i * 3 + 2] = 0;
            audio_buffer[i * 3 + 1] = audio_buffer[i * 2 + 1];
            audio_buffer[i * 3 + 0] = audio_buffer[i * 2 + 0];
        }
/*// 检查WIFI是否连接
        if (WIFI_STATUS_CONNECTED_OK == wifi_connected_already()) {

            // 将音频流样本输入到AFE_SR
            afe_handle->feed(afe_data, audio_buffer);
        }
*/
        // 将音频流样本输入到AFE_SR
        afe_handle->feed(afe_data, audio_buffer);

        audio_record_save(audio_buffer, audio_chunksize);
    }
}
// 音频检测任务函数
static void audio_detect_task(void *arg)
{
    ESP_LOGI(TAG, "Detection task");
    static afe_vad_state_t local_state; // 本地VAD状态
    static uint8_t frame_keep = 0; // 保持帧数计数器

    bool detect_flag = false; // 检测标志
    esp_afe_sr_data_t *afe_data = arg; // 音频前端处理数据

    // 检查音频数据块大小是否与多网络模型一致 
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data); // 获取音频数据块大小
    int mu_chunksize = multinet->get_samp_chunksize(model_data); // 获取多网络模型数据块大小
    assert(mu_chunksize == afe_chunksize); // 确保两者数据块大小一致
    ESP_LOGI(TAG, "------------detect start------------\n");

    while (true) {
        if (NEED_DELETE && xEventGroupGetBits(g_sr_data->event_group)) {
            xEventGroupSetBits(g_sr_data->event_group, DETECT_DELETED);
            vTaskDelete(g_sr_data->handle_task);
            vTaskDelete(NULL);
        }
        afe_fetch_result_t *res = afe_handle->fetch(afe_data); // 获取音频处理结果
        if (!res || res->ret_value == ESP_FAIL) {
            ESP_LOGW(TAG, "AFE Fetch Fail"); // 音频前端处理获取失败
            continue;
        }
        if (res->wakeup_state == WAKENET_DETECTED) {
            ESP_LOGI(TAG, LOG_BOLD(LOG_COLOR_GREEN) "wakeup word detected"); // 唤醒词检测到
            sr_result_t result = {
                .wakenet_mode = WAKENET_DETECTED,
                .state = ESP_MN_STATE_DETECTING,
                .command_id = 0,
            };
            xQueueSend(g_sr_data->result_que, &result, 0); // 发送结果到队列
        } else if (res->wakeup_state == WAKENET_CHANNEL_VERIFIED || manul_detect_flag) {
            detect_flag = true; // 设置检测标志
            if (manul_detect_flag) {
                manul_detect_flag = false;
                sr_result_t result = {
                    .wakenet_mode = WAKENET_DETECTED,
                    .state = ESP_MN_STATE_DETECTING,
                    .command_id = 0,
                };
                xQueueSend(g_sr_data->result_que, &result, 0); // 发送结果到队列
            }
            frame_keep = 0; // 重置帧数计数器
            g_sr_data->afe_handle->disable_wakenet(afe_data); // 禁用唤醒网络
            ESP_LOGI(TAG, LOG_BOLD(LOG_COLOR_GREEN) "AFE_FETCH_CHANNEL_VERIFIED, channel index: %d\n", res->trigger_channel_id); // 通道验证通过
        }

        if (true == detect_flag) {

                 if (local_state != res->vad_state) {
                local_state = res->vad_state; // 更新本地VAD状态
                frame_keep = 0; // 重置帧数计数器
            } else {
                frame_keep++; // 增加帧数计数器
            }
//            ESP_LOGI(TAG, "VAD state: %d, frame_keep: %d", res->vad_state, frame_keep);
            // if(res->vad_state == AFE_VAD_SPEECH)
            // {
            //     task_count++;
            //     if(task_count >= 100)
            //     {task_count = 100; }
            // }
            // else
            // {
            //     task_count = 0;
            // }
            if ((150 == frame_keep) && (AFE_VAD_SILENCE == res->vad_state)) {
                            sr_result_t result = {
                                .wakenet_mode = WAKENET_NO_DETECT,
                                .state = ESP_MN_STATE_TIMEOUT,
                                .command_id = 0,//注意
                            };
                            
                            // if(task_count == 100)
                            // {
                            //     task_count = 0;
                            //     detect_flag = false; // 重置检测标志
                            //     sr_ai_flag = false; // 退出标志
                            // }
                                xQueueSend(g_sr_data->result_que, &result, 0); // 发送结果到队列
                                g_sr_data->afe_handle->enable_wakenet(afe_data); // 启用唤醒网络
                                detect_flag = false; // 重置检测标志
                                
                            continue;
                        }

            esp_mn_state_t mn_state = ESP_MN_STATE_DETECTING;//初始化检测状态
            mn_state = multinet->detect(model_data, res->data); // 进行语音命令词匹配
            if (ESP_MN_STATE_DETECTING == mn_state) {
                continue;
            }

           

///应该在回答结束后再进行这个循环，若无问话且超时，则发送超时结果，并置超时标志位，重新检测
            ///直到超时，发送检测结果，并置超时标志位
            
            // if (ESP_MN_STATE_TIMEOUT == mn_state) {
            //     ESP_LOGW(TAG, "超时");
            //     sr_result_t result = {
            //         .wakenet_mode = WAKENET_NO_DETECT,
            //         .state = mn_state,
            //         .command_id = 0,
            //     };
            //     xQueueSend(g_sr_data->result_que, &result, 10); // 将结果发送到队列
            //     afe_handle->enable_wakenet(afe_data); // 启用唤醒网络
            //     detect_flag = false; // 重置检测标志位
            //     continue;
            // }
            // 语音命令词匹配成功
            if (ESP_MN_STATE_DETECTED == mn_state) {
                esp_mn_results_t *mn_result = multinet->get_results(model_data); // 获取检测结果
                for (int i = 0; i < mn_result->num; i++) {
                    ESP_LOGI(TAG, "TOP %d, command_id: %d, phrase_id: %d, prob: %f",
                            i + 1, mn_result->command_id[i], mn_result->phrase_id[i], mn_result->prob[i]);
                }

                int sr_command_id = mn_result->command_id[0];
                ESP_LOGI(TAG, "检测到的命令 : %d", sr_command_id);
                sr_result_t result = {
                    .wakenet_mode = WAKENET_NO_DETECT,
                    .state = mn_state,
                    .command_id = sr_command_id,//注意
                };
                xQueueSend(g_sr_data->result_que, &result, 0); // 将结果发送到队列
#if !SR_CONTINUE_DET//目前连续检测知道超时，开启后每次唤醒注检测一下命令词
                afe_handle->enable_wakenet(afe_data); // 启用唤醒网络
                detect_flag = false; // 重置检测标志位
#endif
                continue;
            }

            
        }
    }
    /* Task never returns */
    vTaskDelete(NULL); 
}



// 设置语音识别的语言
esp_err_t app_sr_set_language(sr_language_t new_lang)
{
    // 检查语音识别数据是否存在
    ESP_RETURN_ON_FALSE(NULL != g_sr_data, ESP_ERR_INVALID_STATE, TAG, "SR is not running");

    // 如果新语言与当前语言相同，则不执行任何操作
    if (new_lang == g_sr_data->lang) {
        ESP_LOGW(TAG, "nothing to do");
        return ESP_OK;
    } else {
        // 更新语言设置
        g_sr_data->lang = new_lang;
    }
    // 记录新语言设置
    ESP_LOGI(TAG, "Set language %s", SR_LANG_EN == g_sr_data->lang ? "EN" : "CN");
    // 如果模型数据存在，则销毁旧模型数据
    if (g_sr_data->model_data) {
        g_sr_data->multinet->destroy(g_sr_data->model_data);
    }
    // 获取并记录新的唤醒模型名称
    char *wn_name = esp_srmodel_filter(models, ESP_WN_PREFIX, "");
    ESP_LOGI(TAG, "load wakenet:%s", wn_name);
    // 设置新的唤醒模型
    g_sr_data->afe_handle->set_wakenet(g_sr_data->afe_data, wn_name);
    return ESP_OK;
}


// 启动语音识别功能
esp_err_t app_sr_start(bool record_en)
{
    esp_err_t ret = ESP_OK;
    // 检查语音识别是否已经在运行
    ESP_RETURN_ON_FALSE(NULL == g_sr_data, ESP_ERR_INVALID_STATE, TAG, "SR already running");

    // 分配内存给语音识别数据结构
    g_sr_data = heap_caps_calloc(1, sizeof(sr_data_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(NULL != g_sr_data, ESP_ERR_NO_MEM, TAG, "Failed create sr data");

    // 创建结果队列
    g_sr_data->result_que = xQueueCreate(3, sizeof(sr_result_t));
    ESP_GOTO_ON_FALSE(NULL != g_sr_data->result_que, ESP_ERR_NO_MEM, err, TAG, "Failed create result queue");

    // 创建事件组
    g_sr_data->event_group = xEventGroupCreate();
    ESP_GOTO_ON_FALSE(NULL != g_sr_data->event_group, ESP_ERR_NO_MEM, err, TAG, "Failed create event_group");

    BaseType_t ret_val;
    // 初始化语音识别模型
    models = esp_srmodel_init("model");
    afe_handle = (esp_afe_sr_iface_t *)&ESP_AFE_SR_HANDLE;
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();

    afe_config.wakenet_model_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL);
    afe_config.aec_init = false;

    // 根据配置创建语音识别数据
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(&afe_config);
    g_sr_data->afe_handle = afe_handle;
    g_sr_data->afe_data = afe_data;

      
    // 查找多网络模型
    char *mn_name = esp_srmodel_filter(models, ESP_MN_CHINESE, NULL);
    if (NULL == mn_name) {
        ESP_LOGE(TAG, "No multinet model found");
        return ESP_FAIL;
    }

    // 初始化多网络模型
    multinet = esp_mn_handle_from_name(mn_name);
    model_data = multinet->create(mn_name, 5760);//模型名字，超时时间
    ESP_LOGI(TAG, "load multinet:%s", mn_name);

    // 清除多网络命令
    esp_mn_commands_clear();

    // 添加命令
    for (int i = 0; i < sizeof(cmd_phoneme) / sizeof(cmd_phoneme[0]); i++) {
        esp_mn_commands_add(i, (char *)cmd_phoneme[i]);
    }

    // 更新命令
    esp_mn_commands_update();
    esp_mn_commands_print();
    multinet->print_active_speech_commands(model_data);

    g_sr_data->lang = SR_LANG_MAX;
    // 设置语言
    ret = app_sr_set_language(SR_LANG_CN);
    ESP_GOTO_ON_FALSE(ESP_OK == ret, ESP_FAIL, err, TAG,  "Failed to set language");

    // 创建音频输入任务
    ret_val = xTaskCreatePinnedToCore(&audio_feed_task, "Feed Task", 8 * 1024, (void *)afe_data, 5, &g_sr_data->feed_task, 0);
    ESP_GOTO_ON_FALSE(pdPASS == ret_val, ESP_FAIL, err, TAG,  "Failed create audio feed task");

    // 创建音频检测任务
    ret_val = xTaskCreatePinnedToCore(&audio_detect_task, "Detect Task", 10 * 1024, (void *)afe_data, 5, &g_sr_data->detect_task, 1);
    ESP_GOTO_ON_FALSE(pdPASS == ret_val, ESP_FAIL, err, TAG,  "Failed create audio detect task");

    // 创建语音识别处理任务
    ret_val = xTaskCreatePinnedToCore(&sr_handler_task, "SR Handler Task", 8 * 1024, NULL, 5, &g_sr_data->handle_task, 0);
    ESP_GOTO_ON_FALSE(pdPASS == ret_val, ESP_FAIL, err, TAG,  "Failed create audio handler task");

    // 初始化音频录制
    audio_record_init();

    return ESP_OK;
err:
    // 停止语音识别
    app_sr_stop();
    return ret;
}
esp_err_t app_sr_stop(void)
{
    // 检查全局数据结构是否有效，如果无效则返回错误状态
    ESP_RETURN_ON_FALSE(NULL != g_sr_data, ESP_ERR_INVALID_STATE, TAG, "SR is not running");
    
    // 设置事件组中的删除标志位
    xEventGroupSetBits(g_sr_data->event_group, NEED_DELETE);
    
    // 等待事件组中的删除标志位和其他相关标志位被设置
    xEventGroupWaitBits(g_sr_data->event_group, NEED_DELETE | FEED_DELETED | DETECT_DELETED | HANDLE_DELETED, 1, 1, portMAX_DELAY);

    // 如果结果队列存在，则删除它并置空
    if (g_sr_data->result_que) {
        vQueueDelete(g_sr_data->result_que);
        g_sr_data->result_que = NULL;
    }

    // 如果事件组存在，则删除它并置空
    if (g_sr_data->event_group) {
        vEventGroupDelete(g_sr_data->event_group);
        g_sr_data->event_group = NULL;
    }

    // 如果文件指针存在，则关闭文件并置空
    if (g_sr_data->fp) {
        fclose(g_sr_data->fp);
        g_sr_data->fp = NULL;
    }

    // 如果模型数据存在，则销毁模型数据
    if (g_sr_data->model_data) {
        g_sr_data->multinet->destroy(g_sr_data->model_data);
    }

    // 如果音频前端数据存在，则销毁音频前端数据
    if (g_sr_data->afe_data) {
        g_sr_data->afe_handle->destroy(g_sr_data->afe_data);
    }

    // 如果音频前端输入缓冲区存在，则释放内存
    if (g_sr_data->afe_in_buffer) {
        heap_caps_free(g_sr_data->afe_in_buffer);
    }

    // 如果音频前端输出缓冲区存在，则释放内存
    if (g_sr_data->afe_out_buffer) {
        heap_caps_free(g_sr_data->afe_out_buffer);
    }

    // 释放全局数据结构的内存并置空
    heap_caps_free(g_sr_data);
    g_sr_data = NULL;
    
    // 返回操作成功
    return ESP_OK;
}


// 获取语音识别结果的函数
esp_err_t app_sr_get_result(sr_result_t *result, TickType_t xTicksToWait)
{
    // 检查全局语音识别数据是否为空，如果为空则返回错误状态
    ESP_RETURN_ON_FALSE(NULL != g_sr_data, ESP_ERR_INVALID_STATE, TAG, "SR is not running");
    // 从结果队列中接收数据
    xQueueReceive(g_sr_data->result_que, result, xTicksToWait);
    return ESP_OK;
}

// 启动一次语音识别的函数
esp_err_t app_sr_start_once(void)//有这个每次唤醒值识别一下AI问答
{
    // 检查全局语音识别数据是否为空，如果为空则返回错误状态
    ESP_RETURN_ON_FALSE(NULL != g_sr_data, ESP_ERR_INVALID_STATE, TAG, "SR is not running");
    // 设置手动检测标志为真
    manul_detect_flag = true;
    return ESP_OK;
}

