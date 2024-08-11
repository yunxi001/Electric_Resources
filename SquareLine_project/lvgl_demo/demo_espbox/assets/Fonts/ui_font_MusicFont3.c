/*******************************************************************************
 * Size: 55 px
 * Bpp: 1
 * Opts: --bpp 1 --size 55 --font C:/Users/27392/Desktop/lvgl_demo/demo_espbox/assets/Fonts/music.ttf -o C:/Users/27392/Desktop/lvgl_demo/demo_espbox/assets/Fonts\ui_font_MusicFont3.c --format lvgl -r 0xe608 --no-compress --no-prefilter
 ******************************************************************************/

#include "../ui.h"

#ifndef UI_FONT_MUSICFONT3
#define UI_FONT_MUSICFONT3 1
#endif

#if UI_FONT_MUSICFONT3

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+E608 "" */
    0x0, 0x0, 0x7f, 0xe0, 0x0, 0x0, 0x0, 0x3f,
    0xff, 0xc0, 0x0, 0x0, 0xf, 0xff, 0xff, 0x0,
    0x0, 0x3, 0xfc, 0x3, 0xfc, 0x0, 0x0, 0xfe,
    0x0, 0x7, 0xf0, 0x0, 0x1f, 0x80, 0x0, 0x1f,
    0x80, 0x3, 0xe0, 0x0, 0x0, 0x7c, 0x0, 0x7c,
    0x0, 0x0, 0x3, 0xe0, 0xf, 0x80, 0x0, 0x0,
    0x1f, 0x0, 0xf0, 0x0, 0x0, 0x0, 0xf0, 0x1e,
    0x0, 0x0, 0x0, 0x7, 0x83, 0xe0, 0x0, 0x0,
    0x0, 0x7c, 0x3c, 0x0, 0x0, 0x0, 0x3, 0xc7,
    0xc0, 0x0, 0x0, 0x0, 0x3e, 0x78, 0x0, 0x0,
    0x0, 0x1, 0xe7, 0x80, 0x0, 0x0, 0x0, 0x1e,
    0xf8, 0x0, 0x0, 0x0, 0x1, 0xef, 0x0, 0x0,
    0x0, 0x0, 0xf, 0xf0, 0x0, 0x0, 0x0, 0x0,
    0xff, 0x0, 0x0, 0x0, 0x0, 0xf, 0xf0, 0x0,
    0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x0, 0x0,
    0xf, 0xfe, 0x0, 0x0, 0x0, 0x7, 0xff, 0xf8,
    0x0, 0x0, 0x1, 0xff, 0xff, 0xc0, 0x0, 0x0,
    0x3f, 0xff, 0xfc, 0x0, 0x0, 0x3, 0xff, 0xf3,
    0xe0, 0x0, 0x0, 0x7c, 0xff, 0x1e, 0x0, 0x0,
    0x7, 0x8f, 0xf1, 0xe0, 0x0, 0x0, 0x78, 0xff,
    0x1e, 0x0, 0x0, 0x7, 0x8f, 0xf1, 0xe0, 0x0,
    0x0, 0x78, 0xff, 0x1e, 0x0, 0x0, 0x7, 0x8f,
    0xf1, 0xe0, 0x0, 0x0, 0x78, 0xff, 0x3c, 0x0,
    0x0, 0x3, 0xcf, 0xff, 0xc0, 0x0, 0x0, 0x3f,
    0xf7, 0xf8, 0x0, 0x0, 0x1, 0xfe, 0x3e, 0x0,
    0x0, 0x0, 0x7, 0xc0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 880, .box_w = 44, .box_h = 37, .ofs_x = 5, .ofs_y = 2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 58888, .range_length = 1, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t ui_font_MusicFont3 = {
#else
lv_font_t ui_font_MusicFont3 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 37,          /*The maximum line height required by the font*/
    .base_line = -2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = 1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if UI_FONT_MUSICFONT3*/

