#ifndef _LED_BREATH_H_
#define _LED_BREATH_H_


#include "main.h"

#define LED_PWM_MAX_DUTY   1000U       /* 最大占空比（对应 ARR） */
#define LED_PWM_MIN_DUTY   0U          /* 最小占空比 */

void LED_Breath_Init(void);

void LED_PWM_SetDuty(uint16_t duty);
void LED_Breath_Update(uint32_t period_ms);

#endif