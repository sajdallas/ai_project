#include "robot_face_anim.h"
#include <stdlib.h>
#include <string.h>

/* ---- 内部常量 ---- */
#define EYE_SIZE         36
#define PUPIL_SIZE       14
#define PUPIL_RANGE      7
#define BLINK_MIN_MS     2000
#define BLINK_MAX_MS     5000
#define BLINK_CLOSE_MS   80
#define BLINK_OPEN_MS    120
#define WANDER_MIN_MS    1200
#define WANDER_MAX_MS    2800
#define WANDER_DUR_MS    300
#define MOUTH_IDLE_W     80
#define MOUTH_IDLE_H     20
#define MOUTH_O_W        36
#define MOUTH_O_H        36
#define MOUTH_OPEN_H     36
#define MOUTH_CLOSE_H    8
#define MOUTH_SPEAK_W    60
#define SPEAK_PERIOD_MS  180

/* ---- 全局对象引用 ---- */
static lv_obj_t *g_eye_l, *g_eye_r, *g_pupil_l, *g_pupil_r, *g_mouth, *g_parent;
static robot_state_t g_state = ROBOT_IDLE;
static robot_state_t g_prev_state = ROBOT_IDLE;

/* ---- 眨眼 ---- */
static uint32_t g_next_blink;
static int g_blink_phase;   /* 0=idle, 1=closing, 2=opening */
static uint32_t g_blink_start;

/* ---- 眼球游走 ---- */
static int g_pupil_lx, g_pupil_ly;   /* 左瞳孔当前偏移 */
static int g_pupil_rx, g_pupil_ry;   /* 右瞳孔当前偏移 */
static uint32_t g_next_wander;
static int g_wander_anim;

/* ---- 说话 ---- */
static uint32_t g_speak_next;
static int g_speak_open;

/* ---- 瞳孔基准坐标（屏幕绝对坐标） ---- */
static int g_pupil_l_base_x, g_pupil_l_base_y;
static int g_pupil_r_base_x, g_pupil_r_base_y;

/* ==================== 内部函数 ==================== */

static void anim_set_height(void *obj, int32_t v)
{
    lv_obj_set_height((lv_obj_t *)obj, v);
}

static void blink_open_cb(lv_anim_t *a);

static void anim_set_x(void *obj, int32_t v)
{
    lv_obj_set_x((lv_obj_t *)obj, v);
}

static void anim_set_y(void *obj, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)obj, v);
}

static void blink_close_cb(lv_anim_t *a)
{
    (void)a;
    g_blink_phase = 2;
    g_blink_start = lv_tick_get();

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, g_eye_l);
    lv_anim_set_exec_cb(&anim, anim_set_height);
    lv_anim_set_values(&anim, 4, EYE_SIZE);
    lv_anim_set_time(&anim, BLINK_OPEN_MS);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
    lv_anim_set_ready_cb(&anim, blink_open_cb);
    lv_anim_start(&anim);

    lv_anim_init(&anim);
    lv_anim_set_var(&anim, g_eye_r);
    lv_anim_set_exec_cb(&anim, anim_set_height);
    lv_anim_set_values(&anim, 4, EYE_SIZE);
    lv_anim_set_time(&anim, BLINK_OPEN_MS);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
    lv_anim_start(&anim);
}

static void blink_open_cb(lv_anim_t *a)
{
    (void)a;
    g_blink_phase = 0;
    g_next_blink = lv_tick_get() + BLINK_MIN_MS + (rand() % (BLINK_MAX_MS - BLINK_MIN_MS));
}

static void wander_done_cb(lv_anim_t *a)
{
    (void)a;
    g_wander_anim = 0;
}

static void trigger_blink(void)
{
    g_blink_phase = 1;
    g_blink_start = lv_tick_get();

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, g_eye_l);
    lv_anim_set_exec_cb(&anim, anim_set_height);
    lv_anim_set_values(&anim, EYE_SIZE, 4);
    lv_anim_set_time(&anim, BLINK_CLOSE_MS);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&anim, blink_close_cb);
    lv_anim_start(&anim);

    lv_anim_init(&anim);
    lv_anim_set_var(&anim, g_eye_r);
    lv_anim_set_exec_cb(&anim, anim_set_height);
    lv_anim_set_values(&anim, EYE_SIZE, 4);
    lv_anim_set_time(&anim, BLINK_CLOSE_MS);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in);
    lv_anim_start(&anim);
}

static void trigger_wander(void)
{
    g_wander_anim = 1;
    g_next_wander = lv_tick_get() + WANDER_MIN_MS + (rand() % (WANDER_MAX_MS - WANDER_MIN_MS));

    int tx = (rand() % (PUPIL_RANGE * 2 + 1)) - PUPIL_RANGE;
    int ty = (rand() % (PUPIL_RANGE * 2 + 1)) - PUPIL_RANGE;

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, g_pupil_l);
    lv_anim_set_exec_cb(&anim, anim_set_x);
    lv_anim_set_values(&anim, g_pupil_l_base_x + g_pupil_lx, g_pupil_l_base_x + tx);
    lv_anim_set_time(&anim, WANDER_DUR_MS);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
    lv_anim_start(&anim);

    lv_anim_init(&anim);
    lv_anim_set_var(&anim, g_pupil_l);
    lv_anim_set_exec_cb(&anim, anim_set_y);
    lv_anim_set_values(&anim, g_pupil_l_base_y + g_pupil_ly, g_pupil_l_base_y + ty);
    lv_anim_set_time(&anim, WANDER_DUR_MS);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
    lv_anim_start(&anim);

    lv_anim_init(&anim);
    lv_anim_set_var(&anim, g_pupil_r);
    lv_anim_set_exec_cb(&anim, anim_set_x);
    lv_anim_set_values(&anim, g_pupil_r_base_x + g_pupil_rx, g_pupil_r_base_x + tx);
    lv_anim_set_time(&anim, WANDER_DUR_MS);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
    lv_anim_start(&anim);

    lv_anim_init(&anim);
    lv_anim_set_var(&anim, g_pupil_r);
    lv_anim_set_exec_cb(&anim, anim_set_y);
    lv_anim_set_values(&anim, g_pupil_r_base_y + g_pupil_ry, g_pupil_r_base_y + ty);
    lv_anim_set_time(&anim, WANDER_DUR_MS);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
    lv_anim_set_ready_cb(&anim, wander_done_cb);
    lv_anim_start(&anim);

    g_pupil_lx = tx;
    g_pupil_ly = ty;
    g_pupil_rx = tx;
    g_pupil_ry = ty;
}

static void mouth_set_shape(int w, int h, int radius)
{
    lv_obj_set_size(g_mouth, w, h);
    lv_obj_set_style_radius(g_mouth, radius, 0);
}

static void mouth_anim_idle(void)
{
    mouth_set_shape(MOUTH_IDLE_W, MOUTH_IDLE_H, 10);
}

static void mouth_anim_listening(void)
{
    mouth_set_shape(40, 12, 6);
}

static void mouth_anim_thinking(void)
{
    mouth_set_shape(MOUTH_O_W, MOUTH_O_H, LV_RADIUS_CIRCLE);
}

static void mouth_anim_speaking(void)
{
    if (g_speak_open) {
        mouth_set_shape(MOUTH_SPEAK_W, MOUTH_OPEN_H, 20);
    } else {
        mouth_set_shape(MOUTH_SPEAK_W, MOUTH_CLOSE_H, 4);
    }
}

static void state_enter(robot_state_t new_state)
{
    switch (new_state) {
    case ROBOT_IDLE:
        mouth_anim_idle();
        g_next_blink = lv_tick_get() + BLINK_MIN_MS + (rand() % (BLINK_MAX_MS - BLINK_MIN_MS));
        g_next_wander = lv_tick_get() + WANDER_MIN_MS + (rand() % (WANDER_MAX_MS - WANDER_MIN_MS));
        g_blink_phase = 0;
        g_wander_anim = 0;
        break;
    case ROBOT_LISTENING:
        mouth_anim_listening();
        break;
    case ROBOT_THINKING:
        mouth_anim_thinking();
        break;
    case ROBOT_SPEAKING:
        g_speak_open = 0;
        g_speak_next = lv_tick_get() + SPEAK_PERIOD_MS;
        mouth_anim_speaking();
        break;
    default:
        break;
    }
}

/* ==================== 公开函数 ==================== */

void robot_face_anim_init(lv_obj_t *eye_l, lv_obj_t *eye_r,
                          lv_obj_t *pupil_l, lv_obj_t *pupil_r,
                          lv_obj_t *mouth, lv_obj_t *parent)
{
    g_eye_l = eye_l;
    g_eye_r = eye_r;
    g_pupil_l = pupil_l;
    g_pupil_r = pupil_r;
    g_mouth = mouth;
    g_parent = parent;

    srand(lv_tick_get());

    g_state = ROBOT_IDLE;
    g_prev_state = ROBOT_IDLE;
    g_blink_phase = 0;
    g_wander_anim = 0;
    g_pupil_lx = 0;
    g_pupil_ly = 0;
    g_pupil_rx = 0;
    g_pupil_ry = 0;

    lv_obj_update_layout(g_parent);
    g_pupil_l_base_x = lv_obj_get_x(g_pupil_l);
    g_pupil_l_base_y = lv_obj_get_y(g_pupil_l);
    g_pupil_r_base_x = lv_obj_get_x(g_pupil_r);
    g_pupil_r_base_y = lv_obj_get_y(g_pupil_r);

    g_next_blink = lv_tick_get() + BLINK_MIN_MS + (rand() % (BLINK_MAX_MS - BLINK_MIN_MS));
    g_next_wander = lv_tick_get() + WANDER_MIN_MS + (rand() % (WANDER_MAX_MS - WANDER_MIN_MS));
}

void robot_face_set_state(robot_state_t state)
{
    if (state != g_state) {
        g_prev_state = g_state;
        g_state = state;
        state_enter(state);
    }
}

void robot_face_tick(void)
{
    uint32_t now = lv_tick_get();

    switch (g_state) {
    case ROBOT_IDLE:
        if (g_blink_phase == 0 && now >= g_next_blink) {
            trigger_blink();
        }
        if (!g_wander_anim && !g_blink_phase && now >= g_next_wander) {
            trigger_wander();
        }
        break;

    case ROBOT_LISTENING:
        if (g_blink_phase == 0 && now >= g_next_blink) {
            trigger_blink();
        }
        break;

    case ROBOT_THINKING:
        if (g_blink_phase == 0 && now >= g_next_blink) {
            trigger_blink();
        }
        break;

    case ROBOT_SPEAKING:
        if (now >= g_speak_next) {
            g_speak_open = !g_speak_open;
            mouth_anim_speaking();
            g_speak_next = now + SPEAK_PERIOD_MS;
        }
        break;

    default:
        break;
    }
}