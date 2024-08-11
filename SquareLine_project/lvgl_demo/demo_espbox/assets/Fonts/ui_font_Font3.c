/*******************************************************************************
 * Size: 20 px
 * Bpp: 1
 * Opts: --bpp 1 --size 20 --font C:/Users/27392/Desktop/lvgl_demo/demo_espbox/assets/Fonts/GenJyuuGothic-Medium-2.ttf -o C:/Users/27392/Desktop/lvgl_demo/demo_espbox/assets/Fonts\ui_font_Font3.c --format lvgl -r 0x20-0x7f --symbols ° --no-compress --no-prefilter
 ******************************************************************************/

#include "../ui.h"

#ifndef UI_FONT_FONT3
#define UI_FONT_FONT3 1
#endif

#if UI_FONT_FONT3

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0x6d, 0xb6, 0xdb, 0x6c, 0x7f, 0xc0,

    /* U+0022 "\"" */
    0xc7, 0x8f, 0x1e, 0x3c, 0x78, 0xc0,

    /* U+0023 "#" */
    0x11, 0x4, 0x41, 0x11, 0xff, 0x7f, 0xc4, 0xc3,
    0x20, 0xc8, 0xff, 0xbf, 0xe2, 0x20, 0x88, 0x22,
    0x9, 0x80,

    /* U+0024 "$" */
    0x18, 0x18, 0x3c, 0x7f, 0xe3, 0xc0, 0xc0, 0xe0,
    0x78, 0x3c, 0xe, 0x7, 0x3, 0x3, 0xc7, 0xfe,
    0x3c, 0x18, 0x18,

    /* U+0025 "%" */
    0x38, 0x18, 0x3e, 0x8, 0x3b, 0x8c, 0x18, 0xc4,
    0xc, 0x66, 0x6, 0x32, 0x73, 0xba, 0x7c, 0xfb,
    0x77, 0x39, 0x31, 0x81, 0x98, 0xc0, 0x8c, 0x60,
    0xc7, 0x70, 0xc1, 0xf0, 0x40, 0x70,

    /* U+0026 "&" */
    0xf, 0x0, 0xfc, 0xe, 0x60, 0x63, 0x3, 0x10,
    0x9, 0x0, 0x70, 0x6d, 0xc3, 0xc7, 0x36, 0x1f,
    0xb0, 0x79, 0xc3, 0xe7, 0xff, 0x9f, 0x8c,

    /* U+0027 "'" */
    0xff, 0xf0,

    /* U+0028 "(" */
    0x13, 0x26, 0x6c, 0xcc, 0xcc, 0xcc, 0xcc, 0x66,
    0x63, 0x30,

    /* U+0029 ")" */
    0x8c, 0x46, 0x62, 0x33, 0x33, 0x33, 0x33, 0x66,
    0x6c, 0xc0,

    /* U+002A "*" */
    0x10, 0x33, 0xfb, 0xe3, 0xcd, 0x80, 0x0,

    /* U+002B "+" */
    0xc, 0x3, 0x0, 0xc0, 0x30, 0xff, 0xff, 0xf0,
    0xc0, 0x30, 0xc, 0x3, 0x0,

    /* U+002C "," */
    0xdf, 0xb7, 0x80,

    /* U+002D "-" */
    0xff, 0xc0,

    /* U+002E "." */
    0xff, 0x80,

    /* U+002F "/" */
    0x2, 0xc, 0x18, 0x20, 0xc1, 0x83, 0x4, 0x18,
    0x30, 0x41, 0x83, 0x4, 0x8, 0x30, 0x60, 0x83,
    0x0,

    /* U+0030 "0" */
    0x1c, 0x3f, 0x98, 0xdc, 0x7c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xf8, 0xec, 0x67, 0xf0, 0xe0,

    /* U+0031 "1" */
    0x18, 0xf8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18, 0xff, 0xff,

    /* U+0032 "2" */
    0x1e, 0x1f, 0xc6, 0x38, 0x6, 0x1, 0x80, 0x60,
    0x30, 0xc, 0x6, 0x3, 0x1, 0x80, 0xc0, 0xff,
    0xff, 0xf0,

    /* U+0033 "3" */
    0x3c, 0x7f, 0x31, 0xc0, 0x60, 0x30, 0x30, 0xf0,
    0x3c, 0x3, 0x80, 0xc0, 0x78, 0x7f, 0xf1, 0xf0,

    /* U+0034 "4" */
    0x7, 0x1, 0xc0, 0xf0, 0x6c, 0x1b, 0xc, 0xc6,
    0x31, 0x8c, 0xff, 0xff, 0xf0, 0x30, 0xc, 0x3,
    0x0, 0xc0,

    /* U+0035 "5" */
    0x7f, 0xbf, 0xd8, 0xc, 0x7, 0xe3, 0xf8, 0x8e,
    0x3, 0x1, 0x80, 0xc0, 0x78, 0x6f, 0xf1, 0xe0,

    /* U+0036 "6" */
    0x1e, 0x1f, 0x9c, 0x4c, 0xc, 0x6, 0x73, 0xfd,
    0xc7, 0xc1, 0xe0, 0xf0, 0x6c, 0x73, 0xf0, 0xf0,

    /* U+0037 "7" */
    0xff, 0xff, 0xc0, 0xc0, 0x40, 0x60, 0x20, 0x30,
    0x18, 0x8, 0xc, 0x6, 0x3, 0x1, 0x80, 0xc0,

    /* U+0038 "8" */
    0x1e, 0x1f, 0x9c, 0xec, 0x36, 0x19, 0x8, 0x68,
    0x4e, 0x41, 0xe0, 0xf0, 0x7c, 0x77, 0xf1, 0xf0,

    /* U+0039 "9" */
    0x3c, 0x3f, 0x38, 0xd8, 0x3c, 0x1e, 0xf, 0x8e,
    0xff, 0x39, 0x80, 0xc0, 0xc8, 0xe7, 0xe1, 0xe0,

    /* U+003A ":" */
    0xff, 0x80, 0x0, 0xff, 0x80,

    /* U+003B ";" */
    0xff, 0x80, 0x0, 0xdf, 0x97, 0x80,

    /* U+003C "<" */
    0x0, 0xc1, 0xf3, 0xe3, 0xc0, 0xf0, 0xf, 0x80,
    0x78, 0x3,

    /* U+003D "=" */
    0xff, 0xff, 0xf0, 0x0, 0x0, 0x0, 0x3f, 0xff,
    0xfc,

    /* U+003E ">" */
    0xc0, 0x3e, 0x1, 0xe0, 0x1f, 0x1, 0xc3, 0xe7,
    0xc3, 0xc0, 0x80, 0x0,

    /* U+003F "?" */
    0x7d, 0xfe, 0x38, 0x30, 0x61, 0x86, 0x18, 0x70,
    0x40, 0x3, 0x87, 0xe, 0x0,

    /* U+0040 "@" */
    0x1, 0xf0, 0x3, 0xfe, 0x7, 0x83, 0x87, 0x0,
    0xe3, 0x0, 0x33, 0xe, 0x8d, 0x9f, 0xc7, 0x8c,
    0x63, 0xcc, 0x21, 0xe6, 0x10, 0xf3, 0x18, 0xf9,
    0x8c, 0xec, 0xfb, 0xe7, 0x39, 0xe1, 0x80, 0x0,
    0x60, 0x0, 0x3c, 0x10, 0x7, 0xfc, 0x1, 0xf8,
    0x0,

    /* U+0041 "A" */
    0x7, 0x0, 0xf0, 0xf, 0x0, 0xd8, 0x19, 0x81,
    0x98, 0x18, 0xc3, 0xc, 0x3f, 0xe3, 0xfe, 0x60,
    0x66, 0x7, 0x60, 0x3c, 0x3,

    /* U+0042 "B" */
    0xff, 0x3f, 0xec, 0x1f, 0x3, 0xc0, 0xf0, 0x6f,
    0xfb, 0xfe, 0xc1, 0xf0, 0x3c, 0xf, 0x7, 0xff,
    0xbf, 0xc0,

    /* U+0043 "C" */
    0xf, 0x87, 0xf9, 0xc1, 0x30, 0xc, 0x1, 0x80,
    0x30, 0x6, 0x0, 0xc0, 0x18, 0x1, 0x80, 0x38,
    0x33, 0xfe, 0x1f, 0x0,

    /* U+0044 "D" */
    0xfe, 0x1f, 0xf3, 0xf, 0x60, 0x6c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x78, 0x1f, 0x3, 0x61,
    0xef, 0xf9, 0xfc, 0x0,

    /* U+0045 "E" */
    0xff, 0x7f, 0xb0, 0x18, 0xc, 0x6, 0x3, 0xf9,
    0xfc, 0xc0, 0x60, 0x30, 0x18, 0xf, 0xf7, 0xf8,

    /* U+0046 "F" */
    0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xfe,
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0047 "G" */
    0xf, 0x87, 0xf9, 0xc1, 0x30, 0xc, 0x1, 0x80,
    0x30, 0xfe, 0x1f, 0xc0, 0x78, 0xd, 0x81, 0xb8,
    0x33, 0xfe, 0x1f, 0x0,

    /* U+0048 "H" */
    0xc0, 0x78, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xff, 0xff, 0xff, 0xc0, 0x78, 0xf, 0x1, 0xe0,
    0x3c, 0x7, 0x80, 0xc0,

    /* U+0049 "I" */
    0xff, 0xff, 0xff, 0xf0,

    /* U+004A "J" */
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0xc7, 0xfe, 0x3c,

    /* U+004B "K" */
    0xc1, 0xd8, 0x73, 0xc, 0x63, 0xc, 0xc1, 0xb8,
    0x3f, 0x7, 0xb0, 0xe7, 0x18, 0x63, 0xe, 0x60,
    0xcc, 0xd, 0x81, 0x80,

    /* U+004C "L" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0, 0xff, 0xff,

    /* U+004D "M" */
    0xe0, 0x3f, 0x1, 0xfc, 0x1f, 0xe0, 0xfd, 0x87,
    0xec, 0x6f, 0x63, 0x79, 0xb3, 0xcd, 0x9e, 0x2c,
    0xf1, 0xc7, 0x8e, 0x3c, 0x21, 0xe0, 0xc,

    /* U+004E "N" */
    0xe0, 0x7c, 0xf, 0xc1, 0xfc, 0x3d, 0x87, 0xb8,
    0xf3, 0x1e, 0x33, 0xc7, 0x78, 0x6f, 0xf, 0xe0,
    0xfc, 0xf, 0x81, 0xc0,

    /* U+004F "O" */
    0xf, 0x81, 0xff, 0x1c, 0x1c, 0xc0, 0x6c, 0x1,
    0xe0, 0xf, 0x0, 0x78, 0x3, 0xc0, 0x1f, 0x1,
    0xd8, 0xc, 0xe0, 0xe3, 0xfe, 0x7, 0xc0,

    /* U+0050 "P" */
    0xff, 0x3f, 0xec, 0x1f, 0x3, 0xc0, 0xf0, 0x3c,
    0x1f, 0xfe, 0xfe, 0x30, 0xc, 0x3, 0x0, 0xc0,
    0x30, 0x0,

    /* U+0051 "Q" */
    0xf, 0x81, 0xff, 0x1c, 0x1c, 0xc0, 0x6e, 0x3,
    0xe0, 0xf, 0x0, 0x78, 0x3, 0xc0, 0x1e, 0x0,
    0xd8, 0xc, 0xc0, 0xe3, 0xde, 0xf, 0xe0, 0x1c,
    0x0, 0x70, 0x1, 0xf8, 0x7, 0xc0,

    /* U+0052 "R" */
    0xff, 0x3f, 0xec, 0x1f, 0x3, 0xc0, 0xf0, 0x7f,
    0xfb, 0xfc, 0xc6, 0x31, 0xcc, 0x33, 0xe, 0xc1,
    0xf0, 0x30,

    /* U+0053 "S" */
    0x3f, 0x1f, 0xee, 0x1b, 0x0, 0xc0, 0x1c, 0x3,
    0xe0, 0x3e, 0x1, 0xc0, 0x30, 0xf, 0x87, 0x7f,
    0x8f, 0xc0,

    /* U+0054 "T" */
    0xff, 0xff, 0xfc, 0x30, 0x6, 0x0, 0xc0, 0x18,
    0x3, 0x0, 0x60, 0xc, 0x1, 0x80, 0x30, 0x6,
    0x0, 0xc0, 0x18, 0x0,

    /* U+0055 "U" */
    0xc0, 0x78, 0xf, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xf0, 0x1e, 0x3, 0xc0, 0x78, 0xf, 0x83, 0xb0,
    0xe7, 0xfc, 0x3e, 0x0,

    /* U+0056 "V" */
    0xc0, 0x3e, 0x6, 0x60, 0x66, 0xe, 0x70, 0xc3,
    0xc, 0x30, 0xc3, 0x98, 0x19, 0x81, 0x98, 0x1f,
    0x0, 0xf0, 0xf, 0x0, 0x60,

    /* U+0057 "W" */
    0xc1, 0x83, 0xc1, 0x83, 0xc3, 0xc3, 0xe3, 0xc3,
    0xe3, 0xc6, 0x62, 0xc6, 0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x74, 0x6c, 0x3c, 0x3c, 0x3c, 0x3c,
    0x3c, 0x3c, 0x18, 0x18,

    /* U+0058 "X" */
    0x60, 0xce, 0x18, 0xc6, 0x1c, 0xc1, 0xb0, 0x3e,
    0x3, 0x80, 0x70, 0x1b, 0x7, 0x60, 0xc6, 0x38,
    0xe6, 0xd, 0x81, 0xc0,

    /* U+0059 "Y" */
    0xc0, 0xf8, 0x76, 0x19, 0xce, 0x33, 0xc, 0xc1,
    0xe0, 0x78, 0xc, 0x3, 0x0, 0xc0, 0x30, 0xc,
    0x3, 0x0,

    /* U+005A "Z" */
    0xff, 0xff, 0xf0, 0x18, 0xc, 0x7, 0x1, 0x80,
    0xc0, 0x70, 0x18, 0xc, 0x6, 0x1, 0x80, 0xff,
    0xff, 0xf0,

    /* U+005B "[" */
    0xfe, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31, 0x8c,
    0x63, 0x18, 0xc7, 0xc0,

    /* U+005C "\\" */
    0x82, 0xc, 0x30, 0x41, 0x86, 0x8, 0x30, 0xc3,
    0x4, 0x18, 0x60, 0x83, 0xc, 0x10, 0x40,

    /* U+005D "]" */
    0xf8, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31,
    0x8c, 0x63, 0x1f, 0xc0,

    /* U+005E "^" */
    0x0, 0x38, 0x38, 0x6c, 0x6c, 0x66, 0xc6, 0xc3,
    0x83,

    /* U+005F "_" */
    0xff, 0xff, 0xfc,

    /* U+0060 "`" */
    0xce, 0x73,

    /* U+0061 "a" */
    0x3e, 0x3f, 0x90, 0xe0, 0x30, 0x79, 0xff, 0x87,
    0x83, 0xc3, 0xff, 0xde, 0x20,

    /* U+0062 "b" */
    0xc0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x37, 0xcf,
    0xfb, 0x86, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xc1,
    0xf8, 0x6f, 0xfb, 0x78,

    /* U+0063 "c" */
    0x1e, 0x3f, 0x60, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0x60, 0x7f, 0x1e,

    /* U+0064 "d" */
    0x0, 0xc0, 0x30, 0xc, 0x3, 0x0, 0xc7, 0xb3,
    0xfd, 0xc7, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xc0,
    0xd8, 0x77, 0xfc, 0x7b,

    /* U+0065 "e" */
    0x1f, 0xf, 0xe6, 0x1f, 0x3, 0xff, 0xff, 0xfc,
    0x3, 0x80, 0x70, 0xf, 0xe1, 0xf0,

    /* U+0066 "f" */
    0x1e, 0x7c, 0xc1, 0x83, 0x1f, 0xbf, 0x18, 0x30,
    0x60, 0xc1, 0x83, 0x6, 0xc, 0x18,

    /* U+0067 "g" */
    0x1f, 0xcf, 0xf7, 0x31, 0x86, 0x61, 0x9c, 0xe3,
    0xf1, 0xf8, 0x60, 0x1f, 0xe7, 0xff, 0x3, 0xc0,
    0xf8, 0x7f, 0xf8, 0xf8,

    /* U+0068 "h" */
    0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x73, 0xfd,
    0xc7, 0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf,
    0x7, 0x83,

    /* U+0069 "i" */
    0xfc, 0x1, 0xb6, 0xdb, 0x6d, 0xb6,

    /* U+006A "j" */
    0x39, 0xc0, 0x0, 0x18, 0xc6, 0x31, 0x8c, 0x63,
    0x18, 0xc6, 0x31, 0x8d, 0xee, 0x0,

    /* U+006B "k" */
    0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x1b, 0x1d,
    0x9c, 0xcc, 0x6c, 0x3f, 0x1f, 0x8e, 0x66, 0x3b,
    0xd, 0x83,

    /* U+006C "l" */
    0xdb, 0x6d, 0xb6, 0xdb, 0x6d, 0xbb,

    /* U+006D "m" */
    0xce, 0x1c, 0xff, 0x7e, 0xe3, 0xc7, 0xc1, 0x83,
    0xc1, 0x83, 0xc1, 0x83, 0xc1, 0x83, 0xc1, 0x83,
    0xc1, 0x83, 0xc1, 0x83, 0xc1, 0x83,

    /* U+006E "n" */
    0xce, 0x7f, 0xb8, 0xf8, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x60,

    /* U+006F "o" */
    0x1e, 0x1f, 0xe6, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0x61, 0x9f, 0xe1, 0xe0,

    /* U+0070 "p" */
    0xdf, 0x3f, 0xee, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x7, 0xe1, 0xbf, 0xed, 0xe3, 0x0, 0xc0,
    0x30, 0xc, 0x3, 0x0,

    /* U+0071 "q" */
    0x1e, 0xdf, 0xf7, 0x1f, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0x61, 0xdf, 0xf1, 0xec, 0x3, 0x0,
    0xc0, 0x30, 0xc, 0x3,

    /* U+0072 "r" */
    0xdf, 0xfe, 0x30, 0xc3, 0xc, 0x30, 0xc3, 0xc,
    0x0,

    /* U+0073 "s" */
    0x3c, 0xfe, 0xc2, 0xc0, 0x60, 0x3c, 0x7, 0x3,
    0xc3, 0xfe, 0x7c,

    /* U+0074 "t" */
    0x30, 0x60, 0xc7, 0xff, 0xe6, 0xc, 0x18, 0x30,
    0x60, 0xc1, 0x83, 0xe3, 0xc0,

    /* U+0075 "u" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xe3, 0xbf, 0xde, 0x60,

    /* U+0076 "v" */
    0xc1, 0xe0, 0xf0, 0x6c, 0x66, 0x33, 0x18, 0xd8,
    0x6c, 0x36, 0xe, 0x7, 0x0,

    /* U+0077 "w" */
    0xc3, 0x7, 0x87, 0x1f, 0xe, 0x37, 0x34, 0x66,
    0x6c, 0xcc, 0xd9, 0x99, 0x36, 0x1e, 0x2c, 0x3c,
    0x78, 0x78, 0xf0, 0x61, 0xc0,

    /* U+0078 "x" */
    0xc3, 0x71, 0x9d, 0x86, 0xc3, 0xc0, 0xe0, 0xf0,
    0xec, 0x67, 0x71, 0xb0, 0x60,

    /* U+0079 "y" */
    0xc1, 0xe0, 0xf8, 0x6c, 0x66, 0x33, 0x98, 0xd8,
    0x6c, 0x1e, 0xe, 0x7, 0x1, 0x81, 0x80, 0xc3,
    0xc1, 0xc0,

    /* U+007A "z" */
    0xff, 0xff, 0x6, 0xc, 0x1c, 0x18, 0x30, 0x70,
    0x60, 0xff, 0xff,

    /* U+007B "{" */
    0x1c, 0xf3, 0xc, 0x30, 0xc3, 0xc, 0x73, 0x86,
    0xc, 0x30, 0xc3, 0xc, 0x30, 0xc1, 0xc0,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf0,

    /* U+007D "}" */
    0xe3, 0xc3, 0xc, 0x30, 0xc3, 0xc, 0x38, 0x71,
    0x8c, 0x30, 0xc3, 0xc, 0x30, 0xce, 0x0,

    /* U+007E "~" */
    0x70, 0x7e, 0x73, 0xe0, 0xe0,

    /* U+00B0 "°" */
    0x7b, 0x3c, 0xf3, 0xcd, 0xe0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 72, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 110, .box_w = 3, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 7, .adv_w = 166, .box_w = 7, .box_h = 6, .ofs_x = 2, .ofs_y = 10},
    {.bitmap_index = 13, .adv_w = 182, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 31, .adv_w = 182, .box_w = 8, .box_h = 19, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 50, .adv_w = 300, .box_w = 17, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 80, .adv_w = 225, .box_w = 13, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 103, .adv_w = 95, .box_w = 2, .box_h = 6, .ofs_x = 2, .ofs_y = 10},
    {.bitmap_index = 105, .adv_w = 113, .box_w = 4, .box_h = 20, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 115, .adv_w = 113, .box_w = 4, .box_h = 19, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 125, .adv_w = 155, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 132, .adv_w = 182, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 145, .adv_w = 95, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 148, .adv_w = 114, .box_w = 5, .box_h = 2, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 150, .adv_w = 95, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 152, .adv_w = 125, .box_w = 7, .box_h = 19, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 169, .adv_w = 182, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 185, .adv_w = 182, .box_w = 8, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 182, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 217, .adv_w = 182, .box_w = 9, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 233, .adv_w = 182, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 251, .adv_w = 182, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 267, .adv_w = 182, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 283, .adv_w = 182, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 299, .adv_w = 182, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 315, .adv_w = 182, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 331, .adv_w = 95, .box_w = 3, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 95, .box_w = 3, .box_h = 15, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 342, .adv_w = 182, .box_w = 10, .box_h = 8, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 352, .adv_w = 182, .box_w = 10, .box_h = 7, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 361, .adv_w = 182, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 373, .adv_w = 157, .box_w = 7, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 386, .adv_w = 311, .box_w = 17, .box_h = 19, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 427, .adv_w = 199, .box_w = 12, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 448, .adv_w = 213, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 466, .adv_w = 207, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 486, .adv_w = 223, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 506, .adv_w = 192, .box_w = 9, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 522, .adv_w = 181, .box_w = 8, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 536, .adv_w = 224, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 556, .adv_w = 237, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 576, .adv_w = 99, .box_w = 2, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 580, .adv_w = 176, .box_w = 8, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 594, .adv_w = 212, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 614, .adv_w = 178, .box_w = 8, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 628, .adv_w = 265, .box_w = 13, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 651, .adv_w = 235, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 671, .adv_w = 241, .box_w = 13, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 694, .adv_w = 207, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 712, .adv_w = 241, .box_w = 13, .box_h = 18, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 742, .adv_w = 209, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 760, .adv_w = 194, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 778, .adv_w = 195, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 798, .adv_w = 234, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 818, .adv_w = 190, .box_w = 12, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 839, .adv_w = 286, .box_w = 16, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 867, .adv_w = 191, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 887, .adv_w = 177, .box_w = 10, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 905, .adv_w = 194, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 923, .adv_w = 113, .box_w = 5, .box_h = 18, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 935, .adv_w = 125, .box_w = 6, .box_h = 19, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 950, .adv_w = 113, .box_w = 5, .box_h = 18, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 962, .adv_w = 182, .box_w = 8, .box_h = 9, .ofs_x = 2, .ofs_y = 6},
    {.bitmap_index = 971, .adv_w = 180, .box_w = 11, .box_h = 2, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 974, .adv_w = 196, .box_w = 4, .box_h = 4, .ofs_x = 3, .ofs_y = 13},
    {.bitmap_index = 976, .adv_w = 184, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 989, .adv_w = 201, .box_w = 10, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1009, .adv_w = 165, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1020, .adv_w = 202, .box_w = 10, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1040, .adv_w = 181, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1054, .adv_w = 110, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1068, .adv_w = 185, .box_w = 10, .box_h = 16, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1088, .adv_w = 198, .box_w = 9, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1106, .adv_w = 92, .box_w = 3, .box_h = 16, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1112, .adv_w = 92, .box_w = 5, .box_h = 21, .ofs_x = -1, .ofs_y = -5},
    {.bitmap_index = 1126, .adv_w = 184, .box_w = 9, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1144, .adv_w = 95, .box_w = 3, .box_h = 16, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1150, .adv_w = 301, .box_w = 16, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1172, .adv_w = 199, .box_w = 9, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1185, .adv_w = 196, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1199, .adv_w = 202, .box_w = 10, .box_h = 16, .ofs_x = 2, .ofs_y = -5},
    {.bitmap_index = 1219, .adv_w = 202, .box_w = 10, .box_h = 16, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1239, .adv_w = 131, .box_w = 6, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1248, .adv_w = 153, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1259, .adv_w = 127, .box_w = 7, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1272, .adv_w = 198, .box_w = 9, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1285, .adv_w = 174, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1298, .adv_w = 265, .box_w = 15, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1319, .adv_w = 168, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1332, .adv_w = 174, .box_w = 9, .box_h = 16, .ofs_x = 1, .ofs_y = -5},
    {.bitmap_index = 1350, .adv_w = 157, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1361, .adv_w = 113, .box_w = 6, .box_h = 19, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1376, .adv_w = 90, .box_w = 2, .box_h = 22, .ofs_x = 2, .ofs_y = -6},
    {.bitmap_index = 1382, .adv_w = 113, .box_w = 6, .box_h = 19, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1397, .adv_w = 182, .box_w = 9, .box_h = 4, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 1402, .adv_w = 123, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 9}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 176, .range_length = 1, .glyph_id_start = 96,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 1, 0, 0, 0, 0,
    1, 2, 0, 0, 0, 3, 4, 3,
    5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 6, 6, 0, 0, 0,
    0, 0, 7, 8, 9, 10, 0, 11,
    12, 0, 0, 13, 14, 15, 0, 0,
    10, 16, 10, 17, 18, 19, 20, 21,
    22, 23, 24, 25, 2, 26, 0, 0,
    0, 0, 27, 28, 29, 0, 30, 31,
    32, 33, 0, 0, 34, 0, 33, 33,
    28, 28, 35, 36, 37, 38, 35, 39,
    40, 41, 42, 43, 2, 0, 0, 0,
    0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 1, 2, 0, 0, 0, 0,
    2, 0, 3, 4, 0, 5, 6, 5,
    7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 8, 8, 0, 0, 0,
    9, 0, 10, 0, 11, 0, 0, 0,
    11, 0, 0, 12, 0, 0, 0, 0,
    11, 0, 11, 0, 13, 14, 15, 16,
    17, 18, 19, 20, 0, 21, 3, 0,
    0, 0, 22, 0, 23, 23, 23, 24,
    25, 0, 26, 27, 0, 0, 28, 28,
    23, 28, 23, 28, 29, 30, 31, 32,
    33, 34, 35, 36, 0, 0, 3, 0,
    0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, -40, 0, 0, 0,
    0, -21, 0, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -6, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 26, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -36, 0, -49, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -36, -8, -26,
    -12, 0, -32, 0, 0, 0, -2, 0,
    0, 0, 10, 0, 0, -8, 0, -15,
    -9, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, -10, -2, -10, -23, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -13, 0, -26, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -8, -8, 0,
    -3, 5, 5, 0, 0, 0, -6, 0,
    0, 0, 0, 0, 0, 0, 0, -12,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -10, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -20, 0, -28, 0, 0, 0, 0,
    -10, 0, -3, 0, 0, -13, -4, -5,
    0, 0, -5, -2, -13, 0, 0, -3,
    0, 0, 0, 0, 0, -5, -1, -5,
    -2, 0, -5, 0, 0, 0, 0, -10,
    0, 0, 0, 0, 0, 0, 0, -6,
    -5, -8, 0, -3, -2, -2, -5, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -5, -3, -3, -5, 0,
    0, 0, 0, 0, 0, -10, 0, 0,
    0, 0, -11, -3, -8, -5, -5, -2,
    -2, -2, -4, -3, 0, 0, 0, 0,
    -6, 0, 0, 0, 0, -5, -3, -5,
    -3, 0, -5, 0, 0, 0, 0, -9,
    0, 0, -3, 0, 0, -3, 0, -15,
    0, -6, 0, -3, -1, -7, -6, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 0, 0, 0, -26, 0, -18, 0,
    0, -12, -3, -43, -8, 0, 0, 0,
    0, -8, -2, -10, 0, -12, -5, 0,
    -8, 0, 0, -6, -8, -3, -6, -9,
    -7, -10, -7, -13, 0, 0, 0, -8,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, -6, 0, -5, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -8, 0, 0, 0,
    0, 0, 0, -12, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -9, 0, -9,
    0, -13, 0, 0, -4, -3, -8, 0,
    -6, -7, -5, -5, -3, 0, -7, 0,
    0, 0, -4, 0, 0, 0, -3, 0,
    0, -10, -4, -9, -7, -7, -9, -5,
    0, -37, 0, -52, 0, -16, 0, 0,
    -13, 0, -10, 0, -8, -38, -11, -25,
    -18, 0, -26, 0, -26, 0, -4, -5,
    -2, 0, 0, 0, 0, -8, -3, -14,
    -12, 0, -14, 0, 0, 0, 0, 0,
    -39, -5, -20, 0, 0, -16, 0, -46,
    -3, -6, 0, 0, 0, -8, -3, -21,
    0, -12, -7, 0, -7, 0, 0, 0,
    -3, 0, 0, 0, 0, -5, 0, -6,
    0, 0, 0, -3, 0, -11, 0, 0,
    0, -3, 0, -5, -4, -6, 0, -5,
    0, -3, -5, -3, 0, -3, -3, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, -5, 0, 0, 0, -5,
    0, 0, 0, 0, 0, 0, 0, -5,
    -5, -6, 0, 0, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, 0, 0, -36, -26, -25, 0,
    0, -13, -8, -40, -10, 0, 0, 0,
    0, -6, -5, -16, 0, -22, -23, -5,
    -22, 0, 0, -14, -19, -5, -14, -9,
    -9, -10, -9, -22, 0, 0, 0, 0,
    -7, 0, -8, 0, 0, -5, 0, -17,
    -3, 0, 0, -3, 0, -3, -5, 0,
    0, -2, 0, 0, -3, 0, 0, 0,
    -2, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, -22, -6, -12, 0,
    0, -5, -3, -22, -4, 0, -3, 0,
    0, 0, 0, -6, 0, -9, -5, 0,
    -7, 0, 0, -6, -5, 0, -10, -3,
    -3, -5, -3, -7, 0, 0, 0, 0,
    -12, -2, -6, 0, 0, 0, -1, -18,
    -1, 0, 0, 0, 0, 0, 0, -1,
    0, -4, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, -2, 0, -3, 0, -10, 0, 0,
    0, 0, -7, -3, -5, -6, -3, 0,
    0, 0, 0, 0, 0, -3, -4, -7,
    0, 0, 0, 0, 0, -7, -3, -7,
    -5, -3, -7, -5, 0, 0, 0, 0,
    -32, -23, -18, -10, -4, -5, -5, -32,
    -6, -5, -3, 0, 0, 0, 0, -8,
    0, -23, -15, 0, -19, 0, 0, -13,
    -15, -5, -12, -5, -8, -12, -5, -17,
    0, 0, 0, 0, 0, -9, 0, 0,
    0, -1, -6, -9, -10, 0, -3, -1,
    -1, 0, -5, -3, 0, -4, -5, -6,
    -4, 0, 0, 0, 0, -5, -6, -4,
    -4, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -24, -7, -15, -5, 0, -22, 0,
    0, 0, 0, 0, 9, 0, 22, 0,
    0, 0, 0, -6, -1, 0, 3, 0,
    0, 0, 0, -16, 0, 0, 0, 0,
    -7, 0, 0, 0, 0, -8, 0, -7,
    -2, 0, -8, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 1, 0, 0, -7, 0, 0, 0,
    0, -18, 0, -6, 0, -2, -16, 0,
    -10, -4, 0, -1, 0, 0, 0, 0,
    -1, 0, 0, -2, -2, -7, -2, -3,
    0, 0, 0, 0, 0, -9, 0, 0,
    0, 0, 0, 0, 0, -6, 0, -5,
    0, 0, -8, 0, 0, -3, -8, 0,
    -3, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 0, -9,
    0, 1, 0, 0, -4, 0, 0, -6,
    -6, -8, 0, -7, -3, 0, -10, 0,
    -9, -4, 0, -1, -3, 0, 0, 0,
    0, -5, 0, 0, 0, -4, 0, 0,
    3, 13, 13, 0, -18, -5, -3, 0,
    8, 0, 0, 0, 0, 14, 0, 18,
    14, 9, 14, 0, 15, -6, -3, 0,
    -4, 0, -3, 0, -2, 0, -3, 3,
    0, -2, 0, -5, 0, 0, 3, -9,
    0, 0, 12, 0, -13, 0, 0, 0,
    0, -10, 0, 0, 0, 0, -5, 0,
    0, -6, -5, 0, 0, 0, 13, 0,
    0, 0, 0, -2, -2, 0, -2, -5,
    0, 0, 0, -9, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, -8, 0, -3,
    0, 0, -7, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    3, -21, 0, 0, -7, 0, 0, 0,
    0, -12, 0, 0, 0, 0, -4, 0,
    0, -3, -7, 0, -3, 0, -3, 0,
    0, -5, -5, 0, 0, -2, 0, -4,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, -5,
    0, 0, -10, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -23, -8, -9, 0, 0, -6, 0, -21,
    0, 0, 0, 0, 0, 0, 0, -3,
    3, -9, -3, 0, -3, 0, -1, 0,
    -3, 0, 0, 4, 3, 0, 4, -3,
    0, 0, 0, -12, 0, 3, 0, 0,
    -6, 0, 0, 0, 0, -8, 0, -3,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    3, -10, 3, 3, -10, 0, 0, 0,
    0, -5, 0, 0, 0, 0, -2, 0,
    0, -5, -3, 0, -3, 0, 0, 0,
    0, 0, -1, 0, 0, 0, 0, 0,
    0, 0, 0, -2, -15, -4, -6, 0,
    0, -4, 0, -13, 0, -6, 0, -2,
    0, 0, -4, -2, 0, -6, -2, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, -8, 0, 0, 0, -2,
    -20, 0, -2, 0, 0, -2, 0, -10,
    0, -8, 0, -2, 0, -5, -6, 0,
    0, -3, -2, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, -7,
    0, 0, 0, -3, 0, -5, 0, 0,
    0, 0, -3, 0, -2, -9, 0, -4,
    0, -3, -8, 0, 0, -3, -7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -5, 0, 0, 0, 0, 0,
    -15, 0, -3, 0, 0, 0, 0, -13,
    0, -6, 0, -2, 0, -2, -4, 0,
    0, -6, -2, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, -8,
    0, 0, 0, 0, 0, -7, 0, 0,
    0, 0, 0, 0, 0, -8, 0, 0,
    0, 0, -2, 0, 0, -6, -3, 0,
    -1, 0, 0, 0, 0, 0, -3, -2,
    0, 0, -2, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 43,
    .right_class_cnt     = 36,
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
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 1,
    .kern_classes = 1,
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
const lv_font_t ui_font_Font3 = {
#else
lv_font_t ui_font_Font3 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 23,          /*The maximum line height required by the font*/
    .base_line = 6,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if UI_FONT_FONT3*/

