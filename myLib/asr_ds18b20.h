/********************************************************************************
* @File name: asr_ds18b20.h
* @Author: hhdd
* @Version: 1.0
* @Date: 2022-6-21
* @Description: ds18b20驱动库。
********************************************************************************/
#ifndef _ASR_DS18B20_H
#define _ASR_DS18B20_H

#include "asr.h"
#ifndef TW_ASR_PRO
#include "asr_delay.h"
#endif

class DS18B20
{
public:
    DS18B20(uint8_t  pin);
    void init(void);
	float read_temperature(); //读取温度

private:
    uint8_t _pin;
    uint8_t first_read = 0; //标记是否为第一次读取
    uint8_t reset(); //总线复位
    uint8_t read_one_char(); //读一个字节数据
	void write_one_char(uint8_t dat); //写一个字节数据 
    float read_temp();
};

//========================================================================
// 描述: 设置引脚.
// 参数: none.
// 返回: none.
//========================================================================
DS18B20::DS18B20(uint8_t pin)
{
    _pin = pin;
}

//========================================================================
// 描述: 引脚初始化函数.
// 参数: none.
// 返回: none.
//========================================================================
void DS18B20::init(void)
{
#if defined(TW_ASR_PRO)
    set_pin_to_gpio_mode(_pin);
	dpmu_set_io_pull((PinPad_Name)pinToFun[_pin],DPMU_IO_PULL_UP);
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
}

//========================================================================
// 描述: 总线复位.
// 参数: none.
// 返回: none.
//========================================================================
uint8_t DS18B20::reset(void)
{
    uint8_t r;
    uint8_t retries = 125;

    pinMode(_pin,output);
	digitalWrite(_pin,1);
	delayMicroseconds(8);
    pinMode(_pin,INPUT);
    do {
	    if (retries-- == 0)
        {
            return 0;
        } 
	    delayMicroseconds(2);
    } while( !digitalRead( _pin));

    pinMode(_pin,output);
    digitalWrite(_pin,0);   // pull low for 500uS
	delayMicroseconds(520);
    // digitalWrite(_pin,1); 
    pinMode(_pin,INPUT);
	delayMicroseconds(60);
    r = !digitalRead(_pin);
	delayMicroseconds(520);
    return r;
}

//========================================================================
// 描述: 读一个字节数据.
// 参数: none.
// 返回: none.
//========================================================================
uint8_t DS18B20::read_one_char(void) 
{ 
    uint8_t i=0; 
    uint8_t dat = 0; 
    
    for (i=8;i>0;i--) 
    {  
        pinMode(_pin,output);
        digitalWrite(_pin,0); // 给脉冲信号 
        delayMicroseconds(2);   //保证主机拉低总线1μs以上 
        dat>>=1; 
        digitalWrite(_pin,1);
        pinMode(_pin,INPUT);
        delayMicroseconds(8);
        if(digitalRead(_pin))     //主机对总线采样的数 判断 
            dat|=0x80; 
        delayMicroseconds(50);  //等待读操作结束 
    } 
    return(dat); 
} 

//========================================================================
// 描述: 写一个字节数据.
// 参数: none.
// 返回: none.
//========================================================================
void DS18B20::write_one_char(unsigned char dat) 
{ 
    unsigned char i=0; 
    pinMode(_pin,output);
    for (i=8; i>0; i--) 
    {
        digitalWrite(_pin,0);   
        delayMicroseconds(5);
        digitalWrite(_pin,(dat&0x01));  //向总线写入数据 
        delayMicroseconds(70);
        digitalWrite(_pin,1); 
        delayMicroseconds(2);
        dat>>=1; 
    } 
} 

//========================================================================
// 描述: 读温度(内部调用).
// 参数: none.
// 返回: none.
//========================================================================
float DS18B20::read_temp() 
{ 
    // reset(); 
    // write_one_char(0xCC);     // 跳过读序号列号的操作 
    // write_one_char(0x4E);  // //写入"写暂存器"命令,修改TH和TL和分辩率配置寄存器 
    // //先写TH,再写TL,最后写配置寄存器 
    // write_one_char(0x6e); //写入想设定的温度报警上限 
    // write_one_char(0x00); //写入想设定的温度报警下限 
    // write_one_char(0x3f); //写配置寄存器,格式为0 R1 R0 1,1 1 1 1 
    // //R1R0=00分辨率娄9位,R1R0=11分辨率为12位  */ 
    // delayMicroseconds(80);       // this message is wery important 
    uint8_t low_byte,high_byte;
    int16_t TReading, SignBit,Tc_100;

    // taskENTER_CRITICAL();   //进入临界保护
    reset(); 
    write_one_char(0xCC);  // 跳过读序号列号的操作 
    write_one_char(0x44);  // 启动温度转换 
	// delayMicroseconds(600);
    reset(); 
    write_one_char(0xCC);  //跳过读序号列号的操作 
    write_one_char(0xBE);  //读取温度寄存器等（共可读9个寄存器） 前两个就是温度 
    // delayMicroseconds(600); 

    low_byte=read_one_char();     //读取温度值低位 
    high_byte=read_one_char();    //读取温度值高位 

    TReading = (high_byte << 8) + low_byte;
	SignBit = TReading & 0x8000;	// test most sig bit

	if (SignBit)			// negative
	{
		TReading = (TReading ^ 0xffff) + 1;	// 2's comp
		TReading=0-TReading;

	}
	Tc_100 = (6 * TReading) + TReading / 4;
    // taskEXIT_CRITICAL();    //退出临界保护
    return (float)Tc_100/100.0;	 
} 

//========================================================================
// 描述: 读温度.
// 参数: none.
// 返回: none.
//========================================================================
float DS18B20::read_temperature() 
{
    float temp;
    if(first_read == 0)
    {
        first_read = 1;
        // taskENTER_CRITICAL();   //进入临界保护
        temp = read_temp();
        // taskEXIT_CRITICAL();    //退出临界保护
        delay(50);
    }
    taskENTER_CRITICAL();   //进入临界保护
    temp = read_temp();
    taskEXIT_CRITICAL();    //退出临界保护
    return temp;
}


#endif