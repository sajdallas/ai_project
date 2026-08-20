/********************************************************************************
* @File name: asr_hc595.h
* @Author: hhdd
* @Version: 1.0
* @Date: 2022-08-01
* @Description: hc595串转并模块库。
********************************************************************************/
#ifndef __ASR_HC595_H
#define __ASR_HC595_H	
#ifndef TW_ASR_PRO
#include "asr_delay.h"	
#endif
	

class HC595
{
private:
    uint8_t _si_pin;
    uint8_t _rck_pin;
    uint8_t _sck_pin;
    void send_byte(uint8_t outdata);
    void out_enable();  
public:
    HC595(int8_t si_pin, int8_t rck_pin, int8_t sck_pin);
    void begin();
    void send_multi_byte(uint8_t *ddata,uint16_t len);
};

HC595::HC595(int8_t si_pin, int8_t rck_pin, int8_t sck_pin)
{
    _si_pin = si_pin;
    _rck_pin = rck_pin;
    _sck_pin = sck_pin;
}

//========================================================================
// 描述: 595初始化.
// 参数: none.
// 返回: none.
//========================================================================
void HC595::begin()
{
    pinMode(_si_pin, OUTPUT);
    pinMode(_rck_pin, OUTPUT);
    pinMode(_sck_pin, OUTPUT);
#if defined(TW_ASR_PRO)
    set_pin_to_gpio_mode(_si_pin);
    set_pin_to_gpio_mode(_rck_pin);
    set_pin_to_gpio_mode(_sck_pin);   
#else
    if((_si_pin<=8) && (_si_pin >=5))
    {
        setPinFun(_si_pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_si_pin,FIRST_FUNCTION);
    }
    if((_rck_pin<=8) && (_rck_pin >=5))
    {
        setPinFun(_rck_pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_rck_pin,FIRST_FUNCTION);
    }
    if((_sck_pin<=8) && (_sck_pin >=5))
    {
        setPinFun(_sck_pin,SECOND_FUNCTION);
    }
    else
    {
        setPinFun(_sck_pin,FIRST_FUNCTION);
    }
#endif
    digitalWrite(_si_pin,0);
    digitalWrite(_sck_pin,0);
    digitalWrite(_rck_pin,0);  
}

//========================================================================
// 描述: 595使能输出.
// 参数: none.
// 返回: none.
//========================================================================
void HC595::out_enable()
{
    digitalWrite(_rck_pin,0);
    delayMicroseconds(20);   
    digitalWrite(_rck_pin,1);
    delayMicroseconds(20);    
    digitalWrite(_rck_pin,0);
}

//========================================================================
// 描述: 595发送8位数据.
// 参数: 8位数据.
// 返回: none.
//========================================================================
void HC595::send_byte(uint8_t outdata)
{
    uint8_t i;
    for(i=0;i<8;i++)  //将8位数据按位发送，先发高字节再发低字节
	{      
        if((outdata&0x80)==0x80)
	    {
            digitalWrite(_si_pin,1);
        }
	    else
	    {
            digitalWrite(_si_pin,0);
        }
        //时钟线低电平
        digitalWrite(_sck_pin,0);
          //时钟线高电平
        digitalWrite(_sck_pin,1);
        outdata = outdata<<1;
    }
}

//========================================================================
// 描述: 595发送数组.
// 参数: 数组地址，数据长度.
// 返回: none.
//========================================================================
void HC595::send_multi_byte(uint8_t *ddata,uint16_t len)
{
    uint16_t i;
    for(i=0;i<len;i++)
	{
        send_byte(ddata[i]);
    }
    out_enable();
}
#endif
