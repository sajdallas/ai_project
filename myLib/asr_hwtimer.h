/**
 * @file asr_hwtimer.h
 * @brief 硬件定时器模块驱动
 * @version 0.1
 * @date 2021-06-29
 * @author Blue
 * @copyright Copyright (c) 2019  haohaodada Technology Co., Ltd.
 * 
 */
 
#ifndef _ASR_HWTIMER_H_
#define _ASR_HWTIMER_H_

#if defined(TW_ASR_PRO)
#include "asrpro_core_eclic.h"
#include "asrpro_scu.h"
#include "asrpro_timer.h"
#else
#include "asr.h"
#include "asr_timer.h"
#endif


typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr tim_fun[4] = { NULL };

typedef struct {
  uint32_t delay_time;
  timer_base_t timer;
} htparams_t;

volatile htparams_t htparams[4]; 

/**
 * @brief timer中断处理函数
 *
 */
#ifdef __cplusplus
extern "C"{
#endif

static void ht_timer0_timeout_deal(void)
{
  timer_clear_irq(htparams[0].timer);

  if(tim_fun[0]!=NULL)tim_fun[0]();//执行用户函数\

  timer_set_count(htparams[0].timer,htparams[0].delay_time);//继续下一次
  timer_start(htparams[0].timer);
}

static void ht_timer1_timeout_deal(void)
{
  timer_clear_irq(htparams[1].timer);

  if(tim_fun[1]!=NULL)tim_fun[1]();//执行用户函数\

  timer_set_count(htparams[1].timer,htparams[1].delay_time);//继续下一次
  timer_start(htparams[1].timer);
}

static void ht_timer2_timeout_deal(void)
{
  timer_clear_irq(htparams[2].timer);

  if(tim_fun[2]!=NULL)tim_fun[2]();//执行用户函数\

  timer_set_count(htparams[2].timer,htparams[2].delay_time);//继续下一次
  timer_start(htparams[2].timer);
}

static void ht_timer3_timeout_deal(void)
{
  timer_clear_irq(htparams[3].timer);

  if(tim_fun[3]!=NULL)tim_fun[3]();//执行用户函数\

  timer_set_count(htparams[3].timer,htparams[3].delay_time);//继续下一次
  timer_start(htparams[3].timer);
}


#ifdef __cplusplus
} // extern "C"
#endif

/**
  * @brief  定时器配置
  * @param  定时器（TIMER0、TIMER1、TIMER2、TIMER3）
  * @param  定时周期 最大47s  单位us
  * @param  回调函数
  * @retval 
  */
void TIM_attachInterrupt(timer_base_t timer, uint32_t period, void (*userFunc)(void))
{
  uint8_t _timerx_irq;

  timer_init_t httimer_init;
  httimer_init.mode  = timer_count_mode_auto;
  httimer_init.div   = timer_clk_div_0;
  httimer_init.width = timer_iqr_width_f;

  if(timer == TIMER0)
  {
    _timerx_irq = TIMER0_IRQn;
    tim_fun[0] = userFunc;
    htparams[0].timer = timer;
    //ONE:APB=90M 计数器为32位4294967296 最大周期为 4294.967296/90=47.72185884444444 s 
    //PRO:APB=110.592M 计数器为32位4294967296 最大周期为 4294.967296/110.592=38.8361481481 s 
    //如需更大时间，请设置div分频
    htparams[0].delay_time = get_apb_clk()/1000000*period;

    //配置TIMER的中断
#if defined(TW_ASR_PRO)
    scu_set_device_gate(htparams[0].timer,ENABLE); //打开外设时钟
#else
    Scu_SetDeviceGate(htparams[0].timer,ENABLE);
#endif
    __eclic_irq_set_vector(_timerx_irq,(int)ht_timer0_timeout_deal);
    eclic_irq_enable(_timerx_irq);
    
    httimer_init.count = htparams[0].delay_time;
    timer_init(htparams[0].timer,httimer_init);
    //timer_start(htparams[0].timer);
  }
  else if(timer == TIMER1)
  {
    _timerx_irq = TIMER1_IRQn;
    tim_fun[1] = userFunc;
    htparams[1].timer = timer;
    //如需更大时间，请设置div分频
    htparams[1].delay_time = get_apb_clk()/1000000*period;

    //配置TIMER的中断
#if defined(TW_ASR_PRO)
    scu_set_device_gate(htparams[1].timer,ENABLE); //打开外设时钟
#else
    Scu_SetDeviceGate(htparams[1].timer,ENABLE);
#endif
    __eclic_irq_set_vector(_timerx_irq,(int)ht_timer1_timeout_deal);
    eclic_irq_enable(_timerx_irq);
    
    httimer_init.count = htparams[1].delay_time;
    timer_init(htparams[1].timer,httimer_init);
    //timer_start(htparams[1].timer);
  }
  else if(timer == TIMER2)
  {
    _timerx_irq = TIMER2_IRQn;
    tim_fun[2] = userFunc;
    htparams[2].timer = timer;
    //如需更大时间，请设置div分频
    htparams[2].delay_time = get_apb_clk()/1000000*period;

    //配置TIMER的中断
#if defined(TW_ASR_PRO)
    scu_set_device_gate(htparams[2].timer,ENABLE); //打开外设时钟
#else
    Scu_SetDeviceGate(htparams[2].timer,ENABLE);
#endif
    __eclic_irq_set_vector(_timerx_irq,(int)ht_timer2_timeout_deal);
    eclic_irq_enable(_timerx_irq);
    
    httimer_init.count = htparams[2].delay_time;
    timer_init(htparams[2].timer,httimer_init);
    //timer_start(htparams[2].timer);
  }
  else if(timer == TIMER3)
  {
    _timerx_irq = TIMER3_IRQn;
    tim_fun[3] = userFunc;
    htparams[3].timer = timer;
    //如需更大时间，请设置div分频
    htparams[3].delay_time = get_apb_clk()/1000000*period;

    //配置TIMER的中断
#if defined(TW_ASR_PRO)
    scu_set_device_gate(htparams[3].timer,ENABLE); //打开外设时钟
#else
    Scu_SetDeviceGate(htparams[3].timer,ENABLE);
#endif
    __eclic_irq_set_vector(_timerx_irq,(int)ht_timer3_timeout_deal);
    eclic_irq_enable(_timerx_irq);
    
    httimer_init.count = htparams[3].delay_time;
    timer_init(htparams[3].timer,httimer_init);
    //timer_start(htparams[3].timer);
  }
}

/**
  * @brief  更新周期
  * @param  timer（TIMER0、TIMER1、TIMER2、TIMER3）
  * @param  period 周期
  * @retval 
  */
void TIM_Duty_Updata(timer_base_t timer, uint32_t period)
{
  //APB=90M 计数器为32位4294967296 最大周期为 4294.967296/90=47.72185884444444 s 
  //如需更大时间，请设置div分频
  if(timer == TIMER0)
  {
    htparams[0].delay_time = get_apb_clk()/1000000*period;
    htparams[0].timer = timer;
  }
  else if(timer == TIMER1)
  {
    htparams[1].delay_time = get_apb_clk()/1000000*period;
    htparams[1].timer = timer;
  }
  else if(timer == TIMER2)
  {
    htparams[2].delay_time = get_apb_clk()/1000000*period;
    htparams[2].timer = timer;
  }
  else if(timer == TIMER3)
  {
    htparams[3].delay_time = get_apb_clk()/1000000*period;
    htparams[3].timer = timer;
  }
}

/**
  * @brief  关闭定时器
  * @param  定时器（TIMER0、TIMER1、TIMER2、TIMER3）
  * @retval 
  */
void TIM_Disable(timer_base_t timer)
{
  timer_stop(timer);
}

/**
  * @brief  启动定时器
  * @param  定时器（TIMER0、TIMER1、TIMER2、TIMER3）
  * @retval 
  */
void TIM_Enable(timer_base_t timer)
{
  timer_start(timer);
}

/**
  * @brief  定时器解除占用
  * @param  定时器（TIMER0、TIMER1、TIMER2、TIMER3）
  * @retval 
  */
void TIM_detachInterrupt(timer_base_t timer)
{
  timer_stop(timer);

  if(timer == TIMER0)
  {
    tim_fun[0] = NULL;
  }
  else if(timer == TIMER1)
  {
    tim_fun[1] = NULL;
  }
  else if(timer == TIMER2)
  {
    tim_fun[2] = NULL;
  }
  else if(timer == TIMER3)
  {
    tim_fun[3] = NULL;
  }
}

#endif //_ASR_HWTIMER_H_