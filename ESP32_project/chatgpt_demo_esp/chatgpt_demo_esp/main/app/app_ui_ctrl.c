/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "esp_log.h"

#include "app_ui_ctrl.h"
#include "app_wifi.h"
#include "bsp/esp-bsp.h"
#include "ui_helpers.h"
#include "ui.h"
#include "settings.h"
#include "app_weather.h"
#include <time.h>
#include <sys/time.h>
#include "audio_player.h"
static const char *TAG = "APP_UI_CTRL";
int weather_flag = 0;
static void ui_clock_update(lv_timer_t *timer)  // 更新时钟显示的函数
{
    struct timeval tv;
    gettimeofday(&tv, NULL);  // 获取当前时间

    struct tm* timeinfo;
    timeinfo = localtime(&tv.tv_sec);  // 将时间转换为本地时间

    char time_str[9];
    sprintf(time_str, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);  // 格式化时间字符串


    lv_label_set_text_fmt(ui_TimeLable, "%s", time_str);  // 更新时间标签的文本
    
}

static void ui_calendar_update(lv_timer_t *timer)  // 更新日历显示的函数
{
    struct timeval tv;
    gettimeofday(&tv, NULL);  // 获取当前时间

    struct tm* timeinfo;
    timeinfo = localtime(&tv.tv_sec);  // 将时间转换为本地时间

    char date_str[36];
    sprintf(date_str, "%04d/%02d/%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);  // 格式化日期字符串
    lv_label_set_text_fmt(ui_DataLabel, "%s", date_str);  // 更新日期标签的文本

    lv_calendar_set_showed_date(ui_Calendar1, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1);  // 设置日历显示的日期
    lv_calendar_set_today_date(ui_Calendar1, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);  // 设置日历中的今天日期
}

static void ui_weather_update(lv_timer_t *timer)  // 更新天气显示的函数
{
    if(weather_flag == 0)
    {
        /* Allocate weather data from heap */
    weather_data_t *weather_data = http_weather_data_create();  // 从堆中分配天气数据
    /* Perform an HTTP GET request with the weather data */
    sys_param_t *sys_param = settings_get_parameter();  // 获取系统参数
    ESP_ERROR_CHECK(http_get_with_url(sys_param->weather_url, weather_data));  // 执行HTTP GET请求获取天气数据


    lv_label_set_text(ui_Label23, weather_data->city);// 更新城市标签的文本
    lv_label_set_text(ui_LabelText, weather_data->text);// 更新天气标题标签的文本

    char high_low_str[11];
    sprintf(high_low_str, "%d°/%d°", *(weather_data->low), *(weather_data->high));
    char wind_str[24];
    sprintf(wind_str, "%s%s", weather_data->wind_dir, weather_data->wind_class);
    char rh_str[11];
    sprintf(rh_str, "%d%%", *(weather_data->rh));
    char temp_str[8];
    sprintf(temp_str, "%d°", *(weather_data->temp));

    ESP_ERROR_CHECK(http_weather_data_delete(weather_data));  // 释放天气数据资源

    lv_label_set_text(ui_LabelWind, wind_str);// 更新风向风力标签的文本
    lv_label_set_text(ui_LabelTemp,temp_str);//温度
    lv_label_set_text(ui_Labelhighlow,high_low_str);
    lv_label_set_text(ui_Labelrh, rh_str);
    // if (WIFI_STATUS_CONNECTED_OK == wifi_connected_already()) {
    //     lv_label_set_text(ui_Label22WIFI, "重大灾难:地震!!!");
    //     FILE *fp = fopen("/spiffs/freecompre.mp3", "r");
    //                     if (fp) {
    //                     audio_player_play(fp); // 播放等待提示音
    // //                     }
    // }
//    lv_label_set_text(ui_Label3, weather_data->title);//用不到ba 
    /* Release weather data resource */
    }

    
    
}

void ui_timer_init(void)  // 初始化定时器的函数
{

    /* Update calendar every 12 hours */
    lv_timer_t *timer_calendar = lv_timer_create(ui_calendar_update,  30 * 1000, NULL);  // 创建日历更新定时器，每12小时更新一次
    ui_calendar_update(timer_calendar);  // 立即更新日历显示

    lv_timer_t *timer_clock = lv_timer_create(ui_clock_update, 1000, NULL);  // 创建时钟更新定时器，每秒更新一次
    ui_clock_update(timer_clock);  // 立即更新时钟显示

    /* Update weather every 1 hour */
    lv_timer_t *timer_weather = lv_timer_create(ui_weather_update, 30 * 60 * 1000, NULL);  // 创建天气更新定时器，30分钟更新一次
    ui_weather_update(timer_weather);  // 立即更新天气显示
}


static void wifi_check_timer_handler(lv_timer_t *timer)
{
    // 检查WiFi连接状态
    if (WIFI_STATUS_CONNECTED_OK == wifi_connected_already()) {//连接成功
        lv_label_set_text(ui_Label15, "连接成功\n");
        //Label22WIFI
        lv_label_set_text(ui_Label22WIFI, "");
        lv_label_set_text(ui_Label36, "");
        ui_timer_init();//初始化定时器
        // 删除定时器
        lv_timer_del(timer);
    } else if (WIFI_STATUS_CONNECTED_FAILED == wifi_connected_already()) {//连接失败
        // 如果WiFi连接失败，设置标签文本为“未连接WiFi”
        lv_label_set_text(ui_Label15, "连接失败\n");
        lv_label_set_text(ui_Label22WIFI, "wiFi未连接");
        lv_label_set_text(ui_Label36, "");
    } else {//正在连接中的......
        // 如果正在连接中，设置标签文本为“正在连接中”
        lv_label_set_text(ui_Label15, "WiFi连接中\n");
    }
}

// 初始化UI控制
void ui_ctrl_init(void)
{
    // 锁定显示设备
    bsp_display_lock(0);

    // 初始化UI
    ui_init();
    
    // 创建WiFi检查定时器，设置定时器间隔和处理函数
    lv_timer_create(wifi_check_timer_handler, 1 * 1000, NULL);

    // 解锁显示设备
    bsp_display_unlock();
}
//这个函数是一个事件回调函数，当用户选择下拉菜单中的某个选项时，会触发这个回调函数。函数中获取选中的字符串并打印出来。
// static void event_cb(lv_event_t * e)
// {
//     lv_obj_t * dropdown = lv_event_get_target(e);
//     char buf[64];
//     lv_dropdown_get_selected_str(dropdown, buf, sizeof(buf));
//     LV_LOG_USER("'%s' is selected", buf);
// }
//这行代码为下拉菜单添加了事件回调函数，当用户选择某个选项时，会调用event_cb函数
//    lv_obj_add_event_cb(dropdown, event_cb, LV_EVENT_VALUE_CHANGED, NULL);



