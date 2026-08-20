#ifndef _ASR_SIMPLE_LIB_H
#define _ASR_SIMPLE_LIB_H

#include "asr.h"
#include "asr_hwtimer.h"
#include "SoftwareSerial.h"
#include "HardwareSerial.h"

typedef struct 
{
    uint8_t pin;    //引脚号
    uint8_t stu;    //引脚电平
    uint8_t isActive;
    volatile uint32_t ticks;
} pluse_t;
static pluse_t _pluse[25];      //保存脉冲参数的结构体数组

//========================================================================
// 描述: 定时器中断函数.
// 参数: none.
// 返回: none.
//========================================================================
void TIM_attachInterrupt_1()
{ 
    int i;
	for(i = 0; i < 25; i++)
    {
        if((_pluse[i].isActive = true)&&(_pluse[i].ticks > 0))  //脉冲信号
        {
            _pluse[i].ticks--;
            if(_pluse[i].ticks == 0)
            {
                _pluse[i].isActive = false;
                if(_pluse[i].stu == 0){
                    _pluse[i].stu == 1;
                    digitalWrite(_pluse[i].pin,1);    //拉高
                }else{
                    _pluse[i].stu == 0;
                    digitalWrite(_pluse[i].pin,0);    //拉低
                }
            }
        }
    }
}

//========================================================================
// 描述: 定时器1初始化(为脉冲信号提供计时).
// 参数: none.
// 返回: none.
//========================================================================
void asr_simple_timer_init()
{
    TIM_attachInterrupt(TIMER1, 1000, TIM_attachInterrupt_1);
    TIM_Enable(TIMER1);
    for(int i =0; i<8; i++)
    {
        _pluse[i].isActive = false;
        _pluse[i].ticks = 0;
    }
}

//========================================================================
// 描述: 产生脉冲.
// 参数: pin: 引脚; dig:高脉冲/低脉冲; nms:脉冲持续时间(单位毫秒).
// 返回: none.
//========================================================================
void asr_simple_pulse(uint8_t _pin, uint8_t dig,uint32_t nms)
{
    pinMode(_pin,output);    //设置为输出模式
    if((_pin<=8) && (_pin >=5)) //设置功能为GPIO
    {
        setPinFun(_pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_pin,FIRST_FUNCTION);
    }

    if(dig == 0)    //产生低脉冲
    {
        digitalWrite(_pin,1);    //先拉高
        delay(100);
        digitalWrite(_pin,0);
        _pluse[_pin].pin = _pin;
        _pluse[_pin].stu = 0;
        _pluse[_pin].ticks = nms;
        _pluse[_pin].isActive = true;
    }else{
        digitalWrite(_pin,0);    //先拉低
        delay(100);
        digitalWrite(_pin,1);
        _pluse[_pin].pin = _pin;
        _pluse[_pin].stu = 1;
        _pluse[_pin].ticks = nms;
        _pluse[_pin].isActive = true;
    }
}

//========================================================================
// 描述: 产生高低电平.
// 参数: pin: 引脚; dig:高/低电平.
// 返回: none.
//========================================================================
void asr_simple_digital_write(uint8_t pin, uint8_t dig)
{
    pinMode(pin,output);    //设置为输出模式
    if((pin<=8) && (pin >=5)) //设置功能为GPIO
    {
        setPinFun(pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(pin,FIRST_FUNCTION);
    }
    digitalWrite(pin,dig);
}

//========================================================================
// 描述: 产生PWM.
// 参数: pin: 引脚; freq:频率; duty:占空比.
// 返回: none.
//========================================================================
void asr_simple_set_pwm(uint8_t pin, uint32_t freq, uint8_t duty)
{
    pinMode(pin,output);    //设置为输出模式
    if(duty > 100)return;
    if(pin<=4) //设置功能为PWM
    {
        setPinFun(pin,SECOND_FUNCTION);
        PWM_enble((PWM0 + 0x1000 * pin),freq,100,duty);
    }else if((pin>=9)&&(pin<=14)){
        setPinFun(pin,SECOND_FUNCTION);
        PWM_enble((PWM0 + 0x1000 * (pin-9)),freq,100,duty);
    }else if((pin==7)||(pin==8)){
        setPinFun(pin,THIRD_FUNCTION);
        PWM_enble(PWM3,freq,100,duty);
    }else //其余引脚无PWM功能
    {
        return; 
    }  
}

#endif