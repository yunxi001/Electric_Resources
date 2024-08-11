// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.1
// LVGL version: 8.3.11
// Project name: esp_box3

#include "ui.h"
#include "ui_helpers.h"
#include "app_ui_events.h"
#include "bsp_board.h"
#include "bsp/esp-bsp.h"
#include "esp_log.h"
static const char *TAG = "UI";
///////////////////// VARIABLES ////////////////////
void Keyboardgo_Animation(lv_obj_t * TargetObject, int delay);
void AIChatGO_Animation(lv_obj_t * TargetObject, int delay);
void Keyboardout_Animation(lv_obj_t * TargetObject, int delay);
void Keyboardgo1_Animation(lv_obj_t * TargetObject, int delay);


// SCREEN: ui_AIChat
void ui_AIChat_screen_init(void);
void ui_event_AIChat(lv_event_t * e);
lv_obj_t * ui_AIChat;
lv_obj_t * ui_Label1;
lv_obj_t * ui_TimeLable;
lv_obj_t * ui_DataLabel;
lv_obj_t * ui_Label22WIFI;
lv_obj_t * ui_PanelWIFIhiden;
lv_obj_t * ui_Label36;
lv_obj_t * ui_Bar2;
lv_obj_t * ui_Label35;
lv_obj_t * ui_cat_gif;

// SCREEN: ui_Menu
void ui_Menu_screen_init(void);
void ui_event_Menu(lv_event_t * e);
lv_obj_t * ui_Menu;
lv_obj_t * ui_Label12;
lv_obj_t * ui_Label13;
lv_obj_t * ui_Label2;
lv_obj_t * ui_Label3;
lv_obj_t * ui_Label4;
lv_obj_t * ui_Label5;
void ui_event_Panel1(lv_event_t * e);
lv_obj_t * ui_Panel1;
lv_obj_t * ui_Label7;
void ui_event_Panel2(lv_event_t * e);
lv_obj_t * ui_Panel2;
lv_obj_t * ui_Label11;
void ui_event_Panel3(lv_event_t * e);
lv_obj_t * ui_Panel3;
lv_obj_t * ui_Label6;
void ui_event_Panel4(lv_event_t * e);
lv_obj_t * ui_Panel4;
lv_obj_t * ui_Label8;
void ui_event_Panel5(lv_event_t * e);
lv_obj_t * ui_Panel5;
lv_obj_t * ui_Label10;
void ui_event_Panel6(lv_event_t * e);
lv_obj_t * ui_Panel6;
lv_obj_t * ui_Label9;
lv_obj_t * ui_ChangeAIChat1;


// SCREEN: ui_Calendar
void ui_Calendar_screen_init(void);
void ui_event_Calendar(lv_event_t * e);
lv_obj_t * ui_Calendar;
lv_obj_t * ui_Calendar1;
lv_obj_t * ui_ChangeAIChat3;


// SCREEN: ui_Music
void ui_Music_screen_init(void);
void ui_event_Music(lv_event_t * e);
lv_obj_t * ui_Music;
void ui_event_ButtonUPMusic(lv_event_t * e);
lv_obj_t * ui_ButtonUPMusic;
lv_obj_t * ui_Label32;
void ui_event_ButtonstartMusic(lv_event_t * e);
lv_obj_t * ui_ButtonstartMusic;
lv_obj_t * ui_Label21;
void ui_event_ButtondownMusci(lv_event_t * e);
lv_obj_t * ui_ButtondownMusci;
lv_obj_t * ui_Label16;
lv_obj_t * ui_LabelMusicname;
lv_obj_t * ui_ChangeAIChat4;


// SCREEN: ui_Weather
void ui_Weather_screen_init(void);
void ui_event_Weather(lv_event_t * e);
lv_obj_t * ui_Weather;
lv_obj_t * ui_LabelTemp;
lv_obj_t * ui_LabelText;
lv_obj_t * ui_Labelhighlow;
lv_obj_t * ui_Label23;
lv_obj_t * ui_LabelWind;
lv_obj_t * ui_Labelrhtext;
lv_obj_t * ui_Labelrh;
lv_obj_t * ui_ChangeAIChat8;


// SCREEN: ui_Memo
void ui_Memo_screen_init(void);
void ui_event_Memo(lv_event_t * e);
lv_obj_t * ui_Memo;
lv_obj_t * ui_MemoText;
lv_obj_t * ui_ChangeAIChat9;
void ui_event_Button3(lv_event_t * e);
lv_obj_t * ui_Button3;
lv_obj_t * ui_Label37;


// SCREEN: ui_Volume
void ui_Volume_screen_init(void);
lv_obj_t * ui_Volume;
lv_obj_t * ui_Label14;
lv_obj_t * ui_Label17;
lv_obj_t * ui_Label18;
lv_obj_t * ui_Label19;
lv_obj_t * ui_Label24;
lv_obj_t * ui_Label25;
lv_obj_t * ui_Panel13;
lv_obj_t * ui_Label26;
lv_obj_t * ui_Panel14;
lv_obj_t * ui_Label27;
lv_obj_t * ui_Panel15;
lv_obj_t * ui_Label28;
lv_obj_t * ui_Panel16;
lv_obj_t * ui_Label29;
lv_obj_t * ui_Panel17;
lv_obj_t * ui_Label30;
lv_obj_t * ui_Panel18;
lv_obj_t * ui_Label31;
lv_obj_t * ui_ChangeVolume;
void ui_event_Slider2(lv_event_t * e);
lv_obj_t * ui_Slider2;
void ui_event_Panel9(lv_event_t * e);
lv_obj_t * ui_Panel9;
void ui_event_LabelVolumevalue(lv_event_t * e);
lv_obj_t * ui_LabelVolumevalue;
lv_obj_t * ui_ChangeAIChat5;


// SCREEN: ui_Memotext
void ui_Memotext_screen_init(void);
void ui_event_Memotext(lv_event_t * e);
lv_obj_t * ui_Memotext;
lv_obj_t * ui_TextAreamemotext;
// void ui_event_TextAreamemoname(lv_event_t * e);
lv_obj_t * ui_TextAreamemoname;
lv_obj_t * ui_ChangeAIChat6;
// lv_obj_t * ui_Keyboard1;
void ui_event_Button2(lv_event_t * e);
lv_obj_t * ui_Button2;
lv_obj_t * ui_Label33;


// SCREEN: ui_WiFi
void ui_WiFi_screen_init(void);
void ui_event_WiFi(lv_event_t * e);
lv_obj_t * ui_WiFi;
lv_obj_t * ui_Panel7;
lv_obj_t * ui_Label15;
void ui_event_Dropdown1(lv_event_t * e);
lv_obj_t * ui_Dropdown1;
void ui_event_TextArea2(lv_event_t * e);
lv_obj_t * ui_TextArea2;
void ui_event_Button1(lv_event_t * e);
lv_obj_t * ui_Button1;
lv_obj_t * ui_Label22;
void ui_event_Keyboard2(lv_event_t * e);
lv_obj_t * ui_Keyboard2;
lv_obj_t * ui_ChangeAIChat7;


// SCREEN: ui_AIchattext
void ui_AIchattext_screen_init(void);
lv_obj_t * ui_AIchattext;
lv_obj_t * ui_TextAreaUser;
lv_obj_t * ui_TextAreaChat;
lv_obj_t * ui_Image1;
lv_obj_t * ui_Image2;
lv_obj_t * ui____initial_actions0;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=1
    #error "LV_COLOR_16_SWAP should be 1 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////
void Keyboardgo_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 500);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_0, 190, 60);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_start(&PropertyAnimation_0);

}
void AIChatGO_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 500);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_0, 200, 0);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_start(&PropertyAnimation_0);

}
void Keyboardout_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 500);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_0, 60, 190);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_start(&PropertyAnimation_0);

}
void Keyboardgo1_Animation(lv_obj_t * TargetObject, int delay)
{
    ui_anim_user_data_t * PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 500);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_0, 130, 60);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_start(&PropertyAnimation_0);

}

///////////////////// FUNCTIONS ////////////////////
void ui_event_AIChat(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP) {
        lv_indev_wait_release(lv_indev_get_act());
        ToListening(e);
        _ui_screen_change(&ui_Menu, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Menu_screen_init);
    }
}
void ui_event_Menu(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Calendar, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Calendar_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_WiFi, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_WiFi_screen_init);
    }
}
void ui_event_Panel1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_Calendar, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Calendar_screen_init);
    }
}
void ui_event_Panel2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_Music, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Music_screen_init);
    }
}
void ui_event_Panel3(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_Volume, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Volume_screen_init);
    }
}
void ui_event_Panel4(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_Weather, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Weather_screen_init);
    }
}
void ui_event_Panel5(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_Memo, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Memo_screen_init);
    }
}
void ui_event_Panel6(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(&ui_WiFi, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_WiFi_screen_init);
    }
}
void ui_event_Calendar(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Music, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Music_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Menu, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Menu_screen_init);
    }
}
void ui_event_Music(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Weather, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Weather_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Calendar, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Calendar_screen_init);
    }
}
void ui_event_ButtonUPMusic(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        Chgemusicup(e);
    }
}
void ui_event_ButtonstartMusic(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        MusicStartatop(e);
    }
}
void ui_event_ButtondownMusci(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        ChgemusicDowm(e);
    }
}
void ui_event_Weather(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Memo, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Memo_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Music, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Music_screen_init);
    }
}
void ui_event_Memo(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_WiFi, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_WiFi_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Weather, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Weather_screen_init);
    }
}
void ui_event_Button3(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        SetMemotext(e);
        _ui_screen_change(&ui_Memotext, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Memotext_screen_init);
    }
}
void ui_event_Slider2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED) {
        _ui_slider_set_text_value(ui_LabelVolumevalue, target, "", "");
        uint16_t value_a = lv_slider_get_value(ui_Slider2);
        if(value_a <=0)
        {value_a = 0;}
        if(value_a >=100)
        value_a   = 100;
    bsp_codec_mute_set(true); // 静音
    bsp_codec_mute_set(false); // 取消静音
    bsp_codec_volume_set(value_a,NULL);//设置音量
    ESP_LOGI(TAG, "volume_value:%d", value_a);
    }
}
void ui_event_Panel9(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_SHORT_CLICKED) {
        _ui_screen_change(&ui_Menu, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, &ui_Menu_screen_init);
    }
}
void ui_event_LabelVolumevalue(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED) {
        CHANGEVolume(e);
    }
}
void ui_event_Memotext(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Weather, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Weather_screen_init);
    }
}
// void ui_event_TextAreamemoname(lv_event_t * e)
// {
//     lv_event_code_t event_code = lv_event_get_code(e);
//     lv_obj_t * target = lv_event_get_target(e);
//     if(event_code == LV_EVENT_PRESSED) {
//         Keyboardgo1_Animation(ui_Keyboard1, 0);
//     }
// }
void ui_event_Button2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        DelMemotext(e);
        _ui_screen_change(&ui_Memo, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Memo_screen_init);
    }
}
void ui_event_WiFi(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_LEFT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Memo, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Memo_screen_init);
    }
    if(event_code == LV_EVENT_GESTURE &&  lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        lv_indev_wait_release(lv_indev_get_act());
        _ui_screen_change(&ui_Menu, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, &ui_Menu_screen_init);
    }
}
void ui_event_Dropdown1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) {
        _ui_checked_set_text_value(ui_Label15, target, "WiFi", "请选择WiFi");
    }
}
void ui_event_TextArea2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) {
        _ui_checked_set_text_value(ui_Label15, target, "WiFi", "请输入密码");
    }
    if(event_code == LV_EVENT_CLICKED) {
        Keyboardgo_Animation(ui_Keyboard2, 0);
    }
    if(event_code == LV_EVENT_VALUE_CHANGED) {
        WIFIpassworrd(e);
    }
}
void ui_event_Button1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_PRESSED) {
        _ui_checked_set_text_value(ui_Label15, target, "WiFi", "WiFi连接中");
    }
    if(event_code == LV_EVENT_CLICKED) {
        WIFIgoto(e);
        Keyboardout_Animation(ui_Keyboard2, 0);
    }
}
void ui_event_Keyboard2(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED) {
        _ui_keyboard_set_target(ui_Keyboard2,  ui_TextArea2);
    }
}

///////////////////// SCREENS ////////////////////

void ui_init(void)
{
    LV_EVENT_GET_COMP_CHILD = lv_event_register_id();

    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_AIChat_screen_init();
    ui_Menu_screen_init();
    ui_Calendar_screen_init();
    ui_Music_screen_init();
    ui_Weather_screen_init();
    ui_Memo_screen_init();
    ui_Volume_screen_init();
    ui_Memotext_screen_init();
    ui_WiFi_screen_init();
    ui_AIchattext_screen_init();
    ui____initial_actions0 = lv_obj_create(NULL);
    lv_disp_load_scr(ui_AIChat);
}
