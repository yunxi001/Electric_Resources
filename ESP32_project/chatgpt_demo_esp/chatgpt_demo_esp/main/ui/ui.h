// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.1
// LVGL version: 8.3.11
// Project name: esp_box3

#ifndef _ESP_BOX3_UI_H
#define _ESP_BOX3_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#include "ui_helpers.h"
#include "components/ui_comp.h"
#include "components/ui_comp_hook.h"
#include "ui_events.h"
#include "ui_theme_manager.h"
#include "ui_themes.h"

void Keyboardgo_Animation(lv_obj_t * TargetObject, int delay);
void AIChatGO_Animation(lv_obj_t * TargetObject, int delay);
void Keyboardout_Animation(lv_obj_t * TargetObject, int delay);
void Keyboardgo1_Animation(lv_obj_t * TargetObject, int delay);
// SCREEN: ui_AIChat
void ui_AIChat_screen_init(void);
void ui_event_AIChat(lv_event_t * e);
extern lv_obj_t * ui_AIChat;
extern lv_obj_t * ui_Label1;
extern lv_obj_t * ui_TimeLable;
extern lv_obj_t * ui_DataLabel;
extern lv_obj_t * ui_Label22WIFI;
extern lv_obj_t * ui_PanelWIFIhiden;
extern lv_obj_t * ui_Label36;
extern lv_obj_t * ui_Bar2;
extern lv_obj_t * ui_Label35;
extern lv_obj_t * ui_cat_gif;
// SCREEN: ui_Menu
void ui_Menu_screen_init(void);
void ui_event_Menu(lv_event_t * e);
extern lv_obj_t * ui_Menu;
extern lv_obj_t * ui_Label12;
extern lv_obj_t * ui_Label13;
extern lv_obj_t * ui_Label2;
extern lv_obj_t * ui_Label3;
extern lv_obj_t * ui_Label4;
extern lv_obj_t * ui_Label5;
void ui_event_Panel1(lv_event_t * e);
extern lv_obj_t * ui_Panel1;
extern lv_obj_t * ui_Label7;
void ui_event_Panel2(lv_event_t * e);
extern lv_obj_t * ui_Panel2;
extern lv_obj_t * ui_Label11;
void ui_event_Panel3(lv_event_t * e);
extern lv_obj_t * ui_Panel3;
extern lv_obj_t * ui_Label6;
void ui_event_Panel4(lv_event_t * e);
extern lv_obj_t * ui_Panel4;
extern lv_obj_t * ui_Label8;
void ui_event_Panel5(lv_event_t * e);
extern lv_obj_t * ui_Panel5;
extern lv_obj_t * ui_Label10;
void ui_event_Panel6(lv_event_t * e);
extern lv_obj_t * ui_Panel6;
extern lv_obj_t * ui_Label9;
extern lv_obj_t * ui_ChangeAIChat1;
// SCREEN: ui_Calendar
void ui_Calendar_screen_init(void);
void ui_event_Calendar(lv_event_t * e);
extern lv_obj_t * ui_Calendar;
extern lv_obj_t * ui_Calendar1;
extern lv_obj_t * ui_ChangeAIChat3;
// SCREEN: ui_Music
void ui_Music_screen_init(void);
void ui_event_Music(lv_event_t * e);
extern lv_obj_t * ui_Music;
void ui_event_ButtonUPMusic(lv_event_t * e);
extern lv_obj_t * ui_ButtonUPMusic;
extern lv_obj_t * ui_Label32;
void ui_event_ButtonstartMusic(lv_event_t * e);
extern lv_obj_t * ui_ButtonstartMusic;
extern lv_obj_t * ui_Label21;
void ui_event_ButtondownMusci(lv_event_t * e);
extern lv_obj_t * ui_ButtondownMusci;
extern lv_obj_t * ui_Label16;
extern lv_obj_t * ui_LabelMusicname;
extern lv_obj_t * ui_ChangeAIChat4;
// SCREEN: ui_Weather
void ui_Weather_screen_init(void);
void ui_event_Weather(lv_event_t * e);
extern lv_obj_t * ui_Weather;
extern lv_obj_t * ui_LabelTemp;
extern lv_obj_t * ui_LabelText;
extern lv_obj_t * ui_Labelhighlow;
extern lv_obj_t * ui_Label23;
extern lv_obj_t * ui_LabelWind;
extern lv_obj_t * ui_Labelrhtext;
extern lv_obj_t * ui_Labelrh;
extern lv_obj_t * ui_ChangeAIChat8;
// SCREEN: ui_Memo
void ui_Memo_screen_init(void);
void ui_event_Memo(lv_event_t * e);
extern lv_obj_t * ui_Memo;
extern lv_obj_t * ui_MemoText;
extern lv_obj_t * ui_ChangeAIChat9;
void ui_event_Button3(lv_event_t * e);
extern lv_obj_t * ui_Button3;
extern lv_obj_t * ui_Label37;
// SCREEN: ui_Volume
void ui_Volume_screen_init(void);
extern lv_obj_t * ui_Volume;
extern lv_obj_t * ui_Label14;
extern lv_obj_t * ui_Label17;
extern lv_obj_t * ui_Label18;
extern lv_obj_t * ui_Label19;
extern lv_obj_t * ui_Label24;
extern lv_obj_t * ui_Label25;
extern lv_obj_t * ui_Panel13;
extern lv_obj_t * ui_Label26;
extern lv_obj_t * ui_Panel14;
extern lv_obj_t * ui_Label27;
extern lv_obj_t * ui_Panel15;
extern lv_obj_t * ui_Label28;
extern lv_obj_t * ui_Panel16;
extern lv_obj_t * ui_Label29;
extern lv_obj_t * ui_Panel17;
extern lv_obj_t * ui_Label30;
extern lv_obj_t * ui_Panel18;
extern lv_obj_t * ui_Label31;
extern lv_obj_t * ui_ChangeVolume;
void ui_event_Slider2(lv_event_t * e);
extern lv_obj_t * ui_Slider2;
void ui_event_Panel9(lv_event_t * e);
extern lv_obj_t * ui_Panel9;
void ui_event_LabelVolumevalue(lv_event_t * e);
extern lv_obj_t * ui_LabelVolumevalue;
extern lv_obj_t * ui_ChangeAIChat5;
// SCREEN: ui_Memotext
void ui_Memotext_screen_init(void);
void ui_event_Memotext(lv_event_t * e);
extern lv_obj_t * ui_Memotext;
extern lv_obj_t * ui_TextAreamemotext;
// void ui_event_TextAreamemoname(lv_event_t * e);
extern lv_obj_t * ui_TextAreamemoname;
extern lv_obj_t * ui_ChangeAIChat6;
// extern lv_obj_t * ui_Keyboard1;
void ui_event_Button2(lv_event_t * e);
extern lv_obj_t * ui_Button2;
extern lv_obj_t * ui_Label33;
// SCREEN: ui_WiFi
void ui_WiFi_screen_init(void);
void ui_event_WiFi(lv_event_t * e);
extern lv_obj_t * ui_WiFi;
extern lv_obj_t * ui_Panel7;
extern lv_obj_t * ui_Label15;
void ui_event_Dropdown1(lv_event_t * e);
extern lv_obj_t * ui_Dropdown1;
void ui_event_TextArea2(lv_event_t * e);
extern lv_obj_t * ui_TextArea2;
void ui_event_Button1(lv_event_t * e);
extern lv_obj_t * ui_Button1;
extern lv_obj_t * ui_Label22;
void ui_event_Keyboard2(lv_event_t * e);
extern lv_obj_t * ui_Keyboard2;
extern lv_obj_t * ui_ChangeAIChat7;
// SCREEN: ui_AIchattext
void ui_AIchattext_screen_init(void);
extern lv_obj_t * ui_AIchattext;
extern lv_obj_t * ui_TextAreaUser;
extern lv_obj_t * ui_TextAreaChat;
extern lv_obj_t * ui_Image1;
extern lv_obj_t * ui_Image2;
extern lv_obj_t * ui____initial_actions0;

LV_IMG_DECLARE(blue_cat_gif);
LV_IMG_DECLARE(ui_img_user_png);    // assets/user.png
LV_IMG_DECLARE(ui_img_reply_chatgpt_logo_png);    // assets/reply_chatgpt_logo.png



LV_FONT_DECLARE(ui_font_Font1);
LV_FONT_DECLARE(ui_font_Font2);
LV_FONT_DECLARE(ui_font_Font3);
LV_FONT_DECLARE(ui_font_FontAI1);
LV_FONT_DECLARE(ui_font_fontwifi);
LV_FONT_DECLARE(ui_font_icon1);
LV_FONT_DECLARE(ui_font_icon2);
LV_FONT_DECLARE(ui_font_icon3);
LV_FONT_DECLARE(ui_font_icon4);
LV_FONT_DECLARE(ui_font_MusicFont1);
LV_FONT_DECLARE(ui_font_MusicFont2);
LV_FONT_DECLARE(ui_font_MusicFont3);
LV_FONT_DECLARE(ui_font_Fontshanchu);



void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
