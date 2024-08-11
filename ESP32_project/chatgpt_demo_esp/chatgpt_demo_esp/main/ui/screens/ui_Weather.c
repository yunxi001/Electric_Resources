// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.1
// LVGL version: 8.3.11
// Project name: esp_box3

#include "../ui.h"

void ui_Weather_screen_init(void)
{
    ui_Weather = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Weather, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_Weather, lv_color_hex(0xF5E6C7), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Weather, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LabelTemp = lv_label_create(ui_Weather);
    lv_obj_set_width(ui_LabelTemp, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LabelTemp, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_LabelTemp, 1);
    lv_obj_set_y(ui_LabelTemp, 29);
    lv_obj_set_align(ui_LabelTemp, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_LabelTemp, "26°");
    lv_obj_set_style_text_font(ui_LabelTemp, &ui_font_Font1, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LabelText = lv_label_create(ui_Weather);
    lv_obj_set_width(ui_LabelText, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LabelText, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_LabelText, 16);
    lv_obj_set_y(ui_LabelText, 123);
    lv_label_set_text(ui_LabelText, "多云");
    lv_obj_set_style_text_font(ui_LabelText, &ui_font_Font2, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Labelhighlow = lv_label_create(ui_Weather);
    lv_obj_set_width(ui_Labelhighlow, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Labelhighlow, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Labelhighlow, -13);
    lv_obj_set_y(ui_Labelhighlow, 126);
    lv_obj_set_align(ui_Labelhighlow, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_Labelhighlow, "24° / 34°");
    lv_obj_set_style_text_font(ui_Labelhighlow, &ui_font_Font3, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label23 = lv_label_create(ui_Weather);
    lv_obj_set_width(ui_Label23, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label23, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label23, -7);
    lv_obj_set_y(ui_Label23, 5);
    lv_obj_set_align(ui_Label23, LV_ALIGN_TOP_RIGHT);
    lv_label_set_text(ui_Label23, "文峰区");
    lv_obj_set_style_text_font(ui_Label23, &ui_font_Font2, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LabelWind = lv_label_create(ui_Weather);
    lv_obj_set_width(ui_LabelWind, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LabelWind, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_LabelWind, 15);
    lv_obj_set_y(ui_LabelWind, 166);
    lv_label_set_text(ui_LabelWind, "东南风二级");
    lv_obj_set_style_text_font(ui_LabelWind, &ui_font_Font2, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Labelrhtext = lv_label_create(ui_Weather);
    lv_obj_set_width(ui_Labelrhtext, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Labelrhtext, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Labelrhtext, -25);
    lv_obj_set_y(ui_Labelrhtext, 125);
    lv_obj_set_align(ui_Labelrhtext, LV_ALIGN_TOP_RIGHT);
    lv_label_set_text(ui_Labelrhtext, "湿度");
    lv_obj_set_style_text_font(ui_Labelrhtext, &ui_font_Font2, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Labelrh = lv_label_create(ui_Weather);
    lv_obj_set_width(ui_Labelrh, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Labelrh, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Labelrh, -26);
    lv_obj_set_y(ui_Labelrh, 155);
    lv_obj_set_align(ui_Labelrh, LV_ALIGN_TOP_RIGHT);
    lv_label_set_text(ui_Labelrh, "50%");
    lv_obj_set_style_text_font(ui_Labelrh, &ui_font_Font3, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ChangeAIChat8 = ui_ChangeAIChat1_create(ui_Weather);
    lv_obj_set_x(ui_ChangeAIChat8, -1);
    lv_obj_set_y(ui_ChangeAIChat8, 108);


    lv_obj_add_event_cb(ui_Weather, ui_event_Weather, LV_EVENT_ALL, NULL);

}
