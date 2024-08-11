/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "esp_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char *TAG = "app_sntp";

/* 变量保存自ESP32首次启动以来的重启次数。
 * 它使用RTC_DATA_ATTR放置在RTC内存中，
 * 并且在ESP32从深度睡眠中唤醒时保持其值。
 */
RTC_DATA_ATTR static int boot_count = 0;

// 时间同步通知的回调函数
static void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "时间同步事件的通知, 秒数=%lld", (long long int)tv->tv_sec);
    settimeofday(tv, NULL);
}
// 初始化SNTP的函数
static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    // 设置SNTP操作模式
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    // 设置SNTP服务器地址
    esp_sntp_setservername(0, "ntp.aliyun.com");
    esp_sntp_setservername(1, "time.asia.apple.com");
    esp_sntp_setservername(2, "pool.ntp.org");
    // 设置时间同步通知回调函数
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    // 初始化SNTP
    esp_sntp_init();
}

// 从SNTP服务器获取时间的函数
static void obtain_time(void)
{
    // 初始化SNTP
    initialize_sntp();

    // 等待时间设置
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    // 获取当前时间
    time(&now);
    // 将时间转换为本地时间
    localtime_r(&now, &timeinfo);
}



void app_sntp_init(void)
{
    ++boot_count; // 增加启动次数计数器
    ESP_LOGI(TAG, "Boot count: %d", boot_count); // 打印启动次数

    time_t now; // 定义时间变量
    struct tm timeinfo; // 定义时间结构体
    time(&now); // 获取当前时间
    localtime_r(&now, &timeinfo); // 将时间转换为本地时间
    // 设置时区为中国标准时间
    setenv("TZ", "CST-8", 1);
    tzset();
    // 检查时间是否已设置，如果未设置，tm_year 将会是 (1970 - 1900)
    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP."); // 打印时间未设置的日志
        obtain_time(); // 通过 NTP 获取时间
        // 更新 'now' 变量为当前时间
        time(&now);
    }

    char strftime_buf[64]; // 定义时间字符串缓冲区
    localtime_r(&now, &timeinfo); // 将时间转换为本地时间
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo); // 格式化时间为字符串
    ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf); // 打印当前上海的日期和时间
}




