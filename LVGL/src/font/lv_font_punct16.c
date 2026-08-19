/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --font C:/Windows/Fonts/simhei.ttf --size 16 --bpp 1 --range 0x20-0x7F,0x2000-0x206F,0x3000-0x303F,0xFF00-0xFFEF --format lvgl --lv-font-name lv_font_punct16 -o LVGL\src\font\lv_font_punct16.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_FONT_PUNCT16
#define LV_FONT_PUNCT16 1
#endif

#if LV_FONT_PUNCT16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xb0,

    /* U+0022 "\"" */
    0xbb, 0xb0,

    /* U+0023 "#" */
    0x24, 0x48, 0x91, 0x2f, 0xe4, 0x8a, 0x24, 0xfe,
    0x91, 0x22, 0x40,

    /* U+0024 "$" */
    0x11, 0xed, 0x65, 0x93, 0xc3, 0x86, 0x16, 0x59,
    0x75, 0x78, 0x41, 0x0,

    /* U+0025 "%" */
    0x45, 0x4a, 0xa5, 0x4a, 0x8a, 0x5, 0x15, 0x2a,
    0x95, 0x2a, 0x20,

    /* U+0026 "&" */
    0x30, 0x48, 0x48, 0x48, 0x50, 0x60, 0x60, 0xb2,
    0x9e, 0x8e, 0xce, 0x7a,

    /* U+0027 "'" */
    0xe0,

    /* U+0028 "(" */
    0x32, 0x44, 0x88, 0x88, 0x88, 0x44, 0x23,

    /* U+0029 ")" */
    0xc4, 0x22, 0x11, 0x11, 0x11, 0x22, 0x4c,

    /* U+002A "*" */
    0x11, 0x27, 0x59, 0xc7, 0xda, 0xc4, 0x8,

    /* U+002B "+" */
    0x10, 0x20, 0x47, 0xf1, 0x2, 0x4, 0x0,

    /* U+002C "," */
    0xe0,

    /* U+002D "-" */
    0xfe,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x0, 0x2, 0x2, 0x4, 0x4, 0x8, 0x8, 0x10,
    0x20, 0x20, 0x40, 0x40, 0x80,

    /* U+0030 "0" */
    0x38, 0xc9, 0xa, 0x1c, 0x38, 0x50, 0xa1, 0x42,
    0xcc, 0xf0, 0xc0,

    /* U+0031 "1" */
    0x2f, 0xd2, 0x49, 0x24, 0x90,

    /* U+0032 "2" */
    0x7b, 0xb8, 0x41, 0xc, 0x21, 0x84, 0x31, 0x8c,
    0x3f,

    /* U+0033 "3" */
    0x0, 0x79, 0x1a, 0x30, 0x61, 0x87, 0x3, 0x3,
    0x8d, 0x11, 0xc0,

    /* U+0034 "4" */
    0x4, 0xc, 0xc, 0x14, 0x34, 0x24, 0x44, 0xc4,
    0xff, 0x4, 0x4, 0x4,

    /* U+0035 "5" */
    0x3e, 0x40, 0x83, 0x7, 0xcc, 0x80, 0x81, 0x2,
    0x85, 0xb1, 0xc0,

    /* U+0036 "6" */
    0x8, 0x30, 0xc1, 0x7, 0x8f, 0xb1, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0037 "7" */
    0xfe, 0xc, 0x10, 0x60, 0x83, 0x4, 0x8, 0x30,
    0x40, 0x81, 0x0,

    /* U+0038 "8" */
    0x79, 0x9a, 0x14, 0x2c, 0xcf, 0x33, 0x43, 0x87,
    0xf, 0xf3, 0xe0,

    /* U+0039 "9" */
    0x38, 0x8b, 0xe, 0x1c, 0x78, 0xdf, 0x6, 0x8,
    0x30, 0x41, 0x80,

    /* U+003A ":" */
    0x81,

    /* U+003B ";" */
    0x81, 0xc0,

    /* U+003C "<" */
    0x2, 0x8, 0x20, 0x82, 0x8, 0x10, 0x10, 0x10,
    0x10, 0x18, 0x10,

    /* U+003D "=" */
    0xfe, 0x0, 0x0, 0xf, 0xe0,

    /* U+003E ">" */
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x86, 0x18,
    0x61, 0x82, 0x0,

    /* U+003F "?" */
    0x7b, 0x38, 0x61, 0xc, 0x63, 0x8, 0x20, 0x0,
    0x8,

    /* U+0040 "@" */
    0x38, 0xc9, 0x6d, 0x5c, 0xb9, 0x74, 0xe9, 0xad,
    0x81, 0x21, 0xc0,

    /* U+0041 "A" */
    0x18, 0x18, 0x18, 0x38, 0x2c, 0x24, 0x24, 0x7e,
    0x46, 0x42, 0x42, 0xc3,

    /* U+0042 "B" */
    0xfa, 0x38, 0x61, 0x8f, 0xe8, 0xe1, 0x86, 0x18,
    0xfe,

    /* U+0043 "C" */
    0x38, 0x89, 0xc, 0x18, 0x10, 0x20, 0x41, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0044 "D" */
    0xf2, 0x28, 0xe1, 0x86, 0x18, 0x61, 0x86, 0x38,
    0xbc,

    /* U+0045 "E" */
    0xfe, 0x8, 0x20, 0x83, 0xe8, 0x20, 0x82, 0x8,
    0x3f,

    /* U+0046 "F" */
    0xfe, 0x8, 0x20, 0x82, 0xf, 0xa0, 0x82, 0x8,
    0x20,

    /* U+0047 "G" */
    0x39, 0x28, 0x61, 0x82, 0x9, 0xe1, 0x86, 0x14,
    0xcf,

    /* U+0048 "H" */
    0x86, 0x18, 0x61, 0x87, 0xf8, 0x61, 0x86, 0x18,
    0x61,

    /* U+0049 "I" */
    0xff, 0xf0,

    /* U+004A "J" */
    0x4, 0x10, 0x41, 0x4, 0x10, 0x41, 0x86, 0x1c,
    0xde,

    /* U+004B "K" */
    0x8d, 0x12, 0x45, 0x8e, 0x1c, 0x3c, 0x48, 0x99,
    0x12, 0x34, 0x20,

    /* U+004C "L" */
    0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x82, 0x8,
    0x3f,

    /* U+004D "M" */
    0xc7, 0x8f, 0x1f, 0x7e, 0xf5, 0x6a, 0xd5, 0xbb,
    0x76, 0x4c, 0x90,

    /* U+004E "N" */
    0x87, 0x1c, 0x79, 0xa6, 0xdb, 0x65, 0x9e, 0x38,
    0xe1,

    /* U+004F "O" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0050 "P" */
    0xfa, 0x38, 0x61, 0x86, 0x3f, 0xa0, 0x82, 0x8,
    0x20,

    /* U+0051 "Q" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xc5, 0x8b,
    0x1d, 0x19, 0xf0, 0x0,

    /* U+0052 "R" */
    0xfa, 0x38, 0x61, 0x86, 0x3f, 0xa6, 0x8a, 0x28,
    0xe1,

    /* U+0053 "S" */
    0x7a, 0x28, 0x60, 0xc1, 0xc1, 0x83, 0x86, 0x1c,
    0xde,

    /* U+0054 "T" */
    0xfc, 0x41, 0x4, 0x10, 0x41, 0x4, 0x10, 0x41,
    0x4,

    /* U+0055 "U" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0x61, 0x86, 0x1c,
    0xde,

    /* U+0056 "V" */
    0xc3, 0x42, 0x42, 0x46, 0x64, 0x24, 0x24, 0x2c,
    0x38, 0x18, 0x18, 0x18,

    /* U+0057 "W" */
    0x9b, 0xdb, 0xda, 0xda, 0x5a, 0x5a, 0x6a, 0x6a,
    0x66, 0x66, 0x64, 0x64,

    /* U+0058 "X" */
    0x46, 0x66, 0x24, 0x3c, 0x18, 0x18, 0x18, 0x38,
    0x2c, 0x64, 0x46, 0xc2,

    /* U+0059 "Y" */
    0xc6, 0x89, 0x11, 0x42, 0x83, 0x4, 0x8, 0x10,
    0x20, 0x40, 0x80,

    /* U+005A "Z" */
    0xfc, 0x30, 0x86, 0x10, 0x42, 0x8, 0x41, 0x8,
    0x3f,

    /* U+005B "[" */
    0xf8, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0xf0,

    /* U+005C "\\" */
    0x82, 0x4, 0x10, 0x60, 0x82, 0x4, 0x10, 0x40,
    0x82, 0xc, 0x10,

    /* U+005D "]" */
    0xf1, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0xf0,

    /* U+005E "^" */
    0x31, 0xec, 0xc0,

    /* U+005F "_" */
    0xff,

    /* U+0060 "`" */
    0xc6, 0x20,

    /* U+0061 "a" */
    0x79, 0x98, 0x33, 0xee, 0x50, 0xa3, 0x3a,

    /* U+0062 "b" */
    0x82, 0x8, 0x20, 0xfb, 0x28, 0x61, 0x86, 0x1c,
    0xbe,

    /* U+0063 "c" */
    0x39, 0x38, 0x60, 0x82, 0x14, 0xce,

    /* U+0064 "d" */
    0x4, 0x10, 0x41, 0x7d, 0x38, 0x61, 0x86, 0x1c,
    0xdf,

    /* U+0065 "e" */
    0x38, 0x9a, 0x17, 0xe8, 0x10, 0xd1, 0x1c,

    /* U+0066 "f" */
    0x1e, 0x40, 0x81, 0xf, 0xc4, 0x8, 0x10, 0x20,
    0x40, 0x81, 0x0,

    /* U+0067 "g" */
    0x36, 0x91, 0x22, 0x47, 0x8, 0x10, 0x3e, 0x85,
    0xf0,

    /* U+0068 "h" */
    0x82, 0x8, 0x20, 0xbb, 0x18, 0x61, 0x86, 0x18,
    0x61,

    /* U+0069 "i" */
    0xcf, 0xf0,

    /* U+006A "j" */
    0x11, 0x0, 0x11, 0x11, 0x11, 0x11, 0x1e,

    /* U+006B "k" */
    0x82, 0x8, 0x20, 0x8a, 0x4a, 0x3c, 0xd2, 0x68,
    0xa3,

    /* U+006C "l" */
    0xff, 0xf0,

    /* U+006D "m" */
    0xff, 0x26, 0x4c, 0x99, 0x32, 0x64, 0xc9,

    /* U+006E "n" */
    0xbb, 0x18, 0x61, 0x86, 0x18, 0x61,

    /* U+006F "o" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x51, 0x1c,

    /* U+0070 "p" */
    0xfb, 0x28, 0x61, 0x86, 0x1c, 0xbe, 0x82, 0x0,

    /* U+0071 "q" */
    0x7f, 0x38, 0x61, 0x86, 0x14, 0xdf, 0x4, 0x10,

    /* U+0072 "r" */
    0xbe, 0xc8, 0x88, 0x88,

    /* U+0073 "s" */
    0x7a, 0x18, 0x3c, 0x3c, 0x18, 0x5e,

    /* U+0074 "t" */
    0x20, 0x40, 0x87, 0xe2, 0x4, 0x8, 0x10, 0x20,
    0x44, 0x78,

    /* U+0075 "u" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0xdd,

    /* U+0076 "v" */
    0x86, 0x3c, 0x92, 0x59, 0xc3, 0xc,

    /* U+0077 "w" */
    0x9b, 0xda, 0x5a, 0x5a, 0x6a, 0x6a, 0x64, 0x24,

    /* U+0078 "x" */
    0xcd, 0x27, 0x8c, 0x31, 0xec, 0xa3,

    /* U+0079 "y" */
    0xc6, 0x89, 0x13, 0x62, 0x85, 0xe, 0x8, 0x10,
    0xc0,

    /* U+007A "z" */
    0xfc, 0x31, 0x84, 0x21, 0x8c, 0x3f,

    /* U+007B "{" */
    0x69, 0x24, 0x96, 0x49, 0x24, 0x98,

    /* U+007C "|" */
    0xff, 0xff, 0x80,

    /* U+007D "}" */
    0xc9, 0x24, 0x93, 0x49, 0x24, 0xb0,

    /* U+007E "~" */
    0x62, 0x58, 0x80,

    /* U+2010 "‐" */
    0xf0,

    /* U+2013 "–" */
    0xfc,

    /* U+2014 "—" */
    0xff, 0xff,

    /* U+2015 "―" */
    0xff, 0xf8,

    /* U+2016 "‖" */
    0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
    0x90,

    /* U+2018 "‘" */
    0x6b, 0xc0,

    /* U+2019 "’" */
    0xf5, 0x80,

    /* U+201C "“" */
    0x46, 0x28, 0xb3, 0xcc,

    /* U+201D "”" */
    0xcf, 0x34, 0x51, 0x88,

    /* U+2025 "‥" */
    0xc3, 0xc3,

    /* U+2026 "…" */
    0xc6, 0x1e, 0x30, 0xc0,

    /* U+2030 "‰" */
    0x0, 0x41, 0x81, 0x9, 0x8, 0x24, 0x40, 0x91,
    0x2, 0x48, 0x7, 0x40, 0x1, 0x0, 0xb, 0x18,
    0x52, 0x91, 0x4a, 0x49, 0x29, 0x44, 0xa6, 0x1e,
    0xf0, 0x0, 0x0,

    /* U+2032 "′" */
    0x22, 0x64, 0x40,

    /* U+2033 "″" */
    0x20, 0xa4, 0x92, 0x92, 0x40,

    /* U+2035 "‵" */
    0x49, 0x22,

    /* U+203B "※" */
    0x86, 0x14, 0xf2, 0x26, 0x41, 0x8, 0x9, 0x8e,
    0x77, 0xe6, 0x7e, 0x57, 0x9, 0x81, 0xc, 0x26,
    0x44, 0xf2, 0x86, 0x10,

    /* U+3000 "　" */
    0x0,

    /* U+3001 "、" */
    0xc4, 0x63, 0x20,

    /* U+3002 "。" */
    0xf, 0x9e,

    /* U+3003 "〃" */
    0x20, 0xa4, 0x96, 0x90, 0x80,

    /* U+3005 "々" */
    0x8, 0x1, 0x80, 0x60, 0xf, 0xf3, 0x6, 0x40,
    0x98, 0x22, 0x6c, 0x7, 0x0, 0x60, 0x6, 0x0,

    /* U+3006 "〆" */
    0x0, 0x20, 0x6, 0x0, 0x40, 0x8, 0x1d, 0x2,
    0x60, 0x47, 0x4, 0x80, 0x58, 0x7, 0x0, 0xe0,
    0xc, 0x0, 0x0, 0x0,

    /* U+3007 "〇" */
    0xf, 0x83, 0xc, 0x60, 0x24, 0x2, 0x80, 0x18,
    0x1, 0x80, 0x18, 0x1, 0xc0, 0x34, 0x2, 0x30,
    0xc1, 0xf8,

    /* U+3008 "〈" */
    0x8, 0x44, 0x42, 0x23, 0x10, 0x42, 0x8, 0x21,
    0x4, 0x0,

    /* U+3009 "〉" */
    0x41, 0x2, 0x4, 0x10, 0x20, 0x81, 0x8, 0x61,
    0x8, 0x21, 0x0, 0x0,

    /* U+300A "《" */
    0x12, 0x28, 0x91, 0x44, 0x8a, 0x24, 0x48, 0x50,
    0xa0, 0xa1, 0x42, 0x42, 0x84, 0x80,

    /* U+300B "》" */
    0x90, 0xa1, 0x21, 0x42, 0x42, 0x84, 0x89, 0x14,
    0x48, 0xa2, 0x45, 0x12, 0x20, 0x0,

    /* U+300C "「" */
    0xfc, 0x21, 0x8, 0x42, 0x10, 0x84, 0x21, 0x8,
    0x40,

    /* U+300D "」" */
    0x8, 0x42, 0x10, 0x84, 0x21, 0x8, 0x42, 0x10,
    0xfc,

    /* U+300E "『" */
    0xfd, 0xf1, 0x8c, 0x63, 0x18, 0xc6, 0x31, 0x8c,
    0x0,

    /* U+300F "』" */
    0x1c, 0x51, 0x45, 0x14, 0x51, 0x45, 0x14, 0x51,
    0x7d, 0x87, 0xf0,

    /* U+3010 "【" */
    0xff, 0xef, 0x3c, 0xe3, 0x8e, 0x38, 0xe3, 0xcf,
    0x3e, 0xfc,

    /* U+3011 "】" */
    0xfd, 0xf3, 0xcf, 0x1c, 0x71, 0xc7, 0x1c, 0xf3,
    0xdf, 0xfc,

    /* U+3012 "〒" */
    0xff, 0xfc, 0x0, 0x0, 0x0, 0x3f, 0xff, 0x3,
    0x0, 0xc, 0x0, 0x30, 0x0, 0xc0, 0x3, 0x0,
    0xc, 0x0, 0x30, 0x0, 0xc0, 0x3, 0x0,

    /* U+3013 "〓" */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xf0,

    /* U+3014 "〔" */
    0x36, 0x88, 0x88, 0x88, 0x88, 0x88, 0xc3, 0x10,

    /* U+3015 "〕" */
    0xe1, 0x82, 0x10, 0x84, 0x21, 0x8, 0x42, 0x11,
    0xb0, 0x0,

    /* U+3016 "〖" */
    0xfd, 0xa, 0x24, 0x89, 0x14, 0x28, 0x50, 0x91,
    0x22, 0x24, 0x4f, 0xc0,

    /* U+3017 "〗" */
    0x7e, 0x84, 0x88, 0x91, 0x22, 0x44, 0x89, 0x12,
    0x24, 0x8a, 0x17, 0xe0,

    /* U+301D "〝" */
    0x12, 0x44, 0x92, 0x24, 0x80,

    /* U+301E "〞" */
    0x20, 0xa4, 0x96, 0x90, 0x80,

    /* U+3021 "〡" */
    0xaa, 0xaa, 0xaa, 0xa0,

    /* U+3022 "〢" */
    0x1, 0x1, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x1, 0x1,

    /* U+3023 "〣" */
    0x0, 0x30, 0x6, 0x10, 0xc2, 0x18, 0x43, 0x8,
    0x61, 0xc, 0x21, 0x84, 0x30, 0x86, 0x10, 0xc2,
    0x18, 0x43, 0x0, 0x40, 0x8,

    /* U+3024 "〤" */
    0x0, 0x0, 0x20, 0x8, 0x6, 0xc1, 0x18, 0x81,
    0xa0, 0x30, 0xe, 0x6, 0xc3, 0x19, 0x83, 0xc0,
    0x20, 0x0,

    /* U+3025 "〥" */
    0x0, 0x23, 0xc, 0xc3, 0x18, 0xc1, 0xe0, 0x1c,
    0x4, 0x41, 0x4, 0x40, 0x48, 0x9, 0x1, 0x20,
    0x43, 0x18, 0x3c, 0x0,

    /* U+3026 "〦" */
    0x2, 0x0, 0x10, 0x0, 0x81, 0xff, 0xf0,

    /* U+3027 "〧" */
    0x2, 0x0, 0x10, 0x0, 0x81, 0xff, 0xf0, 0x0,
    0x7f, 0xfc,

    /* U+3028 "〨" */
    0x2, 0x0, 0x10, 0x0, 0x81, 0xff, 0xf0, 0x0,
    0x7f, 0xfc, 0x0, 0x0, 0x0, 0xff, 0xf8,

    /* U+3029 "〩" */
    0x0, 0x0, 0xc0, 0x8, 0x1, 0x80, 0x1f, 0xf2,
    0x6, 0x60, 0x45, 0x8c, 0xd, 0x80, 0x30, 0x3,
    0x80, 0xcc, 0x18, 0x66, 0x0, 0x80, 0x0,

    /* U+303E "〾" */
    0xeb, 0xf8, 0x0, 0x20, 0x3, 0x11, 0x11, 0x70,
    0x40, 0x6, 0x7e, 0x30, 0x81, 0x8b, 0x8, 0x0,
    0x20, 0x2, 0x0, 0xe, 0xbf, 0x80,

    /* U+FF01 "！" */
    0xfe, 0x60,

    /* U+FF02 "＂" */
    0x99, 0x99,

    /* U+FF03 "＃" */
    0x11, 0x4, 0x47, 0xfc, 0x48, 0x12, 0x4, 0x81,
    0x23, 0xfe, 0x22, 0x8, 0x82, 0x60,

    /* U+FF04 "＄" */
    0x8, 0xf, 0x1a, 0xc9, 0x4, 0x83, 0x40, 0xf8,
    0x12, 0x8, 0xc4, 0x5a, 0x47, 0xc0, 0x80,

    /* U+FF05 "％" */
    0x70, 0x18, 0x82, 0x88, 0x48, 0x98, 0xdb, 0x2,
    0x60, 0xc, 0xf1, 0x91, 0x31, 0x16, 0x11, 0xc0,
    0xe0,

    /* U+FF06 "＆" */
    0x1c, 0x4, 0xc0, 0x88, 0x11, 0x2, 0xc0, 0x73,
    0x96, 0x24, 0x48, 0x85, 0x10, 0xc3, 0x37, 0x38,
    0x40, 0x0,

    /* U+FF07 "＇" */
    0xfe,

    /* U+FF08 "（" */
    0x11, 0x88, 0x8c, 0x42, 0x10, 0x84, 0x10, 0x43,
    0x8,

    /* U+FF09 "）" */
    0x8c, 0x42, 0x31, 0x11, 0x11, 0x24, 0xc8,

    /* U+FF0A "＊" */
    0xc, 0x3, 0x0, 0xc3, 0x23, 0xfb, 0xc3, 0x1,
    0x20, 0xcc, 0x61, 0x88, 0x0,

    /* U+FF0B "＋" */
    0x6, 0x0, 0x60, 0x6, 0x0, 0x60, 0x6, 0x0,
    0x60, 0xff, 0xf0, 0x60, 0x6, 0x0, 0x60, 0x6,
    0x0, 0x60,

    /* U+FF0C "，" */
    0xf7,

    /* U+FF0D "－" */
    0xff, 0xf0,

    /* U+FF0E "．" */
    0xf0,

    /* U+FF0F "／" */
    0x4, 0x10, 0x82, 0x10, 0x43, 0x8, 0x21, 0x4,
    0x20, 0x80,

    /* U+FF10 "０" */
    0x3e, 0x31, 0xb0, 0x50, 0x18, 0xc, 0x6, 0x3,
    0x1, 0xc1, 0x31, 0x8f, 0x80,

    /* U+FF11 "１" */
    0x3c, 0x92, 0x49, 0x24, 0x80,

    /* U+FF12 "２" */
    0x3e, 0x30, 0x90, 0x20, 0x10, 0x18, 0x8, 0x18,
    0x18, 0x38, 0x30, 0x3f, 0xe0,

    /* U+FF13 "３" */
    0x3e, 0x30, 0xf0, 0x20, 0x10, 0x38, 0xf8, 0x6,
    0x1, 0x80, 0xe0, 0x8f, 0x80,

    /* U+FF14 "４" */
    0x2, 0x3, 0x3, 0x83, 0x43, 0x21, 0x11, 0x9,
    0x84, 0xff, 0x81, 0x0, 0x80,

    /* U+FF15 "５" */
    0x7f, 0x90, 0x4, 0x1, 0x0, 0x7f, 0x90, 0x30,
    0x4, 0x1, 0xc0, 0x58, 0x63, 0xf0,

    /* U+FF16 "６" */
    0x1f, 0xc, 0x26, 0x5, 0x0, 0x5f, 0x1c, 0x64,
    0x5, 0x1, 0x60, 0x4c, 0x61, 0xf0,

    /* U+FF17 "７" */
    0xff, 0xc0, 0x30, 0x8, 0x6, 0x1, 0x0, 0xc0,
    0x20, 0x18, 0x4, 0x3, 0x0, 0x80,

    /* U+FF18 "８" */
    0x3e, 0x21, 0xe0, 0x70, 0x34, 0x1b, 0xf3, 0x7,
    0x1, 0x80, 0xa0, 0x8f, 0x80,

    /* U+FF19 "９" */
    0x7e, 0x61, 0xa0, 0x50, 0x38, 0x1e, 0x1d, 0xf2,
    0x1, 0x81, 0xe0, 0x8f, 0x80,

    /* U+FF1A "：" */
    0xf0, 0xf0,

    /* U+FF1B "；" */
    0xf0, 0xf7,

    /* U+FF1C "＜" */
    0x0, 0x10, 0x6, 0x3, 0x80, 0xc0, 0x70, 0xe,
    0x0, 0x18, 0x0, 0x70, 0x0, 0xe0, 0x3,

    /* U+FF1D "＝" */
    0xff, 0xf0, 0x0, 0x0, 0xf, 0xff,

    /* U+FF1E "＞" */
    0x80, 0xe, 0x0, 0x1c, 0x0, 0x70, 0x0, 0xe0,
    0x7, 0x1, 0x80, 0xe0, 0x38, 0xc, 0x0,

    /* U+FF1F "？" */
    0x33, 0xe8, 0x82, 0x18, 0x42, 0x8, 0x60, 0x6,
    0x18,

    /* U+FF20 "＠" */
    0xf, 0x83, 0xc, 0x42, 0x24, 0xf9, 0x98, 0x99,
    0x9, 0xb1, 0x9b, 0x12, 0x93, 0x25, 0xdc, 0x60,
    0x63, 0x9c, 0x4, 0x0,

    /* U+FF21 "Ａ" */
    0xc, 0x3, 0x1, 0xe0, 0x48, 0x12, 0xc, 0xc2,
    0x11, 0xfc, 0x41, 0x90, 0x2c, 0xc,

    /* U+FF22 "Ｂ" */
    0xff, 0x40, 0xa0, 0x30, 0x38, 0x17, 0xfa, 0x7,
    0x1, 0x80, 0xc0, 0xff, 0xc0,

    /* U+FF23 "Ｃ" */
    0x1e, 0x18, 0x64, 0xa, 0x0, 0x80, 0x20, 0x8,
    0x2, 0x3, 0x40, 0x98, 0x41, 0xe0,

    /* U+FF24 "Ｄ" */
    0xfe, 0x20, 0x68, 0xa, 0x1, 0x80, 0x60, 0x18,
    0x6, 0x1, 0x80, 0xa0, 0x6f, 0xe0,

    /* U+FF25 "Ｅ" */
    0xff, 0x80, 0x80, 0x80, 0x80, 0xfc, 0x80, 0x80,
    0x80, 0x80, 0xff,

    /* U+FF26 "Ｆ" */
    0xff, 0xc0, 0x20, 0x10, 0x8, 0x7, 0xf2, 0x1,
    0x0, 0x80, 0x40, 0x20, 0x0,

    /* U+FF27 "Ｇ" */
    0x1f, 0x8, 0x64, 0xf, 0x0, 0x80, 0x20, 0x8,
    0x7e, 0x1, 0x40, 0x58, 0x33, 0xfc, 0x30,

    /* U+FF28 "Ｈ" */
    0x80, 0xc0, 0x60, 0x30, 0x1f, 0xfc, 0x6, 0x3,
    0x1, 0x80, 0xc0, 0x40,

    /* U+FF29 "Ｉ" */
    0xff, 0xc0,

    /* U+FF2A "Ｊ" */
    0x0, 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x3,
    0x81, 0x40, 0xb0, 0xcf, 0xc0,

    /* U+FF2B "Ｋ" */
    0x83, 0x43, 0x23, 0x13, 0xb, 0x7, 0x43, 0x31,
    0xc, 0x83, 0x40, 0x80,

    /* U+FF2C "Ｌ" */
    0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1,
    0x0, 0x80, 0x40, 0x3f, 0xe0,

    /* U+FF2D "Ｍ" */
    0xc0, 0x78, 0xe, 0x82, 0xd0, 0x59, 0x13, 0x22,
    0x62, 0xcc, 0x51, 0x8e, 0x30, 0x84,

    /* U+FF2E "Ｎ" */
    0xc0, 0xf0, 0x68, 0x32, 0x19, 0x8c, 0x66, 0x13,
    0xd, 0x83, 0xc0, 0xe0, 0x20,

    /* U+FF2F "Ｏ" */
    0x1e, 0x18, 0x64, 0xa, 0x1, 0x80, 0x60, 0x18,
    0x7, 0x3, 0x40, 0x9c, 0xc1, 0xe0,

    /* U+FF30 "Ｐ" */
    0xff, 0x40, 0xe0, 0x30, 0x18, 0xc, 0xb, 0xf9,
    0x0, 0x80, 0x40, 0x20, 0x0,

    /* U+FF31 "Ｑ" */
    0x3e, 0x8, 0x21, 0x2, 0x40, 0x48, 0xd, 0x1,
    0xa0, 0x34, 0x64, 0x47, 0x8e, 0x60, 0x76, 0x0,
    0x40,

    /* U+FF32 "Ｒ" */
    0xff, 0x40, 0xe0, 0x30, 0x18, 0xc, 0xf, 0xf9,
    0x4, 0x83, 0x40, 0xe0, 0x20,

    /* U+FF33 "Ｓ" */
    0x1f, 0x8, 0x64, 0x1, 0x0, 0x38, 0x7, 0xc0,
    0x18, 0x2, 0x40, 0x98, 0x63, 0xf0,

    /* U+FF34 "Ｔ" */
    0xff, 0xc3, 0x0, 0xc0, 0x30, 0xc, 0x3, 0x0,
    0xc0, 0x30, 0xc, 0x3, 0x0, 0xc0,

    /* U+FF35 "Ｕ" */
    0x80, 0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x3,
    0x1, 0x81, 0xa1, 0x8f, 0x80,

    /* U+FF36 "Ｖ" */
    0x80, 0x70, 0x24, 0x9, 0x6, 0x21, 0x8, 0xc3,
    0x20, 0x58, 0x14, 0x7, 0x0, 0x80,

    /* U+FF37 "Ｗ" */
    0xc2, 0xd, 0xc, 0x24, 0x30, 0x90, 0xc6, 0x25,
    0x90, 0x92, 0x42, 0x49, 0x7, 0x38, 0x18, 0x60,
    0x61, 0x80, 0x84, 0x0,

    /* U+FF38 "Ｘ" */
    0x40, 0x98, 0x43, 0x30, 0x78, 0xc, 0x3, 0x1,
    0x60, 0xcc, 0x61, 0xb0, 0x20,

    /* U+FF39 "Ｙ" */
    0x80, 0xe0, 0xd8, 0xc4, 0x43, 0x60, 0xa0, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x0,

    /* U+FF3A "Ｚ" */
    0xff, 0x0, 0x80, 0x80, 0xc0, 0xc0, 0xc0, 0x40,
    0x60, 0x60, 0x60, 0x3f, 0xe0,

    /* U+FF3B "［" */
    0xfc, 0x21, 0x8, 0x42, 0x10, 0x84, 0x21, 0x8,
    0x7c,

    /* U+FF3C "＼" */
    0x80, 0x60, 0x10, 0xc, 0x2, 0x0, 0x80, 0x60,
    0x10, 0xc, 0x2, 0x0, 0x80, 0x40, 0x10, 0xc,

    /* U+FF3D "］" */
    0xf8, 0x42, 0x10, 0x84, 0x21, 0x8, 0x42, 0x10,
    0x87, 0xe0,

    /* U+FF3E "＾" */
    0x31, 0xe8, 0xc0,

    /* U+FF3F "＿" */
    0xff, 0xff,

    /* U+FF40 "｀" */
    0x13, 0x24, 0x80,

    /* U+FF41 "ａ" */
    0x10, 0x6c, 0x2, 0x2, 0x7e, 0xc2, 0x82, 0x6d,
    0x10,

    /* U+FF42 "ｂ" */
    0x81, 0x2, 0x7, 0xec, 0x70, 0x60, 0xc1, 0xc7,
    0xf8, 0x0,

    /* U+FF43 "ｃ" */
    0x3c, 0x42, 0x40, 0x80, 0x80, 0x42, 0x46, 0x3c,

    /* U+FF44 "ｄ" */
    0x2, 0x4, 0x9, 0xd4, 0xf8, 0xe0, 0xc1, 0x87,
    0x8d, 0xe8,

    /* U+FF45 "ｅ" */
    0x3c, 0x42, 0x42, 0xff, 0xc0, 0x42, 0x7e,

    /* U+FF46 "ｆ" */
    0x9, 0x88, 0x4f, 0x90, 0x84, 0x21, 0x8,

    /* U+FF47 "ｇ" */
    0x3, 0x3c, 0x46, 0x42, 0x42, 0x3c, 0x40, 0x3e,
    0x41, 0x81, 0x43, 0x3c,

    /* U+FF48 "ｈ" */
    0x82, 0x8, 0x2e, 0xc6, 0x18, 0x61, 0x86, 0x10,

    /* U+FF49 "ｉ" */
    0x8f, 0xe0,

    /* U+FF4A "ｊ" */
    0x8, 0x0, 0x10, 0x84, 0x21, 0x8, 0x43, 0x17,
    0x0,

    /* U+FF4B "ｋ" */
    0x81, 0x2, 0x4, 0x29, 0x96, 0x34, 0x44, 0x8d,
    0xc,

    /* U+FF4C "ｌ" */
    0xff, 0xc0,

    /* U+FF4D "ｍ" */
    0x0, 0x3e, 0xec, 0x46, 0x11, 0x84, 0x61, 0x18,
    0x46, 0x11,

    /* U+FF4E "ｎ" */
    0xfd, 0x86, 0xc, 0x18, 0x30, 0x60, 0x80,

    /* U+FF4F "ｏ" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x51, 0x1c,

    /* U+FF50 "ｐ" */
    0xfd, 0x8e, 0xc, 0x18, 0x38, 0xbf, 0x40, 0x80,

    /* U+FF51 "ｑ" */
    0x7f, 0x8e, 0xc, 0x18, 0x38, 0xdf, 0x81, 0x2,

    /* U+FF52 "ｒ" */
    0xfe, 0x61, 0x8, 0x42, 0x0,

    /* U+FF53 "ｓ" */
    0x3c, 0x81, 0x1, 0xc0, 0x60, 0x51, 0x9c,

    /* U+FF54 "ｔ" */
    0x20, 0x20, 0xfc, 0x20, 0x20, 0x20, 0x20, 0x23,
    0x1e, 0x8,

    /* U+FF55 "ｕ" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x61, 0xbd,

    /* U+FF56 "ｖ" */
    0x42, 0x42, 0x66, 0x24, 0x2c, 0x18, 0x18,

    /* U+FF57 "ｗ" */
    0xcc, 0x53, 0x24, 0xc9, 0x4a, 0x33, 0xc, 0xc2,
    0x10,

    /* U+FF58 "ｘ" */
    0x42, 0x48, 0x60, 0xc3, 0x84, 0x90, 0x80,

    /* U+FF59 "ｙ" */
    0xc2, 0x84, 0x91, 0x21, 0x43, 0x2, 0xc, 0x10,
    0xc0,

    /* U+FF5A "ｚ" */
    0xfe, 0x8, 0x20, 0x82, 0x8, 0x3f, 0x80,

    /* U+FF5B "｛" */
    0x74, 0x44, 0x44, 0xcc, 0x44, 0x44, 0x47,

    /* U+FF5C "｜" */
    0xff, 0xf8,

    /* U+FF5D "｝" */
    0xc4, 0x44, 0x44, 0x37, 0x44, 0x44, 0x4c,

    /* U+FF5E "～" */
    0x78, 0x1, 0x18, 0x18, 0x18, 0x80, 0x3c,

    /* U+FFE0 "￠" */
    0x0, 0x0, 0xc1, 0xf1, 0xce, 0x46, 0xb1, 0x8,
    0xc2, 0x20, 0x88, 0x34, 0x35, 0x19, 0xfc, 0x3c,
    0x18, 0x0,

    /* U+FFE1 "￡" */
    0xf, 0x83, 0x18, 0x41, 0x8, 0x1, 0x0, 0x20,
    0x3f, 0xc0, 0x80, 0x70, 0x16, 0x6, 0x71, 0xf9,
    0xe0, 0x0,

    /* U+FFE2 "￢" */
    0xff, 0xf8, 0x0, 0x40, 0x2,

    /* U+FFE3 "￣" */
    0xff, 0xff,

    /* U+FFE4 "￤" */
    0xf8, 0xf8,

    /* U+FFE5 "￥" */
    0x40, 0xc8, 0x41, 0x30, 0x38, 0xc, 0x1f, 0xe0,
    0xc1, 0xfe, 0xc, 0x3, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 128, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 128, .box_w = 1, .box_h = 12, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 128, .box_w = 4, .box_h = 3, .ofs_x = 2, .ofs_y = 9},
    {.bitmap_index = 5, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 16, .adv_w = 128, .box_w = 6, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 28, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 39, .adv_w = 128, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 51, .adv_w = 128, .box_w = 1, .box_h = 3, .ofs_x = 3, .ofs_y = 9},
    {.bitmap_index = 52, .adv_w = 128, .box_w = 4, .box_h = 14, .ofs_x = 4, .ofs_y = -1},
    {.bitmap_index = 59, .adv_w = 128, .box_w = 4, .box_h = 14, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 66, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 73, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 80, .adv_w = 128, .box_w = 1, .box_h = 3, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 81, .adv_w = 128, .box_w = 7, .box_h = 1, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 82, .adv_w = 128, .box_w = 1, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 83, .adv_w = 128, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 96, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 128, .box_w = 3, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 112, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 121, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 132, .adv_w = 128, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 144, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 155, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 166, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 177, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 188, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 128, .box_w = 1, .box_h = 8, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 200, .adv_w = 128, .box_w = 1, .box_h = 10, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 202, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 213, .adv_w = 128, .box_w = 7, .box_h = 5, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 218, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 229, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 238, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 249, .adv_w = 128, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 261, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 270, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 281, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 290, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 299, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 308, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 317, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 326, .adv_w = 128, .box_w = 1, .box_h = 12, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 328, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 337, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 348, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 368, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 377, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 388, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 397, .adv_w = 128, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 409, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 418, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 427, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 436, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 445, .adv_w = 128, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 457, .adv_w = 128, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 469, .adv_w = 128, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 481, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 492, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 501, .adv_w = 128, .box_w = 4, .box_h = 15, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 509, .adv_w = 128, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 520, .adv_w = 128, .box_w = 4, .box_h = 15, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 528, .adv_w = 128, .box_w = 6, .box_h = 3, .ofs_x = 1, .ofs_y = 11},
    {.bitmap_index = 531, .adv_w = 128, .box_w = 8, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 532, .adv_w = 128, .box_w = 4, .box_h = 3, .ofs_x = 2, .ofs_y = 11},
    {.bitmap_index = 534, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 541, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 550, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 556, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 565, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 572, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 583, .adv_w = 128, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 592, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 601, .adv_w = 128, .box_w = 1, .box_h = 12, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 603, .adv_w = 128, .box_w = 4, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 610, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 619, .adv_w = 128, .box_w = 1, .box_h = 12, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 621, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 628, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 634, .adv_w = 128, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 641, .adv_w = 128, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 649, .adv_w = 128, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 657, .adv_w = 128, .box_w = 4, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 661, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 667, .adv_w = 128, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 677, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 683, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 689, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 697, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 703, .adv_w = 128, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 712, .adv_w = 128, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 718, .adv_w = 128, .box_w = 3, .box_h = 15, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 724, .adv_w = 128, .box_w = 1, .box_h = 17, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 727, .adv_w = 128, .box_w = 3, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 733, .adv_w = 128, .box_w = 6, .box_h = 3, .ofs_x = 1, .ofs_y = 12},
    {.bitmap_index = 736, .adv_w = 256, .box_w = 4, .box_h = 1, .ofs_x = 6, .ofs_y = 6},
    {.bitmap_index = 737, .adv_w = 256, .box_w = 6, .box_h = 1, .ofs_x = 5, .ofs_y = 6},
    {.bitmap_index = 738, .adv_w = 256, .box_w = 16, .box_h = 1, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 740, .adv_w = 256, .box_w = 13, .box_h = 1, .ofs_x = 2, .ofs_y = 6},
    {.bitmap_index = 742, .adv_w = 256, .box_w = 4, .box_h = 17, .ofs_x = 6, .ofs_y = -2},
    {.bitmap_index = 751, .adv_w = 256, .box_w = 2, .box_h = 5, .ofs_x = 12, .ofs_y = 9},
    {.bitmap_index = 753, .adv_w = 256, .box_w = 2, .box_h = 5, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 755, .adv_w = 256, .box_w = 6, .box_h = 5, .ofs_x = 9, .ofs_y = 9},
    {.bitmap_index = 759, .adv_w = 256, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 763, .adv_w = 256, .box_w = 8, .box_h = 2, .ofs_x = 4, .ofs_y = 5},
    {.bitmap_index = 765, .adv_w = 256, .box_w = 13, .box_h = 2, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 769, .adv_w = 256, .box_w = 14, .box_h = 15, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 796, .adv_w = 256, .box_w = 4, .box_h = 5, .ofs_x = 2, .ofs_y = 8},
    {.bitmap_index = 799, .adv_w = 256, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 804, .adv_w = 256, .box_w = 3, .box_h = 5, .ofs_x = 6, .ofs_y = 9},
    {.bitmap_index = 806, .adv_w = 256, .box_w = 12, .box_h = 13, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 826, .adv_w = 256, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 827, .adv_w = 256, .box_w = 4, .box_h = 5, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 830, .adv_w = 256, .box_w = 4, .box_h = 4, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 832, .adv_w = 256, .box_w = 6, .box_h = 6, .ofs_x = 5, .ofs_y = 3},
    {.bitmap_index = 837, .adv_w = 256, .box_w = 11, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 853, .adv_w = 256, .box_w = 12, .box_h = 13, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 873, .adv_w = 256, .box_w = 12, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 891, .adv_w = 256, .box_w = 5, .box_h = 15, .ofs_x = 8, .ofs_y = -2},
    {.bitmap_index = 901, .adv_w = 256, .box_w = 6, .box_h = 15, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 913, .adv_w = 256, .box_w = 7, .box_h = 15, .ofs_x = 8, .ofs_y = -2},
    {.bitmap_index = 927, .adv_w = 256, .box_w = 7, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 941, .adv_w = 256, .box_w = 5, .box_h = 14, .ofs_x = 8, .ofs_y = -1},
    {.bitmap_index = 950, .adv_w = 256, .box_w = 5, .box_h = 14, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 959, .adv_w = 256, .box_w = 5, .box_h = 13, .ofs_x = 8, .ofs_y = 0},
    {.bitmap_index = 968, .adv_w = 256, .box_w = 6, .box_h = 14, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 979, .adv_w = 256, .box_w = 6, .box_h = 13, .ofs_x = 8, .ofs_y = -1},
    {.bitmap_index = 989, .adv_w = 256, .box_w = 6, .box_h = 13, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 999, .adv_w = 256, .box_w = 14, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1022, .adv_w = 256, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1042, .adv_w = 256, .box_w = 4, .box_h = 15, .ofs_x = 8, .ofs_y = -2},
    {.bitmap_index = 1050, .adv_w = 256, .box_w = 5, .box_h = 15, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 1060, .adv_w = 256, .box_w = 7, .box_h = 13, .ofs_x = 7, .ofs_y = -1},
    {.bitmap_index = 1072, .adv_w = 256, .box_w = 7, .box_h = 13, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 1084, .adv_w = 256, .box_w = 6, .box_h = 6, .ofs_x = 9, .ofs_y = 7},
    {.bitmap_index = 1089, .adv_w = 256, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 1094, .adv_w = 256, .box_w = 2, .box_h = 14, .ofs_x = 7, .ofs_y = -2},
    {.bitmap_index = 1098, .adv_w = 256, .box_w = 8, .box_h = 14, .ofs_x = 5, .ofs_y = -1},
    {.bitmap_index = 1112, .adv_w = 256, .box_w = 11, .box_h = 15, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 1133, .adv_w = 256, .box_w = 10, .box_h = 14, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 1151, .adv_w = 256, .box_w = 11, .box_h = 14, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 1171, .adv_w = 256, .box_w = 13, .box_h = 4, .ofs_x = 2, .ofs_y = 8},
    {.bitmap_index = 1178, .adv_w = 256, .box_w = 13, .box_h = 6, .ofs_x = 2, .ofs_y = 6},
    {.bitmap_index = 1188, .adv_w = 256, .box_w = 13, .box_h = 9, .ofs_x = 2, .ofs_y = 3},
    {.bitmap_index = 1203, .adv_w = 256, .box_w = 12, .box_h = 15, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 1226, .adv_w = 256, .box_w = 13, .box_h = 13, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 1248, .adv_w = 256, .box_w = 1, .box_h = 11, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 1250, .adv_w = 256, .box_w = 4, .box_h = 4, .ofs_x = 6, .ofs_y = 7},
    {.bitmap_index = 1252, .adv_w = 256, .box_w = 10, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1266, .adv_w = 256, .box_w = 9, .box_h = 13, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 1281, .adv_w = 256, .box_w = 12, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1298, .adv_w = 256, .box_w = 11, .box_h = 13, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 1316, .adv_w = 256, .box_w = 2, .box_h = 4, .ofs_x = 7, .ofs_y = 7},
    {.bitmap_index = 1317, .adv_w = 256, .box_w = 5, .box_h = 14, .ofs_x = 10, .ofs_y = -1},
    {.bitmap_index = 1326, .adv_w = 256, .box_w = 4, .box_h = 14, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 1333, .adv_w = 256, .box_w = 10, .box_h = 10, .ofs_x = 3, .ofs_y = 1},
    {.bitmap_index = 1346, .adv_w = 256, .box_w = 12, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1364, .adv_w = 256, .box_w = 2, .box_h = 4, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 1365, .adv_w = 256, .box_w = 12, .box_h = 1, .ofs_x = 2, .ofs_y = 6},
    {.bitmap_index = 1367, .adv_w = 256, .box_w = 2, .box_h = 2, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1368, .adv_w = 256, .box_w = 6, .box_h = 13, .ofs_x = 5, .ofs_y = -1},
    {.bitmap_index = 1378, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 1391, .adv_w = 256, .box_w = 3, .box_h = 11, .ofs_x = 6, .ofs_y = 0},
    {.bitmap_index = 1396, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1409, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 1422, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 1435, .adv_w = 256, .box_w = 10, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1449, .adv_w = 256, .box_w = 10, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1463, .adv_w = 256, .box_w = 10, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1477, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1490, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 1503, .adv_w = 256, .box_w = 2, .box_h = 6, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1505, .adv_w = 256, .box_w = 2, .box_h = 8, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 1507, .adv_w = 256, .box_w = 12, .box_h = 10, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 1522, .adv_w = 256, .box_w = 12, .box_h = 4, .ofs_x = 2, .ofs_y = 4},
    {.bitmap_index = 1528, .adv_w = 256, .box_w = 12, .box_h = 10, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 1543, .adv_w = 256, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 1552, .adv_w = 256, .box_w = 12, .box_h = 13, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 1572, .adv_w = 256, .box_w = 10, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1586, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1599, .adv_w = 256, .box_w = 10, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1613, .adv_w = 256, .box_w = 10, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1627, .adv_w = 256, .box_w = 8, .box_h = 11, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 1638, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 1651, .adv_w = 256, .box_w = 10, .box_h = 12, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 1666, .adv_w = 256, .box_w = 9, .box_h = 10, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1678, .adv_w = 256, .box_w = 1, .box_h = 10, .ofs_x = 7, .ofs_y = 0},
    {.bitmap_index = 1680, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1693, .adv_w = 256, .box_w = 9, .box_h = 10, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1705, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 1718, .adv_w = 256, .box_w = 11, .box_h = 10, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1732, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1745, .adv_w = 256, .box_w = 10, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1759, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1772, .adv_w = 256, .box_w = 11, .box_h = 12, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 1789, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1802, .adv_w = 256, .box_w = 10, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1816, .adv_w = 256, .box_w = 10, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1830, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1843, .adv_w = 256, .box_w = 10, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1857, .adv_w = 256, .box_w = 14, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1877, .adv_w = 256, .box_w = 10, .box_h = 10, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1890, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1903, .adv_w = 256, .box_w = 9, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 1916, .adv_w = 256, .box_w = 5, .box_h = 14, .ofs_x = 9, .ofs_y = -1},
    {.bitmap_index = 1925, .adv_w = 256, .box_w = 9, .box_h = 14, .ofs_x = 4, .ofs_y = -1},
    {.bitmap_index = 1941, .adv_w = 256, .box_w = 5, .box_h = 15, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 1951, .adv_w = 256, .box_w = 6, .box_h = 3, .ofs_x = 5, .ofs_y = 9},
    {.bitmap_index = 1954, .adv_w = 256, .box_w = 16, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1956, .adv_w = 256, .box_w = 3, .box_h = 6, .ofs_x = 11, .ofs_y = 7},
    {.bitmap_index = 1959, .adv_w = 256, .box_w = 8, .box_h = 9, .ofs_x = 4, .ofs_y = -1},
    {.bitmap_index = 1968, .adv_w = 256, .box_w = 7, .box_h = 11, .ofs_x = 5, .ofs_y = -1},
    {.bitmap_index = 1978, .adv_w = 256, .box_w = 8, .box_h = 8, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 1986, .adv_w = 256, .box_w = 7, .box_h = 11, .ofs_x = 5, .ofs_y = 0},
    {.bitmap_index = 1996, .adv_w = 256, .box_w = 8, .box_h = 7, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 2003, .adv_w = 256, .box_w = 5, .box_h = 11, .ofs_x = 5, .ofs_y = 0},
    {.bitmap_index = 2010, .adv_w = 256, .box_w = 8, .box_h = 12, .ofs_x = 4, .ofs_y = -3},
    {.bitmap_index = 2022, .adv_w = 256, .box_w = 6, .box_h = 10, .ofs_x = 5, .ofs_y = 0},
    {.bitmap_index = 2030, .adv_w = 256, .box_w = 1, .box_h = 11, .ofs_x = 7, .ofs_y = 0},
    {.bitmap_index = 2032, .adv_w = 256, .box_w = 5, .box_h = 13, .ofs_x = 5, .ofs_y = -2},
    {.bitmap_index = 2041, .adv_w = 256, .box_w = 7, .box_h = 10, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 2050, .adv_w = 256, .box_w = 1, .box_h = 10, .ofs_x = 8, .ofs_y = 0},
    {.bitmap_index = 2052, .adv_w = 256, .box_w = 10, .box_h = 8, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 2062, .adv_w = 256, .box_w = 7, .box_h = 7, .ofs_x = 5, .ofs_y = 0},
    {.bitmap_index = 2069, .adv_w = 256, .box_w = 7, .box_h = 8, .ofs_x = 4, .ofs_y = -1},
    {.bitmap_index = 2076, .adv_w = 256, .box_w = 7, .box_h = 9, .ofs_x = 5, .ofs_y = -2},
    {.bitmap_index = 2084, .adv_w = 256, .box_w = 7, .box_h = 9, .ofs_x = 4, .ofs_y = -2},
    {.bitmap_index = 2092, .adv_w = 256, .box_w = 5, .box_h = 7, .ofs_x = 5, .ofs_y = 0},
    {.bitmap_index = 2097, .adv_w = 256, .box_w = 7, .box_h = 8, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 2104, .adv_w = 256, .box_w = 8, .box_h = 10, .ofs_x = 4, .ofs_y = -1},
    {.bitmap_index = 2114, .adv_w = 256, .box_w = 7, .box_h = 8, .ofs_x = 5, .ofs_y = 0},
    {.bitmap_index = 2121, .adv_w = 256, .box_w = 8, .box_h = 7, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 2128, .adv_w = 256, .box_w = 10, .box_h = 7, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 2137, .adv_w = 256, .box_w = 7, .box_h = 7, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 2144, .adv_w = 256, .box_w = 7, .box_h = 10, .ofs_x = 4, .ofs_y = -2},
    {.bitmap_index = 2153, .adv_w = 256, .box_w = 7, .box_h = 7, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 2160, .adv_w = 256, .box_w = 4, .box_h = 14, .ofs_x = 8, .ofs_y = -1},
    {.bitmap_index = 2167, .adv_w = 256, .box_w = 1, .box_h = 13, .ofs_x = 7, .ofs_y = -1},
    {.bitmap_index = 2169, .adv_w = 256, .box_w = 4, .box_h = 14, .ofs_x = 4, .ofs_y = -1},
    {.bitmap_index = 2176, .adv_w = 256, .box_w = 14, .box_h = 4, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 2183, .adv_w = 256, .box_w = 10, .box_h = 14, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 2201, .adv_w = 256, .box_w = 11, .box_h = 13, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 2219, .adv_w = 256, .box_w = 13, .box_h = 3, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 2224, .adv_w = 256, .box_w = 16, .box_h = 1, .ofs_x = 0, .ofs_y = 13},
    {.bitmap_index = 2226, .adv_w = 256, .box_w = 1, .box_h = 13, .ofs_x = 8, .ofs_y = -1},
    {.bitmap_index = 2228, .adv_w = 256, .box_w = 10, .box_h = 10, .ofs_x = 3, .ofs_y = 1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x3, 0x4, 0x5, 0x6, 0x8, 0x9, 0xc,
    0xd, 0x15, 0x16, 0x20, 0x22, 0x23, 0x25, 0x2b,
    0xff0, 0xff1, 0xff2, 0xff3
};

static const uint16_t unicode_list_3[] = {
    0x0, 0x1, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9,
    0xa, 0xb, 0xc, 0x21
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 8208, .range_length = 4084, .glyph_id_start = 96,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 20, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 12293, .range_length = 19, .glyph_id_start = 116,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 12317, .range_length = 34, .glyph_id_start = 135,
        .unicode_list = unicode_list_3, .glyph_id_ofs_list = NULL, .list_length = 12, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 65281, .range_length = 94, .glyph_id_start = 147,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 65504, .range_length = 6, .glyph_id_start = 241,
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
    .cmap_num = 6,
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
const lv_font_t lv_font_punct16 = {
#else
lv_font_t lv_font_punct16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 18,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if LV_FONT_PUNCT16*/

