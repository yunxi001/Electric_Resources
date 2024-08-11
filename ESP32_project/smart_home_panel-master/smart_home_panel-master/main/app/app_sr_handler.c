/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_check.h"
#include "app_sr.h"
#include "app_sr_handler.h"
#include "esp_afe_sr_iface.h"
#include "app_mqtt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "app_play_music.h"
#include "ui/ui.h"

static const char *TAG = "app_sr_handler";
void sr_handler_task(void *pvParam)
{
    QueueHandle_t xQueue = (QueueHandle_t) pvParam; // 获取队列句柄

    while (true) {
        sr_result_t result;
        xQueueReceive(xQueue, &result, portMAX_DELAY); // 从队列中接收数据，阻塞等待

        ESP_LOGI(TAG, "cmd:%d, wakemode:%d,state:%d", result.command_id, result.wakenet_mode, result.state); // 打印日志，显示命令ID、唤醒模式和状态

        if (ESP_MN_STATE_TIMEOUT == result.state) { // 如果状态是超时
            ESP_LOGI(TAG, "timeout"); // 打印超时日志
            lv_obj_clear_flag(ui_cat_gif, LV_OBJ_FLAG_HIDDEN); // 显示ui_cat_gif
            lv_obj_add_flag(ui_Image1, LV_OBJ_FLAG_HIDDEN); // 隐藏ui_Image1
            lv_obj_add_flag(ui_Labelwakenet, LV_OBJ_FLAG_HIDDEN); // 隐藏ui_Labelwakenet
            continue; // 继续下一次循环
        }

        if (WAKENET_DETECTED == result.wakenet_mode) { // 如果唤醒模式是检测到唤醒词
            ESP_LOGI(TAG, "wakenet detected"); // 打印唤醒词检测日志

            /* 修改UI */
            lv_obj_add_flag(ui_cat_gif, LV_OBJ_FLAG_HIDDEN); // 隐藏ui_cat_gif
            lv_obj_clear_flag(ui_Image1, LV_OBJ_FLAG_HIDDEN); // 显示ui_Image1
            lv_label_set_text(ui_Labelwakenet, "请讲！"); // 设置ui_Labelwakenet的文本为“请讲！”
            lv_obj_clear_flag(ui_Labelwakenet, LV_OBJ_FLAG_HIDDEN); // 显示ui_Labelwakenet
            _ui_screen_change(&ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Screen4_screen_init); // 切换到屏幕4，使用淡入动画
            continue; // 继续下一次循环
        }


        if (ESP_MN_STATE_DETECTED & result.state) {
            ESP_LOGI(TAG, "mn detected");

            switch (result.command_id) {
                case 0:
                    ESP_LOGI(TAG, "Turn on the air purifier");
                    lv_label_set_text(ui_Labelwakenet, "已打开");
                    mqtt_air_purifer_on();
                    break;
                case 1:
                    ESP_LOGI(TAG, "Turn off the air purifier");
                    mqtt_air_purifer_off();
                    lv_label_set_text(ui_Labelwakenet, "已关闭");
                    break;
                case 2:
                    ESP_LOGI(TAG, "Turn On the Lamp");
                    mqtt_lamp_on();
                    lv_label_set_text(ui_Labelwakenet, "已打开");
                    break;
                case 3:
                    ESP_LOGI(TAG, "Turn Off the Lamp");
                    mqtt_lampr_off();
                    lv_label_set_text(ui_Labelwakenet, "已关闭");
                    break;
                case 4:
                    ESP_LOGI(TAG, "Turn Lamp Brighter");
                    mqtt_lamp_brighter();
                    lv_label_set_text(ui_Labelwakenet, "好的！");
                    break;
                case 5:
                    ESP_LOGI(TAG, "Turn Lamp Dimmer");
                    mqtt_lampr_dimmer();
                    lv_label_set_text(ui_Labelwakenet, "好的！");
                    break;
                case 6:
                    ESP_LOGI(TAG, "Turn on the LED Strip");
                    mqtt_led_on();
                    lv_label_set_text(ui_Labelwakenet, "已打开");
                    break;
                case 7:
                    ESP_LOGI(TAG, "Turn off the LED Strip");
                    mqtt_led_off();
                    lv_label_set_text(ui_Labelwakenet, "已关闭");
                    break;
                case 8:
                    ESP_LOGI(TAG, "Play Music");
                    lv_label_set_text(ui_Labelwakenet, "好的！");
                    _ui_screen_change(&ui_Screen4, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Screen4_screen_init);
                    music_play_resume();
                    break;
                case 9:
                    ESP_LOGI(TAG, "Stop Music");
                    lv_label_set_text(ui_Labelwakenet, "好的！");
                    _ui_screen_change(&ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Screen4_screen_init);
                    music_play_pause();
                    break;
                case 10:
                    ESP_LOGI(TAG, "Show Time");
                    lv_label_set_text(ui_Labelwakenet, "好的！");
                    _ui_screen_change(&ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Screen1_screen_init);
                    break;
                case 11:
                    ESP_LOGI(TAG, "Show Calander");
                    lv_label_set_text(ui_Labelwakenet, "好的！");
                    _ui_screen_change(&ui_Screen3, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Screen1_screen_init);
                    break;
                default:
                    break;
            }
            /* **************** REGISTER COMMAND CALLBACK HERE **************** */
        }
    }

    vTaskDelete(NULL);
}