#include "lv_port_disp.h"
#include "lvgl.h"
#include "lcd.h"

static lv_disp_drv_t disp_drv;
static lv_disp_draw_buf_t draw_buf;

static lv_color_t buf1[240 * 20];
static lv_color_t buf2[240 * 20];

static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    LCD_Color_Fill(area->x1, area->y1, area->x2, area->y2, (u16 *)color_p);
    lv_disp_flush_ready(drv);
}

void lv_port_disp_init(void)
{
    LCD_Clear(0x0000);
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, 240 * 20);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 320;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
}