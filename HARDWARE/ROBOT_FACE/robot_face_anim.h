#ifndef ROBOT_FACE_ANIM_H
#define ROBOT_FACE_ANIM_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ROBOT_IDLE = 0,
    ROBOT_LISTENING,
    ROBOT_THINKING,
    ROBOT_SPEAKING,
} robot_state_t;

void robot_face_anim_init(lv_obj_t *eye_l, lv_obj_t *eye_r,
                          lv_obj_t *pupil_l, lv_obj_t *pupil_r,
                          lv_obj_t *mouth, lv_obj_t *parent);
void robot_face_set_state(robot_state_t state);
void robot_face_tick(void);

#ifdef __cplusplus
}
#endif

#endif