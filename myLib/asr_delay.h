#ifndef  _DELAY_H_
#define  _DELAY_H_

#include "asr.h"

void delayMicroseconds(uint32_t us);
uint64_t micros();

/* us计时器 */
uint64_t micros()
{
    uint32_t lo_temp=*(volatile uint32_t *)(TIMER_CTRL_ADDR + TIMER_MTIME);
    uint32_t hi_temp=*(volatile uint32_t *)(TIMER_CTRL_ADDR + TIMER_MTIME + 4);
    return (uint64_t)((((uint64_t)hi_temp << 32) | lo_temp)/15);
}

/* us级延时 */
void delayMicroseconds(uint32_t us)
{
    uint64_t t;
    t = micros();
    while(1)
    {
        if((micros()- t) >= us)
        {
            break;
        }
    }
}

#endif