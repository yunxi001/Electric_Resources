SET(SOURCES screens/ui_Screen1.c
    screens/ui_Screen2.c
    screens/ui_Screen3.c
    screens/ui_Screen4.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_settings_icon_png.c
    images/ui_img_cat_wait_png.c
    images/ui_img_forward_media_png.c
    images/ui_img_backward_media_png.c
    images/ui_img_headphones_music_png.c
    images/ui_img_kapigongde1_png.c
    images/ui_img_kapigongde2_png.c
    images/ui_img_keyboard2_png.c
    images/ui_img_rmk_logo_60_png.c
    images/ui_img_sao2_png.c
    fonts/ui_font_clock.c
    fonts/ui_font_clock2.c
    fonts/ui_font_clockDemo.c
    fonts/ui_font_fans.c
    fonts/ui_font_gongde.c
    fonts/ui_font_HAPanel.c
    fonts/ui_font_market.c
    fonts/ui_font_MedBox.c
    fonts/ui_font_MedBox2.c
    fonts/ui_font_weatherWeek.c)

add_library(ui ${SOURCES})
