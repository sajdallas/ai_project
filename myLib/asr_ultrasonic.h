/********************************************************************************
* @File name: asr_ultrasonic.h
* @Author: hhdd
* @Version: 1.0
* @Date: 2022-3-21
* @Description: 好好搭搭专用超声波模块库。
********************************************************************************/
#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_

#include "asr.h"

// #define ULTRASONIC_TIMER   (TIMER0)

class Ultrasonic
{
public:
    Ultrasonic(uint8_t pin);
    Ultrasonic(uint8_t trig, uint8_t echo);//四线超声波，收发不同引脚
    double getDistance(uint16_t MAXcm);
private:
    uint8_t _Ultrasonic_pin,_Ultrasonic_pin2;
    volatile bool _measureFlag;
    volatile long _lastEnterTime;
    volatile float _measureValue;
    uint64_t get_timer_value(void);
    unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
    void __delay(unsigned int cnt);
    long measure(unsigned long timeout);
};

//========================================================================
// 描述: 引脚初始化
// 参数: none.
// 返回: none.
//========================================================================
Ultrasonic::Ultrasonic(uint8_t pin)
{
    _Ultrasonic_pin = pin;
    _Ultrasonic_pin2 = pin;
    _measureFlag = true;
    _measureValue = 0;
    _lastEnterTime = 0;
}

inline Ultrasonic::Ultrasonic(uint8_t trig, uint8_t echo)
{
    _Ultrasonic_pin = trig;
    _Ultrasonic_pin2 = echo;
    _lastEnterTime = millis();
    _measureFlag = true;
    _measureValue = 0;
}

//========================================================================
// 描述: 延时函数.
// 参数: none
// 返回: none.
//========================================================================
void Ultrasonic::__delay(unsigned int cnt)
{
    volatile unsigned int j=0;
    volatile unsigned int i=0;
    for(i = 0; i < cnt; i++)
    {
        for(j = 0; j < 0x13; j++)
        {
            ;
        }
    }
}

//========================================================================
// 描述: 获取滴答计时器时间(单位：us).
// 参数: none
// 返回: none.
//========================================================================
uint64_t Ultrasonic::get_timer_value(void)
{
    while (1)
    {
        uint32_t hi = *(volatile uint32_t *)(TIMER_CTRL_ADDR + TIMER_MTIME + 4);
        uint32_t lo = *(volatile uint32_t *)(TIMER_CTRL_ADDR + TIMER_MTIME);
        if (hi == *(volatile uint32_t *)(TIMER_CTRL_ADDR + TIMER_MTIME + 4))
        {
            return (((uint64_t)hi << 32) | lo)/15;
        }
    }
}


//========================================================================
// 描述: 获取脉冲宽度
// 参数: none.
// 返回: none.
//========================================================================
unsigned long Ultrasonic::pulseIn(uint8_t pin, uint8_t state, unsigned long timeout)
{
    unsigned long start = get_timer_value();
    while (digitalRead(pin) != state) {
        if ((get_timer_value() - start) >= timeout) {
            return 0;
        }
    }
    start = get_timer_value();
    while (digitalRead(pin) == state) {
        if ((get_timer_value() - start) >= timeout) {
            return 0;
        }
    }
    return get_timer_value() - start;
}

//========================================================================
// 描述: 获取超声波值
// 参数: none.
// 返回: none.
//========================================================================
long Ultrasonic::measure(unsigned long timeout)
{
    unsigned int duration;
    unsigned int start;
    unsigned int count;
    // pinMode(_pin, output);    //设置为输出模式
#if defined(TW_ASR_PRO)
    set_pin_to_gpio_mode(_Ultrasonic_pin);
    if (_Ultrasonic_pin2 != _Ultrasonic_pin) {
        set_pin_to_gpio_mode(_Ultrasonic_pin2);
    }
#else
    if ((_Ultrasonic_pin <= 8) && (_Ultrasonic_pin >= 5)) //设置功能为GPIO
    {
        setPinFun(_Ultrasonic_pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_Ultrasonic_pin,FIRST_FUNCTION);
    }
    if (_Ultrasonic_pin2 != _Ultrasonic_pin) {
        if ((_Ultrasonic_pin2 <= 8) && (_Ultrasonic_pin2 >= 5)) //设置功能为GPIO
        {
            setPinFun(_Ultrasonic_pin2,SECOND_FUNCTION);
        } else {
            setPinFun(_Ultrasonic_pin2,FIRST_FUNCTION);
        }
    }
    #endif
    if (millis() - _lastEnterTime > 23)
    {
        _measureFlag = true;
    }

    if(_measureFlag == true)
    {
        _lastEnterTime = millis();
        _measureFlag = false;
        pinMode(_Ultrasonic_pin, OUTPUT);
        digitalWrite(_Ultrasonic_pin,0);   //拉低
        __delay(1);__delay(1);
        digitalWrite(_Ultrasonic_pin,1);
        __delay(10);
        digitalWrite(_Ultrasonic_pin,0);
        pinMode(_Ultrasonic_pin2, INPUT);
#ifndef ULTRASONIC_TIMER    //使用滴答
        duration = pulseIn(_Ultrasonic_pin2, 1, timeout);
        _measureValue = duration;

#else   //使用定时器
        Scu_SetDeviceGate(HAL_TIMER0_BASE,ENABLE);
        timer_init_t init;
        init.mode = timer_count_mode_free; //自由计数模式
        init.div = timer_clk_div_0; //不分频
        init.width = timer_iqr_width_2; //中断信号宽度定义，2 个时钟周期
        init.count = 0xFFFFFFFF;  //计数值
        timer_init(ULTRASONIC_TIMER,init); //初始化
        count = 0;
        // timer_set_count(ULTRASONIC_TIMER,count);
        timer_start(ULTRASONIC_TIMER);
        timer_get_count(ULTRASONIC_TIMER,&count);
        start = count;
        while (digitalRead(_pin) == 0) {
            timer_get_count(ULTRASONIC_TIMER,&count);
            if ((start - count) >= (timeout*90)) {
                return 0;
            }
        }
        timer_get_count(ULTRASONIC_TIMER,&count);
        start = count;
        while (digitalRead(_pin) == 1) {
            timer_get_count(ULTRASONIC_TIMER,&count);
            if ((start - count) >= (timeout*90)) {
                return 0;
            }
        }
        timer_stop(ULTRASONIC_TIMER);
        timer_get_count(ULTRASONIC_TIMER,&count);
        duration = (start - count)/90;
        _measureValue = duration;
#endif // !
    }
    else
    {
        duration = _measureValue;
    }
    return (duration);
}

//========================================================================
// 描述: 获取距离
// 参数: none.
// 返回: none.
//========================================================================
double Ultrasonic::getDistance(uint16_t MAXcm)
{
    taskENTER_CRITICAL();
    long distance = measure(MAXcm * 55 + 200);
    taskEXIT_CRITICAL();
    if(distance == 0)
    {
        distance = MAXcm * 58;
    }
    return((double)distance / 58.0);
}

#endif
