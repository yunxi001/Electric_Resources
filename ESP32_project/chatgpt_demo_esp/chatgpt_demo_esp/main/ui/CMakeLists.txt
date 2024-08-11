SET(SOURCES components/ui_comp_changeaichat1.c
    components/ui_comp.c
    screens/ui_AIChat.c
    screens/ui_Menu.c
    screens/ui_Calendar.c
    screens/ui_Music.c
    screens/ui_Weather.c
    screens/ui_Memo.c
    screens/ui_Volume.c
    screens/ui_Memotext.c
    screens/ui_WiFi.c
    screens/ui_AIchattext.c
    ui_theme_manager.c
    ui_themes.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_user_png.c
    images/ui_img_reply_chatgpt_logo_png.c
    fonts/ui_font_Font1.c
    fonts/ui_font_Font2.c
    fonts/ui_font_Font3.c
    fonts/ui_font_FontAI1.c
    fonts/ui_font_fontwifi.c
    fonts/ui_font_icon1.c
    fonts/ui_font_icon2.c
    fonts/ui_font_icon3.c
    fonts/ui_font_icon4.c
    fonts/ui_font_MusicFont1.c
    fonts/ui_font_MusicFont2.c
    fonts/ui_font_MusicFont3.c
    fonts/ui_font_Fontshanchu.c)

add_library(ui ${SOURCES})
