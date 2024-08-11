/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>

#include "esp_log.h"
#include "esp_check.h"

#include "driver/rmt_tx.h"
#include "led_strip.h"
#include "led_strip_interface.h"

#include "audio_player.h"
#include "bsp/esp-bsp.h"
#include "bsp_board_extra.h"

static const char *TAG = "bsp_extra_board";

#define BLINK_GPIO                         (GPIO_NUM_4)

#define CODEC_DEFAULT_SAMPLE_RATE          (16000)
#define CODEC_DEFAULT_BIT_WIDTH            (16)
#define CODEC_DEFAULT_ADC_VOLUME           (24.0)
#define CODEC_DEFAULT_CHANNEL              (2)
static esp_codec_dev_handle_t play_dev_handle; // 播放设备句柄
static esp_codec_dev_handle_t record_dev_handle; // 录音设备句柄

static file_iterator_instance_t *file_iterator; // 文件迭代器实例
static led_strip_handle_t led_strip; // LED灯带句柄

/* LED灯带初始化配置，包括GPIO和像素数量 */
led_strip_config_t strip_config = {
    .strip_gpio_num = BLINK_GPIO, // 连接到LED灯带数据线的GPIO
    .max_leds = 1, // LED灯带中的LED数量
    .led_pixel_format = LED_PIXEL_FORMAT_GRB, // LED灯带的像素格式
    .led_model = LED_MODEL_WS2812, // LED灯带型号
    .flags.invert_out = false, // 是否反转输出信号（当硬件有电平反相器时使用）
};

led_strip_rmt_config_t rmt_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT, // 不同的时钟源会导致不同的功耗
    .resolution_hz = 10 * 1000 * 1000, // 10MHz
    .flags.with_dma = false, // 是否启用DMA功能
};


esp_err_t bsp_extra_led_init()
{
    ESP_LOGI(TAG, "BLINK_GPIO setting %d", BLINK_GPIO);

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, 0x00, 0x00, 0x00));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
    return ESP_OK;
}

esp_err_t bsp_extra_led_set_rgb(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, index, red, green, blue));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
    return ESP_OK;
}
// 获取文件迭代器实例
file_iterator_instance_t *bsp_extra_get_file_instance(void)
{
    return file_iterator;
}

// 音频静音功能
static esp_err_t audio_mute_function(AUDIO_PLAYER_MUTE_SETTING setting)
{
    // 当静音时保存音量，当取消静音时恢复音量。恢复音量是必要的，
    // 因为 es8311_set_voice_mute(true) 会将声音音量（REG32）设置为零。

    bsp_extra_codec_mute_set(setting == AUDIO_PLAYER_MUTE ? true : false);

    // 取消静音时恢复声音音量
    if (setting == AUDIO_PLAYER_UNMUTE) {
        bsp_extra_codec_volume_set(80, NULL);
    }

    return ESP_OK;
}


static void audio_callback(audio_player_cb_ctx_t *ctx)
{
    switch (ctx->audio_event) {
    case AUDIO_PLAYER_CALLBACK_EVENT_IDLE: /**< Player is idle, not playing audio */
        ESP_LOGD(TAG, "IDLE");
        bsp_audio_poweramp_enable(false);
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_COMPLETED_PLAYING_NEXT:
        ESP_LOGD(TAG, "NEXT");
        bsp_audio_poweramp_enable(true);
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_PLAYING:
        ESP_LOGD(TAG, "PLAYING");
        bsp_audio_poweramp_enable(true);
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_PAUSE:
        ESP_LOGD(TAG, "PAUSE");
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_SHUTDOWN:
        ESP_LOGD(TAG, "SHUTDOWN");
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_UNKNOWN_FILE_TYPE:
        ESP_LOGD(TAG, "UNKNOWN FILE");
        break;
    case AUDIO_PLAYER_CALLBACK_EVENT_UNKNOWN:
        ESP_LOGD(TAG, "UNKNOWN");
        break;
    }
}

esp_err_t bsp_extra_i2s_read(void *audio_buffer, size_t len, size_t *bytes_read, uint32_t timeout_ms)
{
    esp_err_t ret = ESP_OK;
    ret = esp_codec_dev_read(record_dev_handle, audio_buffer, len);
    *bytes_read = len;
    return ret;
}

esp_err_t bsp_extra_i2s_write(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms)
{
    esp_err_t ret = ESP_OK;
    ret = esp_codec_dev_write(play_dev_handle, audio_buffer, len);
    *bytes_written = len;
    return ret;
}

esp_err_t bsp_extra_codec_set_fs(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t ch)
{
    esp_err_t ret = ESP_OK;

    esp_codec_dev_sample_info_t fs = {
        .sample_rate = rate,
        .channel = ch,
        .bits_per_sample = bits_cfg,
    };

    if (play_dev_handle) {
        ret = esp_codec_dev_close(play_dev_handle);
    }

    if (play_dev_handle) {
        ret |= esp_codec_dev_open(play_dev_handle, &fs);
    }

    return ret;
}

esp_err_t bsp_extra_codec_volume_set(int volume, int *volume_set)
{
    esp_err_t ret = ESP_OK;
    float v = volume;
    ret = esp_codec_dev_set_out_vol(play_dev_handle, (int)v);
    return ret;
}

esp_err_t bsp_extra_codec_mute_set(bool enable)
{
    esp_err_t ret = ESP_OK;
    ret = esp_codec_dev_set_out_mute(play_dev_handle, enable);
    return ret;
}

esp_err_t bsp_extra_codec_dev_stop(void)
{
    esp_err_t ret = ESP_OK;

    if (play_dev_handle) {
        ret = esp_codec_dev_close(play_dev_handle);
    }

    if (record_dev_handle) {
        ret = esp_codec_dev_close(record_dev_handle);
    }
    return ret;
}

esp_err_t bsp_extra_codec_dev_resume(void)
{
    return bsp_extra_codec_set_fs(CODEC_DEFAULT_SAMPLE_RATE, CODEC_DEFAULT_BIT_WIDTH, CODEC_DEFAULT_CHANNEL);
}

esp_err_t bsp_extra_codec_init()
{
    play_dev_handle = bsp_audio_codec_speaker_init();
    assert((play_dev_handle) && "play_dev_handle not initialized");

    record_dev_handle = bsp_audio_codec_microphone_init();
    assert((record_dev_handle) && "record_dev_handle not initialized");

    bsp_extra_codec_set_fs(CODEC_DEFAULT_SAMPLE_RATE, CODEC_DEFAULT_BIT_WIDTH, CODEC_DEFAULT_CHANNEL);

    esp_codec_dev_sample_info_t fs_record = {
        .sample_rate = CODEC_DEFAULT_SAMPLE_RATE,
        .channel = CODEC_DEFAULT_CHANNEL,
        .bits_per_sample = CODEC_DEFAULT_BIT_WIDTH,
    };

    if (record_dev_handle) {
        esp_codec_dev_close(record_dev_handle);
        esp_codec_dev_set_in_gain(record_dev_handle, CODEC_DEFAULT_ADC_VOLUME);
    }

    if (record_dev_handle) {
        esp_codec_dev_open(record_dev_handle, &fs_record);
    }

    return ESP_OK;
}

esp_err_t bsp_extra_player_init(char *path)
{
    // 检查路径是否为空
    if (path) {
        // 创建文件迭代器
        file_iterator = file_iterator_new(path);
        // 检查文件迭代器是否创建成功
        ESP_ERROR_CHECK(file_iterator == NULL ? ESP_FAIL : ESP_OK);
    }

    // 配置音频播放器
    audio_player_config_t config = { 
        .mute_fn = audio_mute_function, // 静音函数
        .write_fn = bsp_extra_i2s_write, // 写函数
        .clk_set_fn = bsp_extra_codec_set_fs, // 时钟设置函数
        .priority = 4 // 优先级
    };
    // 创建音频播放器
    ESP_ERROR_CHECK(audio_player_new(config));
    // 注册音频回调函数
    audio_player_callback_register(audio_callback, NULL);

    return ESP_OK;
}

