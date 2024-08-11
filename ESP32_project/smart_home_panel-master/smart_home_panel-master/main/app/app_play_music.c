/* SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <dirent.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bsp/esp-bsp.h"
#include "audio_player.h"
#include "file_iterator.h"
#include "bsp_board_extra.h"
#include "app_ui_control.h"
#include "app_play_music.h"
#include "ui.h"
static const char *TAG = "app_play_music";  // 定义日志标签为 "app_play_music"

static file_iterator_instance_t *file_iterator;  // 定义文件迭代器实例

static bool isMP3(const char* filename) {  // 定义一个函数，用于检查文件是否为MP3格式
    const char* dot = strrchr(filename, '.');  // 查找文件名中最后一个 '.' 的位置
    if (dot && dot != filename) {  // 如果找到了 '.' 且不在文件名的开头
        if (strcasecmp(dot, ".mp3") == 0) {  // 比较文件扩展名是否为 ".mp3"（不区分大小写）
            return true;  // 如果是MP3文件，返回 true
        }
    }
    return false;  // 如果不是MP3文件，返回 false
}

static void play_index(int index)
{
    // 记录日志，显示当前播放的索引
    ESP_LOGI(TAG, "play_index(%d)", index);

    char filename[128];
    // 从文件迭代器中获取完整路径并存储在filename中
    int retval = file_iterator_get_full_path_from_index(file_iterator, index, filename, sizeof(filename));

    // 从文件迭代器中获取文件名并显示在UI上
    const char* name = file_iterator_get_name_from_index(file_iterator, index);
    lv_label_set_text(ui_MusicName, name);

    // 如果无法获取文件名，记录错误日志并返回
    if (retval == 0) {
        ESP_LOGE(TAG, "unable to retrieve filename");
        return;
    }

    // 打开文件进行读取
    FILE *fp = fopen(filename, "rb");
    if (fp) {
        // 播放音频文件
        audio_player_play(fp);
    } else {
        // 如果无法打开文件，记录错误日志
        ESP_LOGE(TAG, "unable to open index %d, filename '%s'", index, filename);
    }
}
void volume_slider_cb(lv_event_t *event)
{
    // 获取滑块对象
    lv_obj_t *slider = (lv_obj_t *) event->target;
    // 获取滑块的当前值，即音量值
    int volume = lv_slider_get_value(slider);
    // 设置音频编解码器的音量
    bsp_extra_codec_volume_set(volume, NULL);
    // 打印当前音量值到日志
    ESP_LOGI(TAG, "volume '%d'", volume);
}
void music_forward(lv_event_t * e)
{
    // 将文件迭代器移动到下一个文件
    file_iterator_next(file_iterator);

    // 获取音频播放器的状态
    audio_player_state_t state = audio_player_get_state();
    if (state == AUDIO_PLAYER_STATE_IDLE) {
        // 如果播放器处于空闲状态，则无需执行任何操作
        ESP_LOGI(TAG, "idle, nothing to do");
    } else if (state == AUDIO_PLAYER_STATE_PLAYING) {
        // 如果播放器正在播放，获取当前文件的索引并播放该文件
        int index = file_iterator_get_index(file_iterator);
        play_index(index);
    }
}

void music_backward(lv_event_t * e)
{
    // 将文件迭代器移动到上一个文件
    file_iterator_prev(file_iterator);

    // 获取音频播放器的状态
    audio_player_state_t state = audio_player_get_state();
    if (state == AUDIO_PLAYER_STATE_IDLE) {
        // 如果播放器处于空闲状态，则不需要做任何事情
        ESP_LOGI(TAG, "空闲状态，不需要做任何事情");
    } else if (state == AUDIO_PLAYER_STATE_PLAYING) {
        // 获取当前文件的索引
        int index = file_iterator_get_index(file_iterator);
        // 播放指定索引的文件
        play_index(index);
    }
}

void music_play(lv_event_t * e)
{
    // 获取音频播放器的状态
    audio_player_state_t state = audio_player_get_state();
    if (state == AUDIO_PLAYER_STATE_PLAYING) {
        /* 如果用户想要使用暂停/恢复功能，
           请将 'audio_player_stop()' 替换为 'audio_player_pause()' */
        audio_player_stop();
    } else if (state == AUDIO_PLAYER_STATE_IDLE) {
        // 获取当前文件的索引
        int index = file_iterator_get_index(file_iterator);
        // 播放指定索引的文件
        play_index(index);
    } else if (state == AUDIO_PLAYER_STATE_PAUSE) {
        // 恢复播放
        audio_player_resume();
    }
}

bool is_music_playing(void)
{
    // 检查音频播放器是否正在播放
    if (audio_player_get_state() == AUDIO_PLAYER_STATE_PLAYING) {
        return true;
    } else {
        return false;
    }
}
// 音乐播放暂停函数
void music_play_pause(void)
{
    // 如果音频播放器正在播放状态
    if (audio_player_get_state() == AUDIO_PLAYER_STATE_PLAYING) {
        /* 如果用户想要使用暂停/恢复功能，
           请将 'audio_player_stop()' 替换为 'audio_player_pause()' */
        audio_player_stop();
    }
}

// 音乐播放恢复函数
void music_play_resume(void)
{
    // 如果音频播放器处于空闲状态
    if (audio_player_get_state() == AUDIO_PLAYER_STATE_IDLE) {
        int index = file_iterator_get_index(file_iterator); // 获取文件索引
        play_index(index); // 播放指定索引的文件
    } 
    // 如果音频播放器处于暂停状态
    else if (audio_player_get_state() == AUDIO_PLAYER_STATE_PAUSE) {
        audio_player_resume(); // 恢复播放
    }
}

// 应用音乐播放器初始化函数
void app_music_player_init(void)
{
    file_iterator = file_iterator_new(BSP_SPIFFS_MOUNT_POINT); // 创建文件迭代器
    assert(file_iterator != NULL); // 确保文件迭代器不为空
}
