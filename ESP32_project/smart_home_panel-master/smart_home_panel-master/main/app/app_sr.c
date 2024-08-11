/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Unlicense OR CC0-1.0
*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "app_sr.h"
#include "esp_afe_sr_models.h"
#include "esp_mn_models.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "app_play_music.h"
#include "app_sr_handler.h"
#include "model_path.h"
#include "esp_mn_speech_commands.h"
#include "esp_process_sdkconfig.h"
#include "bsp_board_extra.h"

#define I2S_CHANNEL_NUM     (2)

static const char *TAG = "app_sr";

static model_iface_data_t       *model_data     = NULL;
static const esp_mn_iface_t     *multinet       = NULL;
static const esp_afe_sr_iface_t *afe_handle     = NULL;
static QueueHandle_t            g_result_que    = NULL;
static srmodel_list_t           *models         = NULL;

const char *cmd_phoneme[12] = {
    "da kai kong qi jing hua qi", // 打开空气净化器
    "guan bi kong qi jing hua qi", // 关闭空气净化器
    "da kai tai deng", // 打开台灯
    "guan bi tai deng", // 关闭台灯
    "tai deng tiao liang", // 台灯调亮
    "tai deng tiao an", // 台灯调暗
    "da kai deng dai", // 打开灯带
    "guan bi deng dai", // 关闭灯带
    "bo fang yin yue", // 播放音乐
    "ting zhi bo fang", // 停止播放
    "da kai shi jian", // 打开时间
    "da kai ri li" // 打开日历
};
// 定义语音命令数组，包含智能家居控制指令


static void audio_feed_task(void *pvParam)
{
    size_t bytes_read = 0; // 读取的字节数
    esp_afe_sr_data_t *afe_data = (esp_afe_sr_data_t *) pvParam; // 将参数转换为esp_afe_sr_data_t类型
    int audio_chunksize = afe_handle->get_feed_chunksize(afe_data); // 获取音频块大小
    ESP_LOGI(TAG, "audio_chunksize=%d, feed_channel=%d", audio_chunksize, 3); // 打印音频块大小和通道数

    /* 分配音频缓冲区并检查结果 */
    int16_t *audio_buffer = heap_caps_malloc(audio_chunksize * sizeof(int16_t) * 3, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (NULL == audio_buffer) {
        esp_system_abort("No mem for audio buffer"); // 如果没有足够的内存，则中止系统
    }

    while (true) {
        /* 从I2S总线读取音频数据 */
        esp_err_t read_result = bsp_extra_i2s_read((char *)audio_buffer, audio_chunksize * I2S_CHANNEL_NUM * sizeof(int16_t), &bytes_read, portMAX_DELAY);
        if (read_result != ESP_OK) {
            ESP_LOGE(TAG, "======== bsp_extra_i2s_read failed =========="); // 读取失败时打印错误信息
        }

        /* 通道调整 */
        for (int  i = audio_chunksize - 1; i >= 0; i--) {
            audio_buffer[i * 3 + 2] = 0; // 第三个通道置零
            audio_buffer[i * 3 + 1] = audio_buffer[i * 2 + 1]; // 复制第二个通道的数据
            audio_buffer[i * 3 + 0] = audio_buffer[i * 2 + 0]; // 复制第一个通道的数据
        }

        /* 将音频流样本喂给AFE_SR */
        afe_handle->feed(afe_data, audio_buffer);
    }

    /* 如果音频喂食结束，进行清理 */
    afe_handle->destroy(afe_data);

    /* 任务永远不会返回 */
    vTaskDelete(NULL);
}

static void audio_detect_task(void *pvParam)
{
    bool detect_flag = false; // 检测标志位，用于标记是否需要进行语音命令检测
    esp_afe_sr_data_t *afe_data = (esp_afe_sr_data_t *) pvParam; // 获取音频前端处理数据

    /* 检查音频数据块大小是否与多网络模型一致 */
    int afe_chunksize = afe_handle->get_fetch_chunksize(afe_data); // 获取音频数据块大小
    int mu_chunksize = multinet->get_samp_chunksize(model_data); // 获取多网络模型数据块大小
    assert(mu_chunksize == afe_chunksize); // 确保两者数据块大小一致
    ESP_LOGI(TAG, "------------检测开始------------\n");

    while (true) {
        afe_fetch_result_t *res = afe_handle->fetch(afe_data); // 获取音频数据处理结果
        if (!res || res->ret_value == ESP_FAIL) {
            ESP_LOGE(TAG, "获取数据错误!");
            continue;
        }

        if (res->wakeup_state == WAKENET_DETECTED) {
            ESP_LOGI(TAG, LOG_BOLD(LOG_COLOR_GREEN) "唤醒词检测到");
            sr_result_t result = {
                .wakenet_mode = WAKENET_DETECTED,
                .state = ESP_MN_STATE_DETECTING,
                .command_id = 0,
            };
            xQueueSend(g_result_que, &result, 10); // 将结果发送到队列
        } else if (res->wakeup_state == WAKENET_CHANNEL_VERIFIED) {
            ESP_LOGI(TAG, LOG_BOLD(LOG_COLOR_GREEN) "通道验证通过");
            detect_flag = true; // 设置检测标志位
            afe_handle->disable_wakenet(afe_data); // 禁用唤醒网络
        }

        if (true == detect_flag) {
            esp_mn_state_t mn_state = ESP_MN_STATE_DETECTING;

            mn_state = multinet->detect(model_data, res->data); // 进行语音命令检测

            if (ESP_MN_STATE_DETECTING == mn_state) {
                continue;
            }

            if (ESP_MN_STATE_TIMEOUT == mn_state) {
                ESP_LOGW(TAG, "超时");
                sr_result_t result = {
                    .wakenet_mode = WAKENET_NO_DETECT,
                    .state = mn_state,
                    .command_id = 0,
                };
                xQueueSend(g_result_que, &result, 10); // 将结果发送到队列
                afe_handle->enable_wakenet(afe_data); // 启用唤醒网络
                detect_flag = false; // 重置检测标志位
                continue;
            }

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
                    .command_id = sr_command_id,
                };
                xQueueSend(g_result_que, &result, 10); // 将结果发送到队列
#if !SR_CONTINUE_DET//目前连续检测知道超时，开启后每次唤醒注检测一下命令词
                afe_handle->enable_wakenet(afe_data); // 启用唤醒网络
                detect_flag = false; // 重置检测标志位
#endif
                continue;
            }
            ESP_LOGE(TAG, "未处理的异常");
        }
    }

    /* 如果音频输入结束，清理资源 */
    afe_handle->destroy(afe_data);

    /* 任务永不返回 */
    vTaskDelete(NULL);
}

#include "esp_netif.h"

esp_err_t app_sr_start(void)
{
    // 初始化网络接口
    esp_err_t netif_init_err = esp_netif_init();
    ESP_RETURN_ON_FALSE(netif_init_err == ESP_OK, ESP_FAIL, TAG, "Failed to initialize network interface");

    // 创建结果队列
    g_result_que = xQueueCreate(1, sizeof(sr_result_t));
    ESP_RETURN_ON_FALSE(NULL != g_result_que, ESP_ERR_NO_MEM, TAG, "Failed create result queue");

    // 初始化语音识别模型
    models = esp_srmodel_init("model");

    // 配置音频前端处理
    afe_handle = &ESP_AFE_SR_HANDLE;
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();

    // 设置唤醒词模型
    afe_config.wakenet_model_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL);
    afe_config.aec_init = false;

    // 创建音频前端处理数据
    esp_afe_sr_data_t *afe_data = afe_handle->create_from_config(&afe_config);
    ESP_LOGI(TAG, "load wakenet:%s", afe_config.wakenet_model_name);

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

    // 创建音频输入任务
    BaseType_t ret_val = xTaskCreatePinnedToCore(audio_feed_task, "Feed Task", 4 * 1024, afe_data, 5, NULL, 1);
    ESP_RETURN_ON_FALSE(pdPASS == ret_val, ESP_FAIL, TAG,  "Failed create audio feed task");

    // 创建音频检测任务
    ret_val = xTaskCreatePinnedToCore(audio_detect_task, "Detect Task", 6 * 1024, afe_data, 5, NULL, 0);
    ESP_RETURN_ON_FALSE(pdPASS == ret_val, ESP_FAIL, TAG,  "Failed create audio detect task");

    // 创建语音识别处理任务
    ret_val = xTaskCreatePinnedToCore(sr_handler_task, "SR Handler Task", 4 * 1024, g_result_que, 1, NULL, 1);
    ESP_RETURN_ON_FALSE(pdPASS == ret_val, ESP_FAIL, TAG,  "Failed create audio handler task");

    return ESP_OK;
}

esp_err_t app_sr_reset_command_list(char *command_list)
{
    // 分配1024字节的内存，使用SPIRAM
    char *err_id = heap_caps_malloc(1024, MALLOC_CAP_SPIRAM);
    // 检查内存分配是否成功，如果不成功则返回错误代码ESP_ERR_NO_MEM
    ESP_RETURN_ON_FALSE(NULL != err_id, ESP_ERR_NO_MEM, TAG, "内存不足");
    // 释放分配的内存
    free(err_id);
    // 返回操作成功
    return ESP_OK;
}
#include <stdio.h>
#include <string.h>

// 定义要检测的中文标点符号
const char* delimiters[] = {"？", "！", "：", "。"};

// 检查当前字符是否是指定的标点符号
int isDelimiter(const char* ch) {
    for (int i = 0; i < sizeof(delimiters) / sizeof(delimiters[0]); ++i) {
        if (strncmp(ch, delimiters[i], 3) == 0) { // 中文符号占3个字节
            return 1;
        }
    }
    return 0;
}

void processText(const char* input) {
    // 初始化一些必要的变量
    int start = 0; // 用于记录每个标点符号之前内容的起始位置
    int lastDelimiterIndex = -1; // 用于记录最后一个标点符号的位置
    int len = strlen(input); // 输入字符串的长度

    // 遍历输入字符串的每一个字符
    for (int i = 0; i < len;) {
        // 检查是否为UTF-8多字节字符（中文）
        if ((unsigned char)input[i] >= 0xE4) { // 中文字符通常从0xE4开始
            if (isDelimiter(&input[i])) {
                // 输出当前标点符号之前的内容
                char temp[i - start + 2];
                strncpy(temp, input + start, i - start); // 复制当前标点符号之前的内容
                temp[i - start] = '\0';
                printf("Chinese symbol: %s\n", temp);
               
                // printf("Before symbol '%.3s': ", &input[i]);
                // for (int j = start; j < i; ++j) {
                //     putchar(input[j]);
                // }
                printf("\n");

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
            // printf("After last symbol: ");
            // for (int i = afterContentIndex; i < len; ++i) {
            //     putchar(input[i]);
            // }
            // printf("\n");
            char temp[len - afterContentIndex + 2];
                strncpy(temp, input + afterContentIndex, len - afterContentIndex); // 复制当前标点符号之前的内容
                temp[len - afterContentIndex] = '\0';
                printf("Chinese symbol: %s\n", temp);
                // if(audio_play_finish_flag == 1)
                // {
                //     if(tts_task(temp) != ESP_OK) 
                //     {
                //         ESP_LOGE(TAG, "Failed to get  tts data");
                //     }
                // }
        }
    }
}

int main() {
    // 定义一个输入字符串
    const char* text = "这是一个测试？看看程序能否正常工作：它应该检测到中文标点符号！逗号、顿号。冒号和句号。";

    // 处理输入字符串并输出结果
    processText(text);

    return 0;
}