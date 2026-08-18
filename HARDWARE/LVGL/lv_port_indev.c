#include "lv_port_indev.h"
#include "lvgl.h"
#include "touch.h"

static lv_indev_drv_t indev_drv;

static void touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    TP_Scan(0);
    if (tp_dev.sta & TP_PRES_DOWN) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = tp_dev.x[0];
        data->point.y = tp_dev.y[0];
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void lv_port_indev_init(void)
{
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv);
}