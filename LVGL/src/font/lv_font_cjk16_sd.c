#include "lv_font_cjk16_sd.h"
#include "lvgl.h"
#include "ff.h"
#include "malloc.h"
#include "exfuns.h"
#include "sdio_sdcard.h"
#include <string.h>

#define FONT_BIN_PATH "0:/cjk16_font.bin"

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t total_size;
    uint32_t sec_offset[7];
    uint32_t sec_size[7];
} font_bin_header_t;

static uint8_t *g_font_buffer = NULL;
static lv_font_t *g_font = NULL;
static uint8_t g_fs_ready = 0;

static int font_fs_init(void)
{
    if (g_fs_ready) return 1;
    if (fs[0] == NULL) return 0;
    if (SD_Init() != SD_OK) return 0;
    if (f_mount(fs[0], "0:", 1) != FR_OK) return 0;
    g_fs_ready = 1;
    return 1;
}

static void *fixup_ptr(uint32_t marker, uint8_t *base, const font_bin_header_t *hdr)
{
    if (marker == 0xFFFFFFFF) return NULL;
    uint32_t sec_idx = marker & 0xFFFF;
    if (sec_idx >= 7) return NULL;
    return base + hdr->sec_offset[sec_idx];
}

lv_font_t *lv_font_cjk16_sd_init(void)
{
    FIL file;
    UINT br;
    FRESULT res;
    font_bin_header_t hdr;

    if (g_font) return g_font;
    if (!font_fs_init()) return NULL;

    res = f_open(&file, FONT_BIN_PATH, FA_READ);
    if (res != FR_OK) return NULL;

    res = f_read(&file, &hdr, sizeof(hdr), &br);
    if (res != FR_OK || br != sizeof(hdr)) {
        f_close(&file);
        return NULL;
    }

    if (hdr.magic != 0x314B4A43) { /* "CJK1" */
        f_close(&file);
        return NULL;
    }

    if (!mallco_dev.memrdy[SRAMEX]) {
        f_close(&file);
        return NULL;
    }

    g_font_buffer = (uint8_t *)mymalloc(SRAMEX, hdr.total_size);
    if (!g_font_buffer) {
        f_close(&file);
        return NULL;
    }

    f_lseek(&file, 0);
    res = f_read(&file, g_font_buffer, hdr.total_size, &br);
    f_close(&file);

    if (res != FR_OK || br != hdr.total_size) {
        myfree(SRAMEX, g_font_buffer);
        g_font_buffer = NULL;
        return NULL;
    }

    font_bin_header_t *loaded_hdr = (font_bin_header_t *)g_font_buffer;

    lv_font_fmt_txt_cmap_t *cmaps = (lv_font_fmt_txt_cmap_t *)(g_font_buffer + loaded_hdr->sec_offset[5]);
    for (int i = 0; i < 8; i++) {
        uint32_t ul_marker = *(uint32_t *)&cmaps[i].unicode_list;
        cmaps[i].unicode_list = (const uint16_t *)fixup_ptr(ul_marker, g_font_buffer, loaded_hdr);
        uint32_t go_marker = *(uint32_t *)&cmaps[i].glyph_id_ofs_list;
        cmaps[i].glyph_id_ofs_list = fixup_ptr(go_marker, g_font_buffer, loaded_hdr);
    }

    lv_font_fmt_txt_dsc_t *font_dsc = (lv_font_fmt_txt_dsc_t *)(g_font_buffer + loaded_hdr->sec_offset[6]);
    {
        uint32_t bm_marker = *(uint32_t *)&font_dsc->glyph_bitmap;
        font_dsc->glyph_bitmap = (const uint8_t *)fixup_ptr(bm_marker, g_font_buffer, loaded_hdr);
        uint32_t gd_marker = *(uint32_t *)&font_dsc->glyph_dsc;
        font_dsc->glyph_dsc = (const lv_font_fmt_txt_glyph_dsc_t *)fixup_ptr(gd_marker, g_font_buffer, loaded_hdr);
        uint32_t cm_marker = *(uint32_t *)&font_dsc->cmaps;
        font_dsc->cmaps = (const lv_font_fmt_txt_cmap_t *)fixup_ptr(cm_marker, g_font_buffer, loaded_hdr);
        uint32_t kern_marker = *(uint32_t *)&font_dsc->kern_dsc;
        font_dsc->kern_dsc = fixup_ptr(kern_marker, g_font_buffer, loaded_hdr);
        uint32_t cache_marker = *(uint32_t *)&font_dsc->cache;
        font_dsc->cache = fixup_ptr(cache_marker, g_font_buffer, loaded_hdr);
    }

    g_font = (lv_font_t *)mymalloc(SRAMEX, sizeof(lv_font_t));
    if (!g_font) {
        myfree(SRAMEX, g_font_buffer);
        g_font_buffer = NULL;
        return NULL;
    }

    memset(g_font, 0, sizeof(lv_font_t));
    g_font->get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt;
    g_font->get_glyph_bitmap = lv_font_get_bitmap_fmt_txt;
    g_font->line_height = 18;
    g_font->base_line = 3;
    g_font->subpx = LV_FONT_SUBPX_NONE;
    g_font->underline_position = -1;
    g_font->underline_thickness = 1;
    g_font->dsc = font_dsc;
    g_font->fallback = NULL;
    g_font->user_data = NULL;

    return g_font;
}
