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
/* 
 * 用于UI控制的功能相关代码
 */
/* 定义一个静态字符串，表示连接到Wi-Fi */
#define LABEL_WIFI_TEXT                 "Connecting to Wi-Fi\n"
/* 定义一个静态字符串，表示没有连接到Wi-Fi */
#define LABEL_NOT_WIFI_TEXT                 "Not Connected to Wi-Fi\n"
/* 定义一个常量，表示Wi-Fi的小点最大的数量 */
#define LABEL_WIFI_DOT_COUNT_MAX        (10)
/* 定义一个常量，表示检查Wi-Fi的间隔时间，单位为秒 */
#define WIFI_CHECK_TIMER_INTERVAL_S     (1)
/* 定义一个常量，表示回复滚动的定时器的间隔时间，单位为毫秒 */
#define REPLY_SCROLL_TIMER_INTERVAL_MS  (1000)
/* 定义一个常量，表示回复滚动的速度 */
#define REPLY_SCROLL_SPEED              (1)
/*
 * 控制用户界面的功能实现
 */

static char *TAG = "ui_ctrl";  // 日志标签

// 当前面板的状态
static ui_ctrl_panel_t current_panel = UI_CTRL_PANEL_SLEEP;

// 滚动定时器句柄
static lv_timer_t *scroll_timer_handle = NULL;

// 回复音频开始标志
static bool reply_audio_start = false;

// 回复音频结束标志
static bool reply_audio_end = false;

// 回复内容获取标志
static bool reply_content_get = false;

// 内容高度
static uint16_t content_height = 0;

// 回复内容滚动定时器处理函数
static void reply_content_scroll_timer_handler();

// WiFi检查定时器处理函数
static void wifi_check_timer_handler(lv_timer_t *timer);
/**
 * @brief 初始化UI控制模块
 * 
 */
void ui_ctrl_init(void)
{
    bsp_display_lock(0); // 解锁显示屏

    ui_init(); // 初始化UI界面

    scroll_timer_handle = lv_timer_create(reply_content_scroll_timer_handler, REPLY_SCROLL_TIMER_INTERVAL_MS / REPLY_SCROLL_SPEED, NULL); // 创建滚动定时器
    lv_timer_pause(scroll_timer_handle); // 暂停滚动定时器

    lv_timer_create(wifi_check_timer_handler, WIFI_CHECK_TIMER_INTERVAL_S * 1000, NULL); // 创建WiFi检测定时器

    bsp_display_unlock(); // 解锁显示屏
}

/**
 * @brief WiFi检测定时器处理函数
 * 
 * @param timer 定时器指针
 */
static void wifi_check_timer_handler(lv_timer_t *timer)
{
    if (WIFI_STATUS_CONNECTED_OK == wifi_connected_already()) { // 如果WiFi连接成功
        lv_obj_clear_flag(ui_PanelSetupSteps, LV_OBJ_FLAG_HIDDEN); // 显示设置步骤面板
        lv_obj_add_flag(ui_PanelSetupWifi, LV_OBJ_FLAG_HIDDEN); // 隐藏WiFi设置面板
        lv_timer_del(timer); // 删除定时器
        if (ui_get_btn_op_group()) { // 如果存在按钮操作组
            lv_group_remove_all_objs(ui_get_btn_op_group()); // 移除所有对象
            lv_group_add_obj(ui_get_btn_op_group(), ui_ButtonSetup); // 添加设置按钮到操作组
        }
    } else if (WIFI_STATUS_CONNECTED_FAILED == wifi_connected_already()) { // 如果WiFi连接失败
        lv_label_set_text(ui_LabelSetupWifi, LABEL_NOT_WIFI_TEXT); // 设置WiFi标签文本为连接失败提示
    } else {
        if (strlen(lv_label_get_text(ui_LabelSetupWifi)) >= sizeof(LABEL_WIFI_TEXT) + LABEL_WIFI_DOT_COUNT_MAX + 1) {
            lv_label_set_text(ui_LabelSetupWifi, LABEL_WIFI_TEXT); // 设置WiFi标签文本为常规WiFi连接提示
        } else {
            lv_label_ins_text(ui_LabelSetupWifi, LV_LABEL_POS_LAST, "."); // 在WiFi标签文本末尾插入一个点
        }
    }
}
/*
 * 用于展示控制面板的定时器处理函数
 */
static void show_panel_timer_handler(struct _lv_timer_t *t)
{
    // 将用户数据类型转换为控制面板类型
    ui_ctrl_panel_t panel = (ui_ctrl_panel_t)t->user_data;
    lv_obj_t *show_panel = NULL;
    lv_obj_t *hide_panel[3] = { NULL };

    // 根据面板类型进行相应操作
    switch (panel) {
    case UI_CTRL_PANEL_SLEEP:
        show_panel = ui_PanelSleep;
        hide_panel[0] = ui_PanelListen;
        hide_panel[1] = ui_PanelGet;
        hide_panel[2] = ui_PanelReply;
        lv_obj_clear_flag(ui_ImageListenSettings, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(ui_LabelListenSpeak, " ");
        break;
    case UI_CTRL_PANEL_LISTEN:
        show_panel = ui_PanelListen;
        hide_panel[0] = ui_PanelSleep;
        hide_panel[1] = ui_PanelGet;
        hide_panel[2] = ui_PanelReply;
        lv_obj_clear_flag(ui_LabelListenSpeak, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(ui_LabelListenSpeak, "Listening ...");
        // 重置回复的标志和定时器
        reply_content_get = false;
        reply_audio_start = false;
        reply_audio_end = false;
        lv_timer_pause(scroll_timer_handle);
        break;
    case UI_CTRL_PANEL_GET:
        show_panel = ui_PanelGet;
        hide_panel[0] = ui_PanelSleep;
        hide_panel[1] = ui_PanelListen;
        hide_panel[2] = ui_PanelReply;
        lv_obj_clear_flag(ui_LabelListenSpeak, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(ui_LabelListenSpeak, "Thinking ...");
        break;
    case UI_CTRL_PANEL_REPLY:
        show_panel = ui_PanelReply;
        hide_panel[0] = ui_PanelSleep;
        hide_panel[1] = ui_PanelListen;
        hide_panel[2] = ui_PanelGet;
        lv_obj_add_flag(ui_ImageListenSettings, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_ImageListenSettings, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_LabelListenSpeak, LV_OBJ_FLAG_HIDDEN);
        break;
    default:
        break;
    }

    // 若面板类型不是回复，则清除隐藏设置图标
    if (panel != UI_CTRL_PANEL_REPLY) {
        lv_obj_clear_flag(ui_ImageListenSettings, LV_OBJ_FLAG_HIDDEN);
    }

    // 清除展示面板的隐藏标志
    lv_obj_clear_flag(show_panel, LV_OBJ_FLAG_HIDDEN);
    // 隐藏其他面板
    for (int i = 0; i < sizeof(hide_panel) / sizeof(lv_obj_t *); i++) {
        lv_obj_add_flag(hide_panel[i], LV_OBJ_FLAG_HIDDEN);
    }

    // 设置当前面板为当前处理的面板
    current_panel = panel;

    ESP_LOGI(TAG, "Swich to panel[%d]", panel);
}
// 在此函数中显示指定的面板并设置超时时间
void ui_ctrl_show_panel(ui_ctrl_panel_t panel, uint16_t timeout)
{
    bsp_display_lock(0);

    if (timeout) {
        // 如果有超时时间，则创建定时器并设置面板切换函数作为回调
        lv_timer_t *timer = lv_timer_create(show_panel_timer_handler, timeout, NULL);
        timer->user_data = (void *)panel;
        lv_timer_set_repeat_count(timer, 1);
        ESP_LOGW(TAG, "Switch panel to [%d] in %dms", panel, timeout);
    } else {
        // 如果没有超时时间，则直接调用面板切换函数
        lv_timer_t timer;
        timer.user_data = (void *)panel;
        show_panel_timer_handler(&timer);
    }

    bsp_display_unlock();
}

// 在 UI 中显示文本内容
static void reply_content_show_text(const char *text)
{
    if (NULL == text) {
        return;
    }

    // 解码文本中的转义字符
    char *decode = heap_caps_malloc((strlen(text) + 1), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(decode);

    int j = 0;
    for (int i = 0; i < strlen(text);) {
        if ((*(text + i) == '\\') && ((i + 1) < strlen(text)) && (*(text + i + 1) == 'n')) {
            // 将转义字符 '\n' 转换为换行符
            *(decode + j++) = '\n';
            i += 2;
        } else {
            *(decode + j++) = *(text + i);
            i += 1;
        }
    }
    *(decode + j) = '\0';

    ESP_LOGI(TAG, "decode:[%d, %d] %s\r\n", j, strlen(decode), decode);

    // 设置 UI 中的文本内容
    lv_label_set_text(ui_LabelReplyContent, decode);
    content_height = lv_obj_get_self_height(ui_LabelReplyContent);
    lv_obj_scroll_to_y(ui_ContainerReplyContent, 0, LV_ANIM_OFF);
    reply_content_get = true;
    lv_timer_resume(scroll_timer_handle);
    ESP_LOGI(TAG, "reply scroll timer start");

    if (decode) {
        free(decode);
    }
}
// 在UI控件标签上显示文本
void ui_ctrl_label_show_text(ui_ctrl_label_t label, const char *text)
{
    // 锁定显示屏以防止并发访问
    bsp_display_lock(0);

    // 如果文本不为空
    if (text != NULL) {
        // 根据标签类型执行相应操作
        switch (label) {
        // 更新听说标签
        case UI_CTRL_LABEL_LISTEN_SPEAK:
            ESP_LOGI(TAG, "update listen speak");
            lv_label_set_text(ui_LabelListenSpeak, text);
            break;
        // 更新回答问题标签
        case UI_CTRL_LABEL_REPLY_QUESTION:
            ESP_LOGI(TAG, "update reply question");
            lv_label_set_text(ui_LabelReplyQuestion, text);
            break;
        // 显示回复内容
        case UI_CTRL_LABEL_REPLY_CONTENT:
            ESP_LOGI(TAG, "update reply content");
            reply_content_show_text(text);
            break;
        default:
            break;
        }
    }

    // 解锁显示屏
    bsp_display_unlock();
}

// 设置背景图像不透明度的动画回调函数
static void anim_callback_set_bg_img_opacity(lv_anim_t *a, int32_t v)
{
    ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
    lv_obj_set_style_bg_img_opa(usr->target, v, 0);
}

// 获取不透明度的动画回调函数
static int32_t anim_callback_get_opacity(lv_anim_t *a)
{
    ui_anim_user_data_t *usr = (ui_anim_user_data_t *)a->user_data;
    return lv_obj_get_style_bg_img_opa(usr->target, 0);
}
/*
 * 在UI界面上显示动画
 */
void ui_sleep_show_animation(void)
{
    bsp_display_lock(0);  // 锁定显示屏

    // 创建并初始化第一个动画对象PropertyAnimation_0
    ui_anim_user_data_t *PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = ui_ContainerBigZ;
    PropertyAnimation_0_user_data->val = -1;
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 1000);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, anim_callback_set_bg_img_opacity );
    lv_anim_set_values(&PropertyAnimation_0, 0, 255 );
    lv_anim_set_path_cb( &PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay( &PropertyAnimation_0, 0 );
    lv_anim_set_playback_time(&PropertyAnimation_0, 1000);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, LV_ANIM_REPEAT_INFINITE );
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 1000);
    lv_anim_set_early_apply( &PropertyAnimation_0, false );
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &anim_callback_get_opacity );
    lv_anim_start(&PropertyAnimation_0);  // 启动动画

    // 创建并初始化第二个动画对象PropertyAnimation_1
    ui_anim_user_data_t *PropertyAnimation_1_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_1_user_data->target = ui_ContainerSmallZ;
    PropertyAnimation_1_user_data->val = -1;
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 1000);
    lv_anim_set_user_data(&PropertyAnimation_1, PropertyAnimation_1_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, anim_callback_set_bg_img_opacity );
    lv_anim_set_values(&PropertyAnimation_1, 0, 255 );
    lv_anim_set_path_cb( &PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay( &PropertyAnimation_1, 1000 );
    lv_anim_set_playback_time(&PropertyAnimation_1, 1000);
    lv_anim_set_playback_delay(&PropertyAnimation_1, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_1, LV_ANIM_REPEAT_INFINITE );
    lv_anim_set_repeat_delay(&PropertyAnimation_1, 1000);
    lv_anim_set_early_apply( &PropertyAnimation_1, false );
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &anim_callback_get_opacity );
    lv_anim_start(&PropertyAnimation_1);  // 启动动画

    bsp_display_unlock();  // 解锁显示屏
}
// 设置回复音频开始标志的函数
void ui_ctrl_reply_set_audio_start_flag(bool result)
{
    reply_audio_start = result;
}

// 获取回复音频开始标志的函数
bool ui_ctrl_reply_get_audio_start_flag(void)
{
    return reply_audio_start;
}

// 设置回复音频结束标志的函数
void ui_ctrl_reply_set_audio_end_flag(bool result)
{
    reply_audio_end = result;
}
// 声明静态函数用于处理回复内容的滚动定时器
static void reply_content_scroll_timer_handler()
{
    lv_coord_t offset = 0;  // 初始化滚动偏移量为0
    const lv_font_t *font = NULL;  // 声明字体指针并初始化为NULL

    // 当滚动内容获取并且音频开始播放时执行以下操作
    if (reply_content_get && reply_audio_start) {
        font = lv_obj_get_style_text_font(ui_LabelReplyContent, 0);  // 获取回复内容标签的字体
        offset = lv_obj_get_scroll_y(ui_ContainerReplyContent);  // 获取容器的垂直滚动偏移量
        // ESP_LOGI(TAG, "offset: %d, content_height: %d, font_height: %d", offset, content_height, font->line_height);
        // 如果内容高度超过容器高度且未到达底部，则向下滚动一半行高
        if ((content_height > lv_obj_get_height(ui_ContainerReplyContent)) &&
                (offset < (content_height - lv_obj_get_height(ui_ContainerReplyContent)))) {
            offset += font->line_height / 2;  // 增加滚动偏移量
            lv_obj_scroll_to_y(ui_ContainerReplyContent, offset, LV_ANIM_OFF);  // 垂直滚动到指定偏移量
        } 
        // 若音频播放结束，则停止滚动定时器，展示睡眠面板，并重置相关标记
        else if (reply_audio_end) {
            ESP_LOGI(TAG, "reply scroll timer stop");
            reply_content_get = false;
            reply_audio_start = false;
            reply_audio_end = false;
            lv_timer_pause(scroll_timer_handle);  // 暂停滚动定时器
            ui_ctrl_show_panel(UI_CTRL_PANEL_SLEEP, 1000);  // 展示睡眠面板
        }
    }
}

// 定义处理UI控制引导跳转的函数
void ui_ctrl_guide_jump( void )
{
    lv_obj_t *act_scr = lv_scr_act();  // 获取当前活动的屏幕

    // 若当前活动的屏幕是设置界面，则发送模拟点击设置按钮的事件
    if (act_scr == ui_ScreenSetup) {
        ESP_LOGI(TAG, "act_scr:%p, ui_ScreenSetup:%p", act_scr, ui_ScreenSetup);
        lv_event_send(ui_ButtonSetup, LV_EVENT_CLICKED, 0);  // 发送点击事件
    }
}
