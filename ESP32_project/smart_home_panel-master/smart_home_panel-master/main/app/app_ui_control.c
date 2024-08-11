/* SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "http_get_weather.h"
#include "ui.h"
#include "app_ui_control.h"
#define IMG_PATH "//image"  // 定义图片路径

static const char *TAG = "app_ui_control";  // 定义日志标签

static void ui_clock_update(lv_timer_t *timer)  // 更新时钟显示的函数
{
    struct timeval tv;
    gettimeofday(&tv, NULL);  // 获取当前时间

    struct tm* timeinfo;
    timeinfo = localtime(&tv.tv_sec);  // 将时间转换为本地时间

    char time_str[9];
    sprintf(time_str, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);  // 格式化时间字符串

    char date_str[36];
    sprintf(date_str, "%04d/%02d/%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);  // 格式化日期字符串

    lv_label_set_text_fmt(ui_LabelTime, "%s", time_str);  // 更新时间标签的文本
    lv_label_set_text_fmt(ui_LabelDate, "%s", date_str);  // 更新日期标签的文本
}

static void ui_calendar_update(lv_timer_t *timer)  // 更新日历显示的函数
{
    struct timeval tv;
    gettimeofday(&tv, NULL);  // 获取当前时间

    struct tm* timeinfo;
    timeinfo = localtime(&tv.tv_sec);  // 将时间转换为本地时间

    lv_calendar_set_showed_date(ui_Calendar, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1);  // 设置日历显示的日期
    lv_calendar_set_today_date(ui_Calendar, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);  // 设置日历中的今天日期
}

static void ui_weather_update(lv_timer_t *timer)  // 更新天气显示的函数
{
    /* Allocate weather data from heap */
    weather_data_t *weather_data = http_weather_data_create();  // 从堆中分配天气数据
    /* Perform an HTTP GET request with the weather data */
    ESP_ERROR_CHECK(http_get_with_url(HTTP_GET_URL_ADDRESS, weather_data));  // 执行HTTP GET请求获取天气数据

    lv_label_set_text(ui_Label2, weather_data->week);  // 更新星期标签的文本
    lv_label_set_text(ui_Label4, weather_data->text);  // 更新天气描述标签的文本

    /* Release weather data resource */
    ESP_ERROR_CHECK(http_weather_data_delete(weather_data));  // 释放天气数据资源
}

void ui_timer_init(void)  // 初始化定时器的函数
{
    lv_timer_t *timer_clock = lv_timer_create(ui_clock_update, 1000, NULL);  // 创建时钟更新定时器，每秒更新一次
    ui_clock_update(timer_clock);  // 立即更新时钟显示

    /* Update calendar every 12 hours */
    lv_timer_t *timer_calendar = lv_timer_create(ui_calendar_update, 12 * 60 * 60 * 1000, NULL);  // 创建日历更新定时器，每12小时更新一次
    ui_calendar_update(timer_calendar);  // 立即更新日历显示

    /* Update weather every 1 hour */
    lv_timer_t *timer_weather = lv_timer_create(ui_weather_update, 60 * 60 * 1000, NULL);  // 创建天气更新定时器，每小时更新一次
    ui_weather_update(timer_weather);  // 立即更新天气显示
}
