//鹿小班语音识别IP库
//版权方:长葛市鹿小班信息技术工作室

#ifndef _LUXIAOBAN_H
#define _LUXIAOBAN_H

#include "asr.h"
#include "asr_dhtxx.h"
#include "asr_ds18b20.h"
#include "asr_hwtimer.h"
#include "asr_servo.h"
#include "HardwareSerial.h"

typedef struct 
{
    uint8_t pin;    //引脚号
    uint8_t stu;    //引脚电平
    uint8_t isActive;
    volatile uint32_t ticks;
} pluse_t;
static pluse_t _pluse[8];      //保存脉冲参数的结构体数组

typedef struct 
{
    uint8_t pin;    //引脚号
    uint8_t angle;
    volatile uint32_t ticks;
} Servo_t;

//========================================================================
// 描述: 定时器中断函数.
// 参数: none.
// 返回: none.
//========================================================================
void TIM_attachInterrupt_1()
{ 
    int i;
	for(i = 0; i < 8; i++)
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
// 描述: 定时器1初始化(为脉冲和舵机信号提供计时).
// 参数: none.
// 返回: none.
//========================================================================
void luxiaoban_timer_init()
{
    TIM_attachInterrupt(TIMER1, 10000, TIM_attachInterrupt_1);
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
void luxiaoban_pulse(uint8_t _pin, uint8_t dig,uint32_t nms)
{
    pinMode(_pin,output);    //设置为输出模式
#if defined(TW_ASR_PRO)
    set_pin_to_gpio_mode(_pin);
#else
    if((_pin<=8) && (_pin >=5)) //设置功能为GPIO
    {
        setPinFun(_pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_pin,FIRST_FUNCTION);
    }
#endif
    if(dig == 0)    //产生低脉冲
    {
        digitalWrite(_pin,1);    //先拉高
        delay(100);
        digitalWrite(_pin,0);
        _pluse[_pin].pin = _pin;
        _pluse[_pin].stu = 0;
        _pluse[_pin].ticks = nms / 10;
        _pluse[_pin].isActive = true;
    }else{
        digitalWrite(_pin,0);    //先拉低
        delay(100);
        digitalWrite(_pin,1);
        _pluse[_pin].pin = _pin;
        _pluse[_pin].stu = 1;
        _pluse[_pin].ticks = nms / 10;
        _pluse[_pin].isActive = true;
    }
}

//========================================================================
// 描述: 读取高低电平.
// 参数: pin: 引脚.
// 返回: none.
//========================================================================
uint8_t luxiaoban_digital_read(uint8_t pin)
{
    pinMode(pin,input);    //设置为输入模式
#if defined(TW_ASR_PRO)
    set_pin_to_gpio_mode(pin);
#else
    if((pin<=8) && (pin >=5)) //设置功能为GPIO
    {
        setPinFun(pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(pin,FIRST_FUNCTION);
    }
#endif
    return digitalRead(pin);
}

//========================================================================
// 描述: 产生高低电平.
// 参数: pin: 引脚; dig:高/低电平.
// 返回: none.
//========================================================================
void luxiaoban_digital_write(uint8_t pin, uint8_t dig)
{
    pinMode(pin,output);    //设置为输出模式
#if defined(TW_ASR_PRO)
    set_pin_to_gpio_mode(pin);
#else
    if((pin<=8) && (pin >=5)) //设置功能为GPIO
    {
        setPinFun(pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(pin,FIRST_FUNCTION);
    }
#endif
    digitalWrite(pin,dig);
}

//========================================================================
// 描述: 设置所有引脚电平.
// 参数: s:1高电平,0低电平.
// 返回: none.
//========================================================================
void luxiaoban_digital_write_all(uint8_t s)
{
    for(int i=0;i<8;i++){
        luxiaoban_digital_write(i, s);
    }
}

//========================================================================
// 描述: 产生PWM.
// 参数: pin: 引脚; freq:频率; duty:占空比.
// 返回: none.
//========================================================================
void luxiaoban_set_pwm(uint8_t pin, uint32_t freq, uint8_t duty)
{
    pinMode(pin,output);    //设置为输出模式
    if(duty > 100)return;
#if defined(TW_ASR_PRO)
    if(pin==0)
    {
     dpmu_set_adio_reuse(pinToFun[pin],DIGITAL_MODE);
     setPinFun(pin,SECOND_FUNCTION);
     PWM_enble(PWM5,freq,100,duty);
    }
    if(pin>=2&&pin<=6)
    {
        setPinFun(pin,FIFTH_FUNCTION);
        if(pin<6){
        PWM_enble((PWM0 + 0x1000 * (pin-2)),freq,1000,duty);
        }else{
        PWM_enble((PWM4),freq,100,duty);
        }
    }else if(pin>=7&&pin<=12)
    {
        setPinFun(pin,SECOND_FUNCTION);
        if(pin<11){
        PWM_enble((PWM0 + 0x1000 * (pin-7)),freq,100,duty);
        }else{
        PWM_enble((PWM4 + 0x1000 * (pin-11)),freq,100,duty);

        }
    }else if(pin>=13&&pin<=16)
    {
        setPinFun(pin,FORTH_FUNCTION);
        if(pin<16){
        PWM_enble((PWM1 + 0x1000 * (pin-13)),freq,100,duty);
        }else{
        PWM_enble((PWM4),freq,1000,duty);
        }
    }else if(pin>=17&&pin<=20)
    {
        dpmu_set_adio_reuse(pinToFun[pin],DIGITAL_MODE);
        setPinFun(pin,FORTH_FUNCTION);
        PWM_enble((PWM3 - 0x1000 * (pin-17)),freq,100,duty);
    } 
#else
    if(pin<=3) //设置功能为PWM
    {
        setPinFun(pin,SECOND_FUNCTION);
        PWM_enble((PWM0 + 0x1000 * pin),freq,100,duty);
    }else if(pin == 4){
        setPinFun(pin,SECOND_FUNCTION);
        PWM_enble(PWM2,freq,100,duty);
    }else if((pin==7)){
        setPinFun(pin,THIRD_FUNCTION);
        PWM_enble(PWM3,freq,100,duty);
    }else 
    {
        return; //P05和P06脚无PWM功能
    }  
#endif
}

//========================================================================
// 描述: 播报温湿度.
// 参数: temp:温度(为255则跳过温度播报); humidity:湿度(为255则跳过湿度播报).
// 返回: none.
//========================================================================
void luxiaoban_speech(float temp, uint8_t humidity)
{
    int temp_int, humidity_int;
    int index;
    int i,arr[4],id[15];
    index = 0;
    
    for(i=0;i<4;i++){
        arr[i] = 0;
    }

    if(temp < 255)
    {
        if(temp >= 0)    
        {
            temp_int = 10 * temp; //将温度值放大10倍
            id[index++] = 25; //播报“当前温度为”
        }else{
            temp_int = -10 * temp; //将温度值放大10倍,并转为正数
            id[index++] = 28; //播报“当前温度为零下”
        }

        for(i =  0; temp_int !=0; i++)
        {
            arr[i] = temp_int%10;
            temp_int /= 10;
        }
        
        if(i == 4)
        {
            id[index++] = arr[3]; //百位数
            id[index++] = 11; //百
            if(arr[2] > 0){
                id[index++] = arr[2]; //十位数
                id[index++] = 10; //十
            }else{
                id[index++] = 27; //零
            }
            if(arr[1] > 0){
                id[index++] = arr[1]; //个位数
            }
            if(arr[0] > 0){
                id[index++] = 24; //点
                id[index++] = arr[0]; //第一位小数
            }
        }else if(i == 3)
        {
            id[index++] = arr[2]; //十位数
            id[index++] = 10; //十
            if(arr[1] > 0){
                id[index++] = arr[1]; //个位数
            }
            if(arr[0] > 0){
                id[index++] = 24; //点
                id[index++] = arr[0]; //第一位小数
            }       
        }else if((i == 2)||(i == 1))
        {
            if(arr[1] == 0){
                id[index++] = 27;
            }else{
                id[index++] = arr[1]; //个位数
            }
            if(arr[0] > 0){
                id[index++] = 24; //点
                id[index++] = arr[0]; //第一位小数
            }  
        }
        id[index++] = 13; //播报摄氏度
    }

    if((humidity <= 100)&&(humidity >= 0))
    {
        for(i=0;i<4;i++){
            arr[i] = 0;
        }
        humidity_int = humidity * 10; //将湿度值放大10倍
        for(i =  0; humidity_int !=0; i++)   //123.4
        {
            arr[i] = humidity_int%10;
            humidity_int /= 10;
        }
        id[index++] = 26; //播报“湿度百分之”

        if(humidity >= 100)
        {
            id[index++] = arr[3]; //百位数
            id[index++] = 11; //百
            if(arr[2] > 0){
                id[index++] = arr[2]; //十位数
                id[index++] = 10; //十
            }else{
                id[index++] = 27; //零
            }
            if(arr[1] > 0){
                id[index++] = arr[1]; //个位数
            }
            if(arr[0] > 0){
                id[index++] = 24; //点
                id[index++] = arr[0]; //第一位小数
            }
        }else if(humidity >= 10)
        {
            id[index++] = arr[2]; //十位数
            id[index++] = 10; //十
            if(arr[1] > 0){
                id[index++] = arr[1]; //个位数
            }
            if(arr[0] > 0){
                id[index++] = 24; //点
                id[index++] = arr[0]; //第一位小数
            }       
        }else if(humidity > 0)
        {
            id[index++] = arr[1]; //个位数
            if(arr[0] > 0){
                id[index++] = 24; //点
                id[index++] = arr[0]; //第一位小数
            }  
        }else if(humidity == 0){
            id[index++] = 27; //个位数
        }
    }

    for(i = 0; i < index; i++){
        while (prompt_play_by_cmd_id(id[i], -1, NULL, false))
        {    
            delay(1);
        }   
    }
}

#endif