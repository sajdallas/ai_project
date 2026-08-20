/**
 * @file    asr_event.h
 * @brief   好搭ASRPRO简易事件库
 * @version 0.0.3
 * @date    2023-02-2
 * @author  hhdd
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 * 
 * 
 *  0.0.3 - 修改了将"十六"播报为"一十六"的bug.
 */
#ifndef _ASR_EVENT_H
#define _ASR_EVENT_H

#include <math.h>
#include "asr.h"
#include "asr_hwtimer.h"
//#include "SoftwareSerial.h"
#include "HardwareSerial.h"

/************************************************************************
  * @brief  语音播报完成回调函数.
  * @param  none
  * @return none
  **********************************************************************/
void play_end_callback()
{

}

/************************************************************************
  * @brief  设置引脚为输入模式.
  * @param  pin   引脚
  * @return none
  **********************************************************************/
void set_gpio_input(uint8_t pin)
{
    pinMode(pin,input);    //设置为输入模式
#if defined(TW_ASR_PRO)
    set_pin_to_gpio_mode(pin);
#else
    if((pin<=8) && (pin >=5))
    {
        setPinFun(pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(pin,FIRST_FUNCTION);
    }
#endif
}

/************************************************************************
  * @brief  读取高低电平.
  * @param  pin   引脚
  * @return none
  **********************************************************************/
uint8_t digital_read(uint8_t pin)
{
    return digitalRead(pin);
}

/************************************************************************
  * @brief  产生高低电平.
  * @param  pin   引脚
  * @param  dig   高/低电平
  * @return none
  **********************************************************************/
void digital_write(uint8_t pin, uint8_t dig)
{
    pinMode(pin,output);    //设置为输出模式
#if defined(TW_ASR_PRO)
    set_pin_to_gpio_mode(pin);
#else
    if((pin<=8) && (pin >=5))
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

/************************************************************************
  * @brief  设置全部引脚电平.
  * @param  s   高/低电平
  * @return none
  **********************************************************************/
void digital_write_all(uint8_t s)
{
    for(int i=0;i<26;i++){
        digital_write(i, s);
    }
}

/************************************************************************
  * @brief  产生PWM.
  * @param  pin     引脚
  * @param  freq    频率
  * @param  duty    占空比
  * @return none
  **********************************************************************/
void set_pwm(uint8_t pin, uint32_t freq, uint16_t duty)
{
    pinMode(pin,output);    //设置为输出模式
    if(duty > 1000)return;
    
    if(pin==0)
    {
        dpmu_set_adio_reuse(pinToFun[pin],DIGITAL_MODE);
        setPinFun(pin,SECOND_FUNCTION);
        PWM_enble(PWM5,freq,1000,duty);
    }
    if(pin>=2&&pin<=6)
    {
        setPinFun(pin,FIFTH_FUNCTION);
        if(pin<6){
            PWM_enble((PWM0 + 0x1000 * (pin-2)),freq,1000,duty);
        }else{
            PWM_enble((PWM4),freq,1000,duty);
        }
    }else if(pin>=7&&pin<=12)
    {
        setPinFun(pin,SECOND_FUNCTION);
        if(pin<11){
            PWM_enble((PWM0 + 0x1000 * (pin-7)),freq,1000,duty);
        }else{
            PWM_enble((PWM4 + 0x1000 * (pin-11)),freq,1000,duty);
        }
    }else if(pin>=13&&pin<=16)
    {
        setPinFun(pin,FORTH_FUNCTION);
        if(pin<16){
            PWM_enble((PWM1 + 0x1000 * (pin-13)),freq,1000,duty);
        }else{
            PWM_enble((PWM4),freq,1000,duty);
        }
    }else if(pin>=17&&pin<=20)
    {
        dpmu_set_adio_reuse(pinToFun[pin],DIGITAL_MODE);
        setPinFun(pin,FORTH_FUNCTION);
        PWM_enble((PWM3 - 0x1000 * (pin-17)),freq,1000,duty);
    } 
}

/************************************************************************
  * @brief  播报数值文本.
  * @param  s   需要播报的数值(字符串格式), 例如："123456".
  * @return none
  **********************************************************************/
void play_num(String s)
{
    uint8_t i = 0;
    uint8_t len = s.length(); //字符串长度
    for(i=0;i<len;i++)
    {
        if((s[i]>='0')&&(s[i]<='9')){
            s[i] = s[i]-'0';
        }else{
            return;
        }
    } 
    for(i=0;i<len;i++)
    {
        while(prompt_play_by_cmd_id(10084 + s[i], -1, play_end_callback, false)!=0){
            delay(6);
        }        
    } 
}

/************************************************************************
  * @brief  播报数值(最多可以播报2位小数).
  * @param  d       需要播报的数值,需要放大100被传入,程序内部会回复成正确的值播报.
  * @param  mode    1，以数值模式播报, 0以号码模式播报.
  * @return none
  **********************************************************************/
void play_num(int64_t d, uint8_t mode)
{
    uint8_t i,index = 0;
    volatile uint8_t arr[12],id[28];
    uint8_t temp_f;
    if( d < 0 ){
        d = -d;
        id[index++] = 15;
    }
    temp_f = d % 100; //小数部分
    d = d / 100;
    arr[0] = d / 100000000000 % 10;
    arr[1] = d / 10000000000 % 10;
    arr[2] = d / 1000000000 % 10;
    arr[3] = d / 100000000 % 10;
    arr[4] = d / 10000000 % 10;
    arr[5] = d / 1000000 % 10;
    arr[6] = d / 100000 % 10;
    arr[7] = d / 10000 % 10;
    arr[8] = d / 1000 % 10;
    arr[9] = d / 100 % 10;
    arr[10] = d / 10 % 10;
    arr[11] = d / 1 % 10;
    if(mode){
        if(d >= 100000000)  //1亿
        {
            if(arr[0] > 0){  //千亿位
                id[index++] = arr[0];
                id[index++] = 12;
            }
            if(arr[1] > 0){  //百亿位
                id[index++] = arr[1];
                id[index++] = 11;
            }else if((arr[1] == 0)&&(arr[2] != 0)&&((d / 100000000 % 10000) > 1000)){
                id[index++] = arr[4];
            }
            if(arr[2] > 0){    //十亿位
                id[index++] = arr[2];
                id[index++] = 10;     
            }else if((arr[2] == 0)&&(arr[3] != 0)&&((d / 100000000 % 10000) > 100)){
                id[index++] = arr[2];
            }
            if(arr[3] > 0){    //亿位
                id[index++] = arr[3];
            }else if((d / 100000000 % 10000) == 0){
                id[index++] = 0;
            } 
            id[index++] = 14;
        }
        if(d >= 10000)  //1万
        {
            if((d / 10000 % 10000) != 0){   //中间4位不为0
                if((arr[4] == 0) && (d >= 100000000)){     //千万位为0且有亿位则需要播报
                    id[index++] = 0; 
                }
                if(arr[4] > 0){  //千万位
                    id[index++] = arr[4];
                    id[index++] = 12;
                }
                if(arr[5] > 0){  //百万位
                    id[index++] = arr[5];
                    id[index++] = 11;
                }else if((arr[5] == 0)&&(arr[6] != 0)&&((d / 10000 % 10000) > 1000)){
                    id[index++] = arr[5];
                }
                if(arr[6] > 0){   //十万位
                    id[index++] = arr[6];
                    id[index++] = 10;     
                }else if((arr[6] == 0)&&(arr[7] != 0)&&((d / 10000 % 10000) > 100)){
                    id[index++] = arr[6];
                }
                if(arr[7] > 0){   //万位
                    id[index++] = arr[7];
                }else if((d / 10000 % 1000) == 0){
                    id[index++] = 0;
                } 
                id[index++] = 13;  //万
            }
        }
        if((d % 10000) != 0){   //最低4位不为0
            if((arr[8] == 0)&&(d >= 10000)){   //千位为0且有万位则需要播报
                id[index++] = 0;
            }
            if(arr[8] > 0){  //千位
                id[index++] = arr[8];
                id[index++] = 12;
            }
            if(arr[9] > 0){  //百位
                id[index++] = arr[9];
                id[index++] = 11;
            }else if((arr[9] == 0)&&(arr[10] != 0)&&((d / 1 % 10000) > 1000)){
                id[index++] = arr[9];
            }
            if((arr[10]==1)&&(d<20)){       //当数大于10小于20时应播报 十几，而不是一十几
                id[index++] = 10;

            }else if(arr[10] > 0){   //十位
                id[index++] = arr[10];
                id[index++] = 10;    
            }else if((arr[10] == 0)&&(arr[11] != 0)&&((d / 1 % 10000) > 100)){
                id[index++] = arr[10];
            }
            if(arr[11] > 0){   //个位
                id[index++] = arr[11];
            }
        }
        if(d == 0){
            id[index++] = 0;
        }

        if(temp_f == 0){
            //读取完毕
        }else if(temp_f < 10){
            id[index++] = 16;
            id[index++] = 0;
            id[index++] = temp_f%10;
        }else if(temp_f >= 10){
            id[index++] = 16;
            id[index++] = temp_f/10%10;
            id[index++] = temp_f/1%10;
        }
    }else{
        for(i=0; i < 12;i++){
            if(arr[i] != 0){
                break;
            }
        }
        if(i == 12){
            id[index++] = 0;
        }else{
            for(;i<12;i++){
                id[index++] = arr[i];
            }
        }
        if(temp_f == 0){
            //读取完毕
        }else if(temp_f < 10){
            id[index++] = 16;
            id[index++] = 0;
            id[index++] = temp_f%10;
        }else if(temp_f >= 10){
            id[index++] = 16;
            id[index++] = temp_f/10%10;
            id[index++] = temp_f/1%10;
        }
    }
    for(i=0;i<index;i++){
        while(prompt_play_by_cmd_id(10084 + id[i], -1, play_end_callback, false)!=0){
            delay(6);
        }
    }
}

/************************************************************************
  * @brief  播报语音(不会打断正在播报的语音).
  * @param  id    需要播报id.
  *         注意：使用唤醒词第一次切换到唤醒状态时不会播报。
  *              可以通过将播报音加入命令词信息表的“<1>wake”下的方式播报。
  * @return none
  **********************************************************************/
void play_audio(uint16_t id)
{
    while(prompt_play_by_cmd_id(id, -1, play_end_callback, false)!=0){
        delay(6);
    }
}

/***************************************************************
 * 描述: 设置模块为一直唤醒状态
 * 参数: none
 * 返回: none
 * *************************************************************/
void _awaken(){
    delay(3000);
    while (1) {
        delay(500);
        enter_wakeup(1000000);
        delay(4000);
    }
    vTaskDelete(NULL);
}

void set_wakeup_forever()
{
    xTaskCreate(_awaken,"_awaken",128,NULL,4,NULL); 
}

/**************************************  产生脉冲  **************************************/
typedef struct 
{
    uint8_t pin;    //引脚号
    uint8_t stus;   //产生高脉冲还是低脉冲
    volatile uint32_t ticks;    //脉冲时间
    volatile uint16_t count;    //脉冲次数
    volatile uint32_t t;        //当前脉冲时间计数
} pluse_t;
static pluse_t _pluse[30];      //保存脉冲参数的结构体数组

void stimer_8(TimerHandle_t xTimer);
TimerHandle_t softtimer_8 = NULL;

/************************************************************************
  * @brief  定时器中断回调函数.
  * @param  none
  * @return none
  **********************************************************************/
void stimer_8(TimerHandle_t xTimer)
{ 
    int i;
	for(i = 0; i < 30; i++)
    {
        if((_pluse[i].count > 0)&&(_pluse[i].ticks > 0))  //脉冲信号
        {
            _pluse[i].t--;
            if(_pluse[i].t == 0)    //定时时间到
            {
                _pluse[i].count--;
                if(_pluse[i].count == 0){       //脉冲全部发送完毕
                    _pluse[i].ticks = 0;
                }else{
                    _pluse[i].t    = _pluse[i].ticks;
                    _pluse[i].stus = !_pluse[i].stus;       //取反
                    digitalWrite(_pluse[i].pin, _pluse[i].stus);
                }
            }
        }
    }
}

/************************************************************************
  * @brief  初始化(为脉冲信号提供计时).
  * @param  none
  * @return none
  **********************************************************************/
void pulse_init()
{
    softtimer_8 = xTimerCreate("stimer_8",1,1,0,stimer_8);
    xTimerStart(softtimer_8,0);
    for(uint8_t i =0; i<30; i++)
    {
        _pluse[i].ticks = 0;
        _pluse[i].count = 0;
    }
}

/************************************************************************
  * @brief   产生脉冲.
  * @param  _pin    引脚
  * @param  dig     高/低脉冲
  * @param  nms     脉冲持续时间(单位毫秒)
  * @param  c       脉冲次数
  * @return none
  **********************************************************************/
void set_pulse(uint8_t _pin, uint8_t dig, uint32_t nms, uint32_t c=1)
{
    pinMode(_pin,output);    //设置为输出模式
#if defined(TW_ASR_PRO)
    set_pin_to_gpio_mode(_pin);
#else
    if((_pin<=8) && (_pin >=5))
    {
        setPinFun(_pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_pin,FIRST_FUNCTION);
    }
#endif
    digitalWrite(_pin,!dig);    //如果为高脉冲先拉低100ms,否则拉高100ms
    delay(100);
    digitalWrite(_pin,dig);
    _pluse[_pin].pin = _pin;
    _pluse[_pin].stus = dig;
    nms = nms / 2;
    _pluse[_pin].ticks = nms;
    _pluse[_pin].t = nms;
    _pluse[_pin].count = c*2;
}

#endif
